#include <pulse/pulseaudio.h>
#include <QCoreApplication>
#include <QDebug>
#include "PulseAudioService.h"



using namespace pulse;

PulseAudioService::MainLoopPtr PulseAudioService::m_mainLoop = nullptr;


PulseAudioService::PulseAudioService()
//    : m_mainLoop(pa_threaded_mainloop_new())
    : m_mainLoopApi(nullptr)
    , m_context(nullptr)
    , m_data(nullptr)
{
    init();
}

PulseAudioService::~PulseAudioService()
{
    pa_context_unref(m_context);
    pa_context_disconnect(m_context);

    pa_threaded_mainloop_stop(m_mainLoop);
    pa_threaded_mainloop_free(m_mainLoop);
}

void PulseAudioService::init()
{
    pa_threaded_mainloop_lock(m_mainLoop);
    pa_threaded_mainloop_start(m_mainLoop);

    m_mainLoopApi = pa_threaded_mainloop_get_api(m_mainLoop);

    m_context = pa_context_new_with_proplist(m_mainLoopApi, Name.c_str(), nullptr);

    pa_context_set_state_callback(m_context,
                                  PulseAudioService::stateChanged,
                                  m_data);
    pa_context_connect(m_context, nullptr, PA_CONTEXT_NOAUTOSPAWN, nullptr);
    pa_threaded_mainloop_wait(m_mainLoop);
}

PulseAudioService& PulseAudioService::instance()
{
    static PulseAudioService theSingleInstance;
    return theSingleInstance;
}

void PulseAudioService::stateChanged(ContextPtr context, void* userData)
{
    Q_UNUSED(userData);

    switch(pa_context_get_state(context))
    {
    case PA_CONTEXT_READY:
        qDebug() << "PULSE AUDIO CONNECT. PA_CONTEXT_READY";
        pa_threaded_mainloop_signal(m_mainLoop, 0);
        break;

    case PA_CONTEXT_FAILED:
        qDebug() << "PA_CONTEXT_FAILED";
        break;

    case PA_CONTEXT_TERMINATED:
        qDebug() << "PULSE AUDIO DISCONNECT. PA_CONTEXT_TERMINATED";
        pa_threaded_mainloop_signal(m_mainLoop, 0);
        break;
    }
}
