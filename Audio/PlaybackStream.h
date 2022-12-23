#pragma once
#include <QQueue>
#include "BasicStream.h"
#include <QMutex>
#include <QPair>

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
        static void writeAsyncAccess(StreamPtr stream, size_t requestedBytes, void* buffer);
        void receiveData();

    protected:
    private:
        using AsyncKit = QPair<QQueue<uint8_t>*, QMutex*>;

        AsyncKit        m_params;

        QMutex          m_mutex;
        QQueue<uint8_t> m_queueBuffer;

    };


}

