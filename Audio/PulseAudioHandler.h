#pragma once
#include <QDebug>
#include <pulse/thread-mainloop.h>

#include "def.h"
#include "IHandler.h"


class NetSocket;

namespace pulse
{
    IHandler* newHandler();

    class BasicStream;
    class RecordingStream;
    class PlaybackStream;

    class PulseAudioHandler : public IHandler
    {
        template <typename T>
        static void deviceInfo(ContextPtr context, const T* info, int eol, void *userData);
        static void stateChanged(ContextPtr context, void* userData);

    public:
        PulseAudioHandler();
        ~PulseAudioHandler();

        void initialize() override;
        MainLoopPtr mainLoop() const override;

        QString         nameByStream        (StreamPtr s) const override;
        BasicStream*    basicStreamByStream (StreamPtr s) const override;

        void connectConsumer(IStream* source, IStream* consumer) override;
        IStream* newStream(const QString &name, StreamType type, StreamMapType mtype, NetSocket* socket) override;

    private:
        void doDeviceInfo();
        void initChannelMaps();

    private:
        MainLoopPtr             m_mainLoop;

        MainLoopApiPtr          m_mainLoopApi;
        ContextPtr              m_context;

        ChannelMapPtr           m_channelMapStereo;
        ChannelMapPtr           m_channelMapLeft;
        ChannelMapPtr           m_channelMapRight;
        SampleSpecification*    m_sampleSpec;
        BufferAttributes*       m_bufferAttr;

        QVector<BasicStream*>   m_streams;
    };


    template <typename T>
    void PulseAudioHandler::deviceInfo(ContextPtr context, const T* info, int eol, void *userData)
    {
        Q_UNUSED(context);

        PulseAudioHandler* handler =  static_cast<PulseAudioHandler*>(userData);

        if (eol != 0)
        {
            pa_threaded_mainloop_signal(handler->mainLoop(), 0);
            return;
        }

        qDebug() << "Device : " << info->name;
    }
}




