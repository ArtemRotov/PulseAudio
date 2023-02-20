#include "BufferAttributes.h"
#include "Settings/Settings.h"


using namespace pulse;


BufferAttributes::BufferAttributes()
    : m_maxLength(Settings::value(Settings::bufferMaxLength).toInt())
    , m_tLength  (Settings::value(Settings::bufferTLength).toInt())
    , m_prebuf   (Settings::value(Settings::bufferPrebuf).toInt())
    , m_minReq   (Settings::value(Settings::bufferMinReq).toInt())
    , m_fragSize (Settings::value(Settings::bufferFragSize).toInt())
    , m_buffer(new pa_buffer_attr)
{
    m_buffer->maxlength = m_maxLength;
    m_buffer->tlength = m_tLength;
    m_buffer->prebuf = m_prebuf;
    m_buffer->minreq = m_minReq;
    m_buffer->fragsize = m_fragSize;
}

BufferAttributes::~BufferAttributes()
{
    delete m_buffer;
}

BufferAttrPtr BufferAttributes::get() const
{
    return m_buffer;
}

uint32_t BufferAttributes::maxLength() const
{
    return m_maxLength;
}

uint32_t BufferAttributes::tLength() const
{
    return m_tLength;
}

uint32_t BufferAttributes::prebuf() const
{
    return m_prebuf;
}

uint32_t BufferAttributes::minReq() const
{
    return m_minReq;
}

uint32_t BufferAttributes::fragSize() const
{
    return m_fragSize;
}
