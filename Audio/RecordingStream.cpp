#include <pulse/stream.h>
#include "RecordingStream.h"
#include "BufferAttributes.h"
#include "PulseAudioHandler.h"
#include "MainLoopLocker.h"
#include <QDebug>

using namespace pulse;


RecordingStream::RecordingStream(ContextPtr ctx, SampleSpecification* sample,
                BufferAttributes* buffAttr, ChannelMapPtr map)
    : BasicStream(ctx,sample,buffAttr,map)
    , m_subscribers()
{
    MainLoopLocker lock(PulseAudioHandler::instance().mainLoop());

    pa_stream_set_read_callback(stream(), RecordingStream::read, static_cast<void*>(&m_subscribers));

    if (pa_stream_connect_record(stream(), BasicDevice, bufferAttributes()->get(), RecStreamFlags) != 0)
        qDebug() << "Recording Stream is not connected";
    while (true)
    {
        int ret = pa_stream_get_state(stream());
        if (ret == PA_STREAM_READY)
            break;
        else if (ret == PA_STREAM_FAILED)
        {
            qDebug() << "PA_STREAM_FAILED";
            return;
        }
        pa_threaded_mainloop_wait(PulseAudioHandler::instance().mainLoop());
    }
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

void RecordingStream::read(StreamPtr stream, size_t nbytes, void* subscr)
{
    const Subscribers* subscribers = static_cast<Subscribers*>(subscr);

    while (true)
    {
        const void* data;
        size_t n;
        pa_stream_peek(stream, &data, &n);

        if (data == nullptr && n == 0)
            return;                                     // Buffer is empty. Process more events
        else if (data == nullptr && n != 0)
        {
            qDebug() << "RecordingStream: buffer overrun occurred";
            break;
        }
        else
        {
//            uint8_t* ptr = (uint8_t*)data;
//            for (int i = 0; i < n;)
//            {
//                int bytes = ((n - i) < 1024) ? n - i : 1024;
//                sock->send((void*)ptr,bytes, addr, 1234);
//                //qDebug() << "Sended " << bytes << " bytes";
//                i += bytes;
//                ptr = ptr + bytes;
//            }
        }

        pa_stream_drop(stream);
    }
}
