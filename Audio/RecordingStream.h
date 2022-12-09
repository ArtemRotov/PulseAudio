#pragma once
#include "BasicStream.h"


namespace pulse
{
    class RecordingStream : public BasicStream
    {
    public:
        explicit RecordingStream(ContextPtr ctx, SampleSpecification* sample,
                                 BufferAttributes* buffAttr, ChannelMapPtr map);
        ~RecordingStream();

        void resume() override;
        void pause() override;

    protected:
    private:
    };


}
