#pragma once
#include "BasicStream.h"
#include "QQueue"

namespace pulse
{
    class PlaybackStream : public BasicStream
    {
    public:
        explicit PlaybackStream(ContextPtr ctx, SampleSpecification* sample,
                                 BufferAttributes* buffAttr, ChannelMapPtr map);
        ~PlaybackStream();

        void resume() override;
        void pause() override;

    protected:
    private:
        QQueue<uint8_t> m_buffer;

    };


}

