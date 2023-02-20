#pragma once

namespace pulse
{
    // Interface class, for a possible connection to a Terminal

    class IStream
    {
    public:
        explicit IStream() = default;
        virtual ~IStream() = default;

        virtual int initialize() = 0;
        virtual void resume() = 0;
        virtual void pause() = 0;

    protected:
    private:
    };

}



