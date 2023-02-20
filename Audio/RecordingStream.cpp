#include <pulse/stream.h>
#include <pulse/error.h>
#include "RecordingStream.h"
#include "BufferAttributes.h"
#include "PulseAudioHandler.h"
#include "MainLoopLocker.h"
#include "Settings/Settings.h"
#include "Network/NetSocket.h"

using namespace pulse;


RecordingStream::RecordingStream(const QString &n, ContextPtr ctx, SampleSpecification* sample,
                                 BufferAttributes* buffAttr, ChannelMapPtr map, NetSocket* sock)
    : BasicStream(n, ctx, sample, buffAttr, map, sock)
    , m_consumers()
    , m_kit(this, &m_consumers)
{

}

int RecordingStream::initialize()
{
    BasicStream::initialize();

    MainLoopLocker lock(PulseAudioHandler::instance().mainLoop());

    if (Settings::instance().value(Settings::useRecordAsyncAccessModel).toBool())
        pa_stream_set_read_callback(stream(), RecordingStream::read, static_cast<void*>(&m_kit));
    else
        pa_stream_set_read_callback(stream(), nullptr, NullData);

    int err = pa_stream_connect_record(stream(), BasicDevice, bufferAttributes()->get(), RecStreamFlags);
    if ( err != 0)
    {
        qDebug() << name() << ": not connected" << pa_strerror(err) << err;

        return 1;
    }

    pa_threaded_mainloop_wait(PulseAudioHandler::instance().mainLoop());
    return 0;
}

RecordingStream::~RecordingStream()
{

}

void RecordingStream::resume()
{
    pa_stream_cork(stream(), 0, 0, PulseAudioHandler::instance().mainLoop());
}

void RecordingStream::pause()
{
    pa_stream_cork(stream(), 1, 0, PulseAudioHandler::instance().mainLoop());
}

void RecordingStream::read(StreamPtr stream, size_t nbytes, void* kit)
{
    Q_UNUSED(nbytes);

    AsyncKit* params = static_cast<AsyncKit*>(kit);

    RecordingStream* thisStream = params->first;
    const Consumers* consumers = params->second;

    while (true)
    {
        const void* data;
        size_t n;
        pa_stream_peek(stream, &data, &n);

        if (data == nullptr && n == 0)
            return;

        else if (data == nullptr && n != 0)
        {
            qDebug() << "RecordingStream: buffer overrun occurred";
            break;
        }
        else
        {
            static const int16_t fragsize = pulse::Settings::instance().value(pulse::Settings::bufferFragSize).toInt();

            uint8_t* buffer = (uint8_t*)data;
            for (size_t i = 0; i < n;)
            {
                int bytes = ((n - i) < (size_t)fragsize) ? n - i : fragsize;

                for (const Consumer &s: *consumers)
                    thisStream->socket()->send((void*)buffer, bytes, s.address, s.port);

                i += bytes;
                buffer += bytes;
            }
        }

        pa_stream_drop(stream);
    }
}

void RecordingStream::addConsumer(const QString &addr, int port)
{
    m_consumers.append(Consumer(addr,port));
}
