namespace pulse
{

    // Interface class, for a possible connection to a Terminal

    class IStream
    {
    public:
        explicit IStream() = default;
        virtual ~IStream() = default;

        virtual void resume();
        virtual void pause();

    protected:
    private:
    };

}



