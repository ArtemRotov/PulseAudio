#include <pulse/stream.h>
#include <QCoreApplication>

#include "BasicStream.h"
#include "PulseAudioHandler.h"
#include "SampleSpecification.h"
#include "MainLoopLocker.h"
#include "Network/NetSocket.h"

using namespace pulse;


BasicStream::BasicStream(ContextPtr ctx, SampleSpecification* sample,
                BufferAttributes* buffAttr, ChannelMapPtr map, NetSocket* sock)
    : IStream()
    , m_sock(sock)
    , m_context(ctx)
    , m_sampleSpec(sample)
    , m_bufferAttr(buffAttr)
    , m_channelMap(map)
    , m_stream(nullptr)
{
    QString name = QCoreApplication::applicationName();

    MainLoopLocker lock(PulseAudioHandler::instance().mainLoop());

    m_stream = pa_stream_new(m_context, name.toLocal8Bit().data(), m_sampleSpec->get(), m_channelMap);
    pa_stream_set_state_callback(m_stream,
                                 streamStateCallBack,
                                 PulseAudioHandler::instance().mainLoop());
}

BasicStream::~BasicStream()
{
    MainLoopLocker lock(PulseAudioHandler::instance().mainLoop());

    pa_stream_disconnect(m_stream);
    pa_stream_unref(m_stream);
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

void BasicStream::streamStateCallBack(StreamPtr s, void *mainloop)
{
    Q_UNUSED(s)
    pa_threaded_mainloop_signal((pa_threaded_mainloop*)mainloop, 0);
}
