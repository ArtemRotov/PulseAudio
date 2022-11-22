#include <pulse/pulseaudio.h>
#include "PulseAudioHandler.h"
#include "MainLoopLocker.h"

using namespace pulse;

PulseAudioHandler::MainLoopPtr PulseAudioHandler::m_mainLoop = nullptr;


PulseAudioHandler::PulseAudioHandler()
    : m_mainLoopApi(nullptr)
    , m_context(nullptr)
{
    init();
}

PulseAudioHandler::~PulseAudioHandler()
{
    MainLoopLocker lock(m_mainLoop);
    pa_context_unref(m_context);
    pa_context_disconnect(m_context);
    lock.unlock();

    pa_threaded_mainloop_stop(m_mainLoop);
    pa_threaded_mainloop_free(m_mainLoop);
}

void PulseAudioHandler::init()
{
    m_mainLoop = pa_threaded_mainloop_new();

    MainLoopLocker lock(m_mainLoop);
    pa_threaded_mainloop_start(m_mainLoop);

    m_mainLoopApi = pa_threaded_mainloop_get_api(m_mainLoop);

    m_context = pa_context_new_with_proplist(m_mainLoopApi, Name.c_str(), nullptr);

    pa_context_set_state_callback(m_context,
                                  PulseAudioHandler::stateChanged, nullptr);
    pa_context_connect(m_context, nullptr, PA_CONTEXT_NOAUTOSPAWN, nullptr);
    pa_threaded_mainloop_wait(m_mainLoop);

    doDeviceInfo();
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
    default:
        qDebug() << "DEFAULT OUTPUT PULSE AUDIO";
        break;
    }
}

void PulseAudioHandler::doDeviceInfo() const
{
    OperationPtr operationSink = pa_context_get_sink_info_list(m_context,
                                    PulseAudioHandler::deviceInfo, nullptr);
    while (true)
    {
        int result = pa_operation_get_state(operationSink);
        if (result == PA_OPERATION_DONE || result == PA_OPERATION_CANCELLED)
            break;
        pa_threaded_mainloop_wait(m_mainLoop);
    }
    pa_operation_unref(operationSink);

    OperationPtr operationSource = pa_context_get_source_info_list(m_context,
                                        PulseAudioHandler::deviceInfo, nullptr);
    while (true)
    {
        int result = pa_operation_get_state(operationSource);
        if (result == PA_OPERATION_DONE || result == PA_OPERATION_CANCELLED)
            break;
        pa_threaded_mainloop_wait(m_mainLoop);
    }
    pa_operation_unref(operationSource);
}
