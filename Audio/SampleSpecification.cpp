#include "SampleSpecification.h"
#include "Settings/Settings.h"

using namespace pulse;


SampleSpecification::SampleSpecification()
    : m_format(Settings::value(Settings::sampleFormat).toInt())
    , m_rate(Settings::value(Settings::sampleRate).toInt())
    , m_channels(Settings::value(Settings::sampleChannels).toInt())
{

}

pa_sample_spec SampleSpecification::get() const
{
    pa_sample_spec result;
    result.format = (pa_sample_format_t)m_format;
    result.rate = m_rate;
    result.channels = m_channels;

    return result;
}

uint8_t SampleSpecification::format() const
{
    return m_format;
}

uint32_t SampleSpecification::rate() const
{
    return m_rate;
}

uint8_t SampleSpecification::channels() const
{
    return m_channels;
}
