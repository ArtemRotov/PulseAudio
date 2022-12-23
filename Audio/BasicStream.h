#pragma once
#include "IStream.h"
#include "def.h"

class NetSocket;

namespace pulse
{
    class BasicStream : public IStream
    {
    public:
        explicit BasicStream(ContextPtr ctx, SampleSpecification* sample,
                             BufferAttributes* buffAttr, ChannelMapPtr map, NetSocket* sock);
        ~BasicStream();

        StreamPtr stream() const;
        BufferAttributes* bufferAttributes() const;

        NetSocket* socket() const;
        void setSocket(NetSocket* sock);

    protected:
    private:
        void init();

        NetSocket*              m_sock;

        ContextPtr              m_context;
        SampleSpecification*    m_sampleSpec;
        BufferAttributes*       m_bufferAttr;
        ChannelMapPtr           m_channelMap;

        StreamPtr               m_stream;
    };

}



