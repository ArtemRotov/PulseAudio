#include <string>


class pa_threaded_mainloop;
class pa_mainloop_api;
class pa_context;


namespace pulse
{

    class PulseAudioService
    {
        typedef     pa_threaded_mainloop*   MainLoopPtr;
        typedef     pa_mainloop_api*        MainLoopApiPtr;
        typedef     pa_context*             ContextPtr;

        // Singleton notatin
        PulseAudioService();
        PulseAudioService(const PulseAudioService& );
        PulseAudioService& operator=(const PulseAudioService& );

        ~PulseAudioService();

        const std::string Name = "PA_Service";

    public:
        static PulseAudioService& instance();

    protected:
    private:
        void init();
        static void stateChanged(ContextPtr context, void* userData);

        static MainLoopPtr     m_mainLoop;
        MainLoopApiPtr  m_mainLoopApi;
        ContextPtr      m_context;

        void*           m_data;
    };

}




