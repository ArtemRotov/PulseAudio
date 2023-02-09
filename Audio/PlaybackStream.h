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
        explicit PlaybackStream(ContextPtr ctx, SampleSpecification* sample,
                                 BufferAttributes* buffAttr, ChannelMapPtr map, NetSocket* sock);
        ~PlaybackStream();

        void resume() override;
        void pause() override;

        void writePolledAccess();
        static void writeAsyncAccess(StreamPtr stream, size_t requestedBytes, void* kit);
        void receiveData();

    protected:
    private:
        using AsyncKit = QPair<QQueue<uint8_t>*, QMutex*>;

        QMutex          m_mutex;
        QQueue<uint8_t> m_queueBuffer;

        AsyncKit        m_kit;
    };


}

