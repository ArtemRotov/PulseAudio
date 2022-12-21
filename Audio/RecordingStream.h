#pragma once
#include <QStringList>
#include "BasicStream.h"

namespace pulse
{
    class RecordingStream : public BasicStream
    {
    public:
        explicit RecordingStream(ContextPtr ctx, SampleSpecification* sample,
                                 BufferAttributes* buffAttr, ChannelMapPtr map);
        ~RecordingStream();

        static void read(StreamPtr stream, size_t nbytes, void* subscr);

        void resume() override;
        void pause() override;

    protected:
    private:
        Subscribers m_subscribers;
    };
}
