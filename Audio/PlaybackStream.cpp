#include <pulse/stream.h>
#include <pulse/error.h>
#include <pulse/thread-mainloop.h>
#include <QDebug>
#include <stdexcept>

#include "IHandler.h"
#include "PlaybackStream.h"
#include "BufferAttributes.h"
#include "MainLoopLocker.h"
#include "Settings/Settings.h"
#include "Network/NetSocket.h"

using namespace pulse;


PlaybackStream::PlaybackStream(IHandler* h, const QString &n, ContextPtr ctx, SampleSpecification* sample,
                               BufferAttributes* buffAttr, ChannelMapPtr map, NetSocket* sock)
    : BasicStream(h, n, ctx, sample, buffAttr, map, sock)
    , m_mutex()
    , m_queueBuffer()
    , m_kit(DataKit(&m_queueBuffer, &m_mutex), h)
{

}

int PlaybackStream::initialize()
{
    BasicStream::initialize();

    MainLoopLocker lock(mainLoop());

    if (Settings::instance().value(Settings::usePlaybackAsyncAccessModel).toBool())
    {
        pa_stream_set_write_callback(stream(), PlaybackStream::writeAsyncAccess, static_cast<void*>(&m_kit));
        socket()->setReceiveMethod(std::bind(&PlaybackStream::receiveData, this));
    }
    else
        socket()->setReceiveMethod(std::bind(&PlaybackStream::writePolledAccess, this));

    int err = pa_stream_connect_playback(stream(), BasicDevice, bufferAttributes()->get(),
                                            PlaybStreamFlags, nullptr, nullptr);
    if ( err != 0)
    {
        qDebug() << name() << ": not connected" << pa_strerror(err) << err;

        if (err < 0)
        {
           // Old pulse audio servers don't like the ADJUST_LATENCY flag, so retry without that
           err = pa_stream_connect_playback(stream(), BasicDevice, bufferAttributes()->get(),
                                               PlaybStreamFlags2, nullptr, nullptr);
           if ( err != 0)
           {
               qDebug() << name() << ": not connected" << pa_strerror(err) << err;
                return 1;
           }
        }
        else
            return 1;
    }

    pa_threaded_mainloop_wait(mainLoop());
    return 0;
}

PlaybackStream::~PlaybackStream()
{

}

void PlaybackStream::resume()
{
    pa_stream_cork(stream(), 0, 0, mainLoop());
}

void PlaybackStream::pause()
{
    pa_stream_cork(stream(), 1, 0, mainLoop());
}

void PlaybackStream::writePolledAccess()
{
    static const int16_t fragsize = pulse::Settings::instance().value(pulse::Settings::bufferFragSize).toInt();

    QByteArray buffer(fragsize, 0);
    size_t len  = socket()->receive(buffer.data(), fragsize);
    if (len <= 0)
        return;

    MainLoopLocker lock(mainLoop());
    size_t requestedBytes = pa_stream_writable_size(stream());
    if (len > requestedBytes)  len = requestedBytes;

    uint8_t* b;
    pa_stream_begin_write(stream(), reinterpret_cast<void**>(&b), &len);

    for (size_t i = 0; i < len; ++i) b[i] = (buffer.data())[i];

    pa_stream_write(stream(), b, len, nullptr, 0, PA_SEEK_RELATIVE_END);
}

void PlaybackStream::receiveData()
{
    static const int16_t fragsize = pulse::Settings::instance().value(pulse::Settings::bufferFragSize).toInt();
    QByteArray buffer(fragsize, 0);

    int len  = socket()->receive(buffer.data(), buffer.size());

    if (len < 0)
        return;

    QMutexLocker lock(&m_mutex);
    for (int i = 0; i < len; ++i) m_queueBuffer.push_back((buffer.data())[i]);
}

void PlaybackStream::writeAsyncAccess(StreamPtr stream, size_t requestedBytes, void* kit)
{
    AsyncKit* acynsKit = static_cast<AsyncKit*>(kit);

    QQueue<uint8_t>* queueBuffer = acynsKit->first.first;
    QMutex* mutex = acynsKit->first.second;
    IHandler* handler = acynsKit->second;

    QMutexLocker lock(mutex);
    MainLoopLocker lock2(handler->basicStreamByStream(stream)->mainLoop());

    if (queueBuffer->empty())
    {
        pa_stream_flush(stream,nullptr,nullptr);
        return;
    }

    uint8_t* b;
    size_t bytesToFill = queueBuffer->size();
    if (bytesToFill > requestedBytes)  bytesToFill = requestedBytes;
    pa_stream_begin_write(stream, reinterpret_cast<void**>(&b), &bytesToFill);

    for (size_t i = 0; i < bytesToFill; ++i)
    {
        b[i] = queueBuffer->front();
        queueBuffer->pop_front();
    }

    lock.unlock();

    pa_stream_write(stream, b, bytesToFill, nullptr, 0, PA_SEEK_RELATIVE);
}
