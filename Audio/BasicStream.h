#pragma once
#include "IStream.h"
#include "def.h"

namespace pulse
{
    class BasicStream : public IStream
    {
    public:
        explicit BasicStream(ContextPtr ctx, SampleSpecification* sample,
                             BufferAttributes* buffAttr, ChannelMapPtr map);
        ~BasicStream();

        StreamPtr stream() const;
        BufferAttributes* bufferAttributes() const;

    protected:
    private:
        void init();

        ContextPtr              m_context;
        SampleSpecification*    m_sampleSpec;
        BufferAttributes*       m_bufferAttr;
        ChannelMapPtr           m_channelMap;

        StreamPtr               m_stream;
    };

}



