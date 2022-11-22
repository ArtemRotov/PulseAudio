#include <pulse/def.h>


namespace pulse
{

    class BufferAttributes
    {
    public:
        BufferAttributes();
        ~BufferAttributes() = default;

        pa_buffer_attr get()    const;

        uint32_t maxLength()    const;
        uint32_t tLength()      const;
        uint32_t prebuf()       const;
        uint32_t minReq()       const;
        uint32_t fragSize()     const;

    protected:
    private:
        const uint32_t  m_maxLength;
        const uint32_t  m_tLength;
        const uint32_t  m_prebuf;
        const uint32_t  m_minReq;
        const uint32_t  m_fragSize;
    };

}



