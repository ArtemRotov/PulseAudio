#include <pulse/stream.h>
#include <QCoreApplication>

#include "BasicStream.h"
#include "PulseAudioHandler.h"
#include "SampleSpecification.h"
#include "MainLoopLocker.h"
#include "Network/NetSocket.h"

using namespace pulse;


BasicStream::BasicStream(const QString &n, ContextPtr ctx, SampleSpecification* sample,
                         BufferAttributes* buffAttr, ChannelMapPtr map, NetSocket* sock)
    : IStream()
    , m_name(n)
    , m_sock(sock)
    , m_context(ctx)
    , m_sampleSpec(sample)
    , m_bufferAttr(buffAttr)
    , m_channelMap(map)
    , m_stream(nullptr)
{

}

int BasicStream::initialize()
{
    QString name = QCoreApplication::applicationName();

    MainLoopLocker lock(PulseAudioHandler::instance().mainLoop());

    m_stream = pa_stream_new(m_context, name.toLocal8Bit().data(), m_sampleSpec->get(), m_channelMap);
    pa_stream_set_state_callback(m_stream,
                                 streamStateCallBack,
                                 &PulseAudioHandler::instance());
    return 0;
}

BasicStream::~BasicStream()
{
    MainLoopLocker lock(PulseAudioHandler::instance().mainLoop());

    pa_stream_disconnect(m_stream);
    pa_stream_unref(m_stream);
}

QString BasicStream::name() const
{
    return m_name;
}

StreamPtr BasicStream::stream() const
{
    return m_stream;
}

BufferAttributes* BasicStream::bufferAttributes() const
{
    return m_bufferAttr;
}


NetSocket* BasicStream::socket() const
{
    return m_sock;
}

void BasicStream::setSocket(NetSocket* sock)
{
    m_sock = sock;
}

void BasicStream::streamStateCallBack(StreamPtr s, void *userData)
{
    PulseAudioHandler* handler =  static_cast<PulseAudioHandler*>(userData);
    QString name = handler->nameByStream(s);

    switch(pa_stream_get_state(s))
    {
    case PA_STREAM_UNCONNECTED:
        qDebug() << name << ": PA_STREAM_UNCONNECTED";
        break;

    case PA_STREAM_CREATING:
        qDebug() << name << ": PA_STREAM_CREATING";
        break;

    case PA_STREAM_READY:
        qDebug() << name << ": PA_STREAM_READY";
        pa_threaded_mainloop_signal(static_cast<pa_threaded_mainloop*>(handler->mainLoop()), 0);
        break;

    case PA_STREAM_FAILED:
        qDebug() << name << ": PA_STREAM_FAILED";
        std::runtime_error("PA_STREAM_FAILED");
        break;

    case PA_STREAM_TERMINATED:
        qDebug() << name << ": PA_STREAM_TERMINATED";
        std::runtime_error("PA_STREAM_TERMINATED");
        break;
    }
}
