#include <pulse/stream.h>
#include "PlaybackStream.h"
#include "BufferAttributes.h"
#include "PulseAudioHandler.h"
#include "MainLoopLocker.h"
#include <QDebug>

using namespace pulse;


PlaybackStream::PlaybackStream(ContextPtr ctx, SampleSpecification* sample,
                BufferAttributes* buffAttr, ChannelMapPtr map)
    : BasicStream(ctx,sample,buffAttr,map)
{
    MainLoopLocker lock(PulseAudioHandler::instance().mainLoop());

    pa_stream_set_write_callback(stream(), nullptr, NullData);

    if (pa_stream_connect_playback(stream(), BasicDevice, bufferAttributes()->get(),
                                   PlaybStreamFlags, nullptr, nullptr) != 0)
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

PlaybackStream::~PlaybackStream()
{

}

void PlaybackStream::resume()
{
    pa_stream_cork(stream(), 0, 0, PulseAudioHandler::instance().mainLoop());
}

void PlaybackStream::pause()
{
    pa_stream_cork(stream(), 1, 0, PulseAudioHandler::instance().mainLoop());
}

