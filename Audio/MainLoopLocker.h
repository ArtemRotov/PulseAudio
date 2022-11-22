class pa_threaded_mainloop;


namespace pulse
{
    class MainLoopLocker
    {
    public:
        explicit MainLoopLocker(pa_threaded_mainloop* ptr);
        ~MainLoopLocker();

        void lock();
        void unlock();

    private:
        bool                    m_isLocked;
        pa_threaded_mainloop*   m_mainLoop;
    };
}



