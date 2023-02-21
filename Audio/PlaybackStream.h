#pragma once
#include <QQueue>
#include <QMutex>
#include <QPair>
#include "BasicStream.h"

class NetSocket;

namespace pulse
{
    class PlaybackStream : public BasicStream
    {
    public:
        explicit PlaybackStream(IHandler* h, const QString &n, ContextPtr ctx, SampleSpecification* sample,
                                BufferAttributes* buffAttr, ChannelMapPtr map, NetSocket* sock);
        ~PlaybackStream();

        int initialize() override;
        void resume() override;
        void pause() override;

        void writePolledAccess();
        static void writeAsyncAccess(StreamPtr stream, size_t requestedBytes, void* kit);
        void receiveData();

    protected:
    private:
        using DataKit = QPair<QQueue<uint8_t>*, QMutex*>;
        using AsyncKit = QPair<DataKit, IHandler*>;

        QMutex          m_mutex;
        QQueue<uint8_t> m_queueBuffer;

        AsyncKit        m_kit;
    };
}

