#pragma once
#include <QStringList>
#include "BasicStream.h"

class NetSocket;

namespace pulse
{
    class RecordingStream : public BasicStream
    {
    public:
        explicit RecordingStream(IHandler* h, const QString &n, ContextPtr ctx, SampleSpecification* sample,
                                 BufferAttributes* buffAttr, ChannelMapPtr map, NetSocket* sock);
        ~RecordingStream();

        void addConsumer(const QString &addr, int port);

        int initialize() override;
        void resume() override;
        void pause() override;

    protected:
    private:
        static void read(StreamPtr stream, size_t nbytes, void* kit);

        using AsyncKit = QPair<RecordingStream*, Consumers*>;

        Consumers m_consumers;

        AsyncKit    m_kit;
    };
}
