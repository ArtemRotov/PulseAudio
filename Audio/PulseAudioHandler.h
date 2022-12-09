#pragma once
#include <QCoreApplication>
#include <QDebug>
#include <string>
#include <pulse/thread-mainloop.h>

#include "def.h"


namespace pulse
{
    class PulseAudioHandler
    {
        // Singleton notation
        PulseAudioHandler();
        PulseAudioHandler(const PulseAudioHandler& );
        PulseAudioHandler& operator=(const PulseAudioHandler& );

        ~PulseAudioHandler();

        template <typename T>
        static void deviceInfo(ContextPtr context, const T* info, int eol, void *udata);
        static void stateChanged(ContextPtr context, void* userData);

    public:
        static PulseAudioHandler& instance();

        MainLoopPtr mainLoop() const;

    protected:
    private:
        void init();
        void doDeviceInfo() const;
        void initChannelMaps();

        static MainLoopPtr      m_mainLoop;

        MainLoopApiPtr          m_mainLoopApi;
        ContextPtr              m_context;

        ChannelMapPtr           m_channelMapStereo;
        ChannelMapPtr           m_channelMapLeft;
        ChannelMapPtr           m_channelMapRight;
        SampleSpecification*    m_sampleSpec;
        BufferAttributes*       m_bufferAttr;

    };


    template <typename T>
    void PulseAudioHandler::deviceInfo(ContextPtr context, const T* info, int eol, void *udata)
    {
        Q_UNUSED(context);
        Q_UNUSED(udata);

        if (eol != 0)
        {
            pa_threaded_mainloop_signal(m_mainLoop, 0);
            return;
        }

        qDebug() << "Device : " << info->name;
    }

}




