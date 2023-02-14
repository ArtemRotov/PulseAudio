#pragma once
#include <QCoreApplication>
#include <QDebug>
#include <QMutex>
#include <QWaitCondition>
#include <string>

#include <pulse/thread-mainloop.h>
#include <mutex>

#include "def.h"

class NetSocket;

namespace pulse
{
    void initialize();

    class BasicStream;
    class RecordingStream;
    class PlaybackStream;

    class PulseAudioHandler : public QObject
    {
        Q_OBJECT
        friend void pulse::initialize();

        // Singleton notation
        PulseAudioHandler();
        PulseAudioHandler(const PulseAudioHandler& );
        PulseAudioHandler& operator=(const PulseAudioHandler& );

        ~PulseAudioHandler();

        template <typename T>
        static void deviceInfo(ContextPtr context, const T* info, int eol, void *userData);
        static void stateChanged(ContextPtr context, void* userData);

    public:
        static PulseAudioHandler& instance();

        MainLoopPtr mainLoop() const;

        RecordingStream* createRecordingStream(const QString &name, StreamMapType type, NetSocket* socket);
        PlaybackStream* createPlaybackStream(const QString &name, StreamMapType type, NetSocket* socket);

        void start();


    private:
        void init();
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
        // Q_UNUSED(udata);
        PulseAudioHandler* pthis =  static_cast<PulseAudioHandler*>(userData);

        if (eol != 0)
        {
            pa_threaded_mainloop_signal(pthis->mainLoop(), 0);
            return;
        }

        qDebug() << "Device : " << info->name;
    }

}




