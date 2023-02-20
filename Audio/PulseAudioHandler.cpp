#include <pulse/pulseaudio.h>
#include <QObject>
#include <QThread>
#include "PulseAudioHandler.h"
#include "MainLoopLocker.h"
#include "Settings/Settings.h"
#include "SampleSpecification.h"
#include "BufferAttributes.h"
#include "Audio/RecordingStream.h"
#include "Audio/PlaybackStream.h"

using namespace pulse;

PulseAudioHandler::PulseAudioHandler()
    : QObject(nullptr)
    , m_mainLoop(nullptr)
    , m_mainLoopApi(nullptr)
    , m_context(nullptr)
    , m_channelMapStereo(new ChannelMap)
    , m_channelMapLeft(new ChannelMap)
    , m_channelMapRight(new ChannelMap)
    , m_sampleSpec(new SampleSpecification)
    , m_bufferAttr(new BufferAttributes)
    , m_streams()
{
}

PulseAudioHandler::~PulseAudioHandler()
{
    MainLoopLocker lock(m_mainLoop);
    pa_context_unref(m_context);
    pa_context_disconnect(m_context);

    for (BasicStream* el : m_streams) delete el;
    lock.unlock();

    pa_threaded_mainloop_stop(m_mainLoop);
    pa_threaded_mainloop_free(m_mainLoop);

    delete m_channelMapStereo;
    delete m_channelMapLeft;
    delete m_channelMapRight;
    delete m_sampleSpec;
    delete m_bufferAttr;
}

void pulse::initialize()
{
    PulseAudioHandler::instance().init();
}

void PulseAudioHandler::init()
{
    pulse::Settings::instance();

    m_mainLoop = pa_threaded_mainloop_new();

    MainLoopLocker lock(m_mainLoop);
    pa_threaded_mainloop_start(m_mainLoop);

    m_mainLoopApi = pa_threaded_mainloop_get_api(m_mainLoop);

    m_context = pa_context_new_with_proplist(m_mainLoopApi,
    Settings::value(Settings::pulseApplicationName).toString().toStdString().c_str(),
                                             nullptr);

    pa_context_set_state_callback(m_context,
                                  PulseAudioHandler::stateChanged, nullptr);
    pa_context_connect(m_context, nullptr, PA_CONTEXT_NOAUTOSPAWN, nullptr);
    pa_threaded_mainloop_wait(m_mainLoop);

    initChannelMaps();
    doDeviceInfo();
}

MainLoopPtr PulseAudioHandler::mainLoop() const
{
    return m_mainLoop;
}

RecordingStream* PulseAudioHandler::createRecordingStream(const QString &name, StreamMapType type, NetSocket* socket)
{
    RecordingStream* stream = nullptr;
    ChannelMapPtr m = nullptr;

    switch (type)
    {
    case StreamMapType::StereoChannel:
        m = m_channelMapStereo;
        break;

    case StreamMapType::LeftChannel:
        m = m_channelMapLeft;
        break;

    case StreamMapType::RightChannel:
        m = m_channelMapRight;
        break;

    default:
        std::runtime_error("Unrecognized channel map");
        break;
    } 

    stream = new RecordingStream(name, m_context, m_sampleSpec, m_bufferAttr, m, socket);

    if (stream)
        m_streams.push_back(stream);

    return stream;
}

PlaybackStream* PulseAudioHandler::createPlaybackStream(const QString &name, StreamMapType type, NetSocket* socket)
{
    PlaybackStream* stream = nullptr;
    ChannelMapPtr m = nullptr;

    switch (type)
    {
    case StreamMapType::StereoChannel:
        m = m_channelMapStereo;
        break;

    case StreamMapType::LeftChannel:
        m = m_channelMapLeft;
        break;

    case StreamMapType::RightChannel:
        m = m_channelMapRight;
        break;

    default:
        std::runtime_error("Unrecognized channel map");
        break;
    }

    stream = new PlaybackStream(name, m_context, m_sampleSpec, m_bufferAttr, m, socket);

    if (stream)
        m_streams.push_back(stream);

    return stream;
}

PulseAudioHandler& PulseAudioHandler::instance()
{
    static PulseAudioHandler theSingleInstance;
    //std::call_once(flag1,[&](){theSingleInstance.init();});
    return theSingleInstance;
}

void PulseAudioHandler::stateChanged(ContextPtr context, void* userData)
{
    Q_UNUSED(userData);

    switch(pa_context_get_state(context))
    {
    case PA_CONTEXT_UNCONNECTED:
        qDebug() << "PA_CONTEXT_UNCONNECTED";
        break;

    case PA_CONTEXT_CONNECTING:
        qDebug() << "PA_CONTEXT_CONNECTING";
        break;

    case PA_CONTEXT_AUTHORIZING:
        qDebug() << "PA_CONTEXT_AUTHORIZING";
        break;

    case PA_CONTEXT_SETTING_NAME:
        qDebug() << "PA_CONTEXT_SETTING_NAME";
        break;

    case PA_CONTEXT_READY:
        qDebug() << "PA_CONTEXT_READY";
        pa_threaded_mainloop_signal(instance().mainLoop(), 0);
        break;

    case PA_CONTEXT_FAILED:
        qDebug() << "PA_CONTEXT_FAILED";
        break;

    case PA_CONTEXT_TERMINATED:
        qDebug() << "PA_CONTEXT_TERMINATED";
        pa_threaded_mainloop_signal(instance().mainLoop(), 0);
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

void PulseAudioHandler::initChannelMaps()
{
    pa_channel_map_init_stereo(m_channelMapStereo);
    m_channelMapStereo->map[0] = PA_CHANNEL_POSITION_LEFT;
    m_channelMapStereo->map[1] = PA_CHANNEL_POSITION_RIGHT;

    pa_channel_map_init_stereo(m_channelMapLeft);
    m_channelMapLeft->map[0] = PA_CHANNEL_POSITION_LEFT;
    m_channelMapLeft->map[1] = PA_CHANNEL_POSITION_LEFT;

    pa_channel_map_init_stereo(m_channelMapRight);
    m_channelMapRight->map[0] = PA_CHANNEL_POSITION_RIGHT;
    m_channelMapRight->map[1] = PA_CHANNEL_POSITION_RIGHT;
}
