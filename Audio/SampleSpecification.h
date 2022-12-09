#include "def.h"

namespace pulse
{
    class SampleSpecification
    {
    public:
        SampleSpecification();
        ~SampleSpecification() = default;

        SampleSpecPtr get() const;

        uint8_t format()    const;
        uint32_t rate()     const;
        uint8_t channels()  const;

    protected:
    private:
        const uint8_t   m_format;
        const uint32_t  m_rate;
        const uint8_t   m_channels;

        SampleSpecPtr   m_sampleSpec;
    };

}



