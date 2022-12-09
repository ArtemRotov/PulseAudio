#include <pulse/sample.h>

#include "SampleSpecification.h"
#include "Settings/Settings.h"

using namespace pulse;


SampleSpecification::SampleSpecification()
    : m_format(Settings::value(Settings::sampleFormat).toInt())
    , m_rate(Settings::value(Settings::sampleRate).toInt())
    , m_channels(Settings::value(Settings::sampleChannels).toInt())
    , m_sampleSpec(new pa_sample_spec)
{
    m_sampleSpec->format = (pa_sample_format_t)m_format;
    m_sampleSpec->rate = m_rate;
    m_sampleSpec->channels = m_channels;

}

SampleSpecPtr SampleSpecification::get() const
{
    return m_sampleSpec;
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
