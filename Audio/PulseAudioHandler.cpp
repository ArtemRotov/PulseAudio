#include <pulse/pulseaudio.h>
#include <QCoreApplication>
#include <QDebug>
#include "PulseAudioHandler.h"



using namespace pulse;

PulseAudioHandler::MainLoopPtr PulseAudioHandler::m_mainLoop = nullptr;


PulseAudioHandler::PulseAudioHandler()
    : m_mainLoopApi(nullptr)
    , m_context(nullptr)
    , m_data(nullptr)
{
    init();
}

PulseAudioHandler::~PulseAudioHandler()
{
    pa_context_unref(m_context);
    pa_context_disconnect(m_context);

    pa_threaded_mainloop_stop(m_mainLoop);
    pa_threaded_mainloop_free(m_mainLoop);
}

void PulseAudioHandler::init()
{
    m_mainLoop = pa_threaded_mainloop_new();

    pa_threaded_mainloop_lock(m_mainLoop);
    pa_threaded_mainloop_start(m_mainLoop);

    m_mainLoopApi = pa_threaded_mainloop_get_api(m_mainLoop);

    m_context = pa_context_new_with_proplist(m_mainLoopApi, Name.c_str(), nullptr);

    pa_context_set_state_callback(m_context,
                                  PulseAudioHandler::stateChanged,
                                  m_data);
    pa_context_connect(m_context, nullptr, PA_CONTEXT_NOAUTOSPAWN, nullptr);
    pa_threaded_mainloop_wait(m_mainLoop);
}

PulseAudioHandler& PulseAudioHandler::instance()
{
    static PulseAudioHandler theSingleInstance;
    return theSingleInstance;
}

void PulseAudioHandler::stateChanged(ContextPtr context, void* userData)
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
