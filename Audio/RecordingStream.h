#pragma once
#include <QStringList>
#include "BasicStream.h"

class NetSocket;

namespace pulse
{
    class RecordingStream : public BasicStream
    {
    public:
        explicit RecordingStream(ContextPtr ctx, SampleSpecification* sample,
                                 BufferAttributes* buffAttr, ChannelMapPtr map, NetSocket* sock);
        ~RecordingStream();

        void addSubscriber(const QString &addr, int port);

        void resume() override;
        void pause() override;

    protected:
    private:
        static void read(StreamPtr stream, size_t nbytes, void* kit);

        using AsyncKit = QPair<RecordingStream*, Subscribers*>;

        Subscribers m_subscribers;

        AsyncKit    m_kit;

    };
}
