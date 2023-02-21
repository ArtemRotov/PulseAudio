#pragma once
#include "IStream.h"
#include "def.h"

class NetSocket;

namespace pulse
{
    class IHandler;

    class BasicStream : public IStream
    {
    public:
        explicit BasicStream(IHandler* h, const QString &n, ContextPtr ctx, SampleSpecification* sample,
                             BufferAttributes* buffAttr, ChannelMapPtr map, NetSocket* sock);
        ~BasicStream();

        QString name() const;

        virtual int initialize();

        MainLoopPtr mainLoop() const;

        StreamPtr stream() const;
        BufferAttributes* bufferAttributes() const;

        NetSocket* socket() const;
        void setSocket(NetSocket* sock);

    protected:
    private:
        static void streamStateCallBack(StreamPtr s, void *userData);

        IHandler*               m_handler;

        QString                 m_name;

        NetSocket*              m_sock;

        ContextPtr              m_context;
        SampleSpecification*    m_sampleSpec;
        BufferAttributes*       m_bufferAttr;
        ChannelMapPtr           m_channelMap;

        StreamPtr               m_stream;
    };

}



