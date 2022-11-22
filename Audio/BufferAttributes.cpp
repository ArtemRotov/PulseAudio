#include "BufferAttributes.h"
#include "Settings/Settings.h"


using namespace pulse;


BufferAttributes::BufferAttributes()
    : m_maxLength(Settings::value(Settings::bufferMaxLength).toInt())
    , m_tLength(Settings::value(Settings::bufferTLength).toInt())
    , m_prebuf(Settings::value(Settings::bufferPrebuf).toInt())
    , m_minReq(Settings::value(Settings::bufferMinReq).toInt())
    , m_fragSize(Settings::value(Settings::bufferFragSize).toInt())
{

}


pa_buffer_attr BufferAttributes::get() const
{
    pa_buffer_attr result;
    result.maxlength = m_maxLength;
    result.tlength = m_tLength;
    result.prebuf = m_prebuf;
    result.minreq = m_minReq;
    result.fragsize = m_fragSize;

    return result;
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
