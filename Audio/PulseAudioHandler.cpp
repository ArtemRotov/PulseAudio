#include <pulse/pulseaudio.h>

#include "PulseAudioHandler.h"
#include "Network/NetSocket.h"
#include "MainLoopLocker.h"
#include "Settings/Settings.h"
#include "SampleSpecification.h"
#include "BufferAttributes.h"
#include "Audio/RecordingStream.h"
#include "Audio/PlaybackStream.h"

using namespace pulse;

IHandler* pulse::newHandler()
{
    PulseAudioHandler* handler = nullptr;
    try
    {
        handler = new PulseAudioHandler;
        handler->initialize();
    }
    catch(...)
    {
        qDebug() << "Handler error";
        return nullptr;
    }

    return handler;
}

PulseAudioHandler::PulseAudioHandler()
    : IHandler()
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
    if (m_context)
    {
        pa_context_unref(m_context);
        pa_context_disconnect(m_context);
    }

    for (BasicStream* el : m_streams) delete el;
    lock.unlock();

    if (m_mainLoop)
    {
        pa_threaded_mainloop_stop(m_mainLoop);
        pa_threaded_mainloop_free(m_mainLoop);
    }

    delete m_channelMapStereo;
    delete m_channelMapLeft;
    delete m_channelMapRight;
    delete m_sampleSpec;
    delete m_bufferAttr;
}

void PulseAudioHandler::initialize()
{
    pulse::Settings::instance();

    m_mainLoop = pa_threaded_mainloop_new();
    if (!m_mainLoop)
        std::runtime_error("Сouldn't initialize mainloop in PulseAudioHandler");

    qDebug() << "pa_threaded_mainloop_start" << pa_threaded_mainloop_start(m_mainLoop);

    m_mainLoopApi = pa_threaded_mainloop_get_api(m_mainLoop);
    if (!m_mainLoopApi)
        std::runtime_error("Сouldn't initialize API in PulseAudioHandler");

    m_context = pa_context_new_with_proplist(m_mainLoopApi,
    Settings::value(Settings::pulseApplicationName).toString().toStdString().c_str(),
                                             nullptr);
    if (!m_context)
        std::runtime_error("Сouldn't initialize context in PulseAudioHandler");

    MainLoopLocker lock(m_mainLoop);
    pa_context_set_state_callback(m_context, PulseAudioHandler::stateChanged, this);

    pa_context_connect(m_context, nullptr, PA_CONTEXT_NOAUTOSPAWN, nullptr);

    pa_threaded_mainloop_wait(m_mainLoop);

    lock.unlock();
    initChannelMaps();
    lock.lock();

    doDeviceInfo();
}

MainLoopPtr PulseAudioHandler::mainLoop() const
{
    return m_mainLoop;
}

IStream* PulseAudioHandler::newStream(const QString &name, StreamType type, StreamMapType mtype, NetSocket* socket)
{
    IStream* stream = nullptr;
    ChannelMapPtr cm = nullptr;

    switch (mtype)
    {
    case StreamMapType::StereoChannel:
        cm = m_channelMapStereo;
        break;

    case StreamMapType::LeftChannel:
        cm = m_channelMapLeft;
        break;

    case StreamMapType::RightChannel:
        cm = m_channelMapRight;
        break;

    default:
        std::runtime_error("Unrecognized channel map");
        break;
    }

    switch (type)
    {
    case StreamType::Playback:
        stream = new PlaybackStream(this, name, m_context, m_sampleSpec, m_bufferAttr, cm, socket);
        break;

    case StreamType::Recording:
        stream = new RecordingStream(this, name, m_context, m_sampleSpec, m_bufferAttr, cm, socket);
        break;

    default:
        std::runtime_error("Unrecognized stream type");
        break;
    }

    m_streams.push_back(dynamic_cast<BasicStream*>(stream));

    if (!m_streams.back()->initialize())
        return stream;
    else
        m_streams.pop_back();

    return nullptr;
}

void PulseAudioHandler::connectConsumer(IStream* source, IStream* consumer)
{
    RecordingStream* rec = dynamic_cast<RecordingStream*>(source);
    if (!rec)
    {
        qDebug() << "The source stream is not defined";
        return;
    }

    PlaybackStream* plbck = dynamic_cast<PlaybackStream*>(consumer);
    if (!plbck)
    {
        qDebug() << "The consumer stream is not defined";
        return;
    }

    rec->addConsumer(plbck->socket()->address(), plbck->socket()->port());
}

QString PulseAudioHandler::nameByStream(StreamPtr s) const
{
    for (BasicStream* stream : m_streams)
    {
        if (stream->stream() == s)
            return stream->name();
    }

    return QString();
}

BasicStream* PulseAudioHandler::basicStreamByStream(StreamPtr s) const
{
    for (BasicStream* stream : m_streams)
    {
        if (stream->stream() == s)
            return stream;
    }

    return nullptr;
}

void PulseAudioHandler::stateChanged(ContextPtr context, void* userData)
{
    PulseAudioHandler* handler =  static_cast<PulseAudioHandler*>(userData);

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
        pa_threaded_mainloop_signal(handler->mainLoop(), 0);
        break;

    case PA_CONTEXT_FAILED:
        qDebug() << "PA_CONTEXT_FAILED";
        break;

    case PA_CONTEXT_TERMINATED:
        qDebug() << "PA_CONTEXT_TERMINATED";
        pa_threaded_mainloop_signal(handler->mainLoop(), 0);
        break;
    default:
        qDebug() << "DEFAULT OUTPUT PULSE AUDIO";
        break;
    }
}

void PulseAudioHandler::doDeviceInfo()
{
    OperationPtr operationSink = pa_context_get_sink_info_list(m_context,
                                    &PulseAudioHandler::deviceInfo, this);

    if(operationSink == 0)
        return;

    while (true)
    {
        int result = pa_operation_get_state(operationSink);
        if (result == PA_OPERATION_DONE || result == PA_OPERATION_CANCELLED)
            break;
        pa_threaded_mainloop_wait(m_mainLoop);
    }
    pa_operation_unref(operationSink);

    OperationPtr operationSource = pa_context_get_source_info_list(m_context,
                                        PulseAudioHandler::deviceInfo, this);

    if(operationSource == 0)
        return;

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
