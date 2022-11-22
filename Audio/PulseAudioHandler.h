#include <QCoreApplication>
#include <QDebug>
#include <string>


class pa_threaded_mainloop;
class pa_mainloop_api;
class pa_context;
class pa_operation;



namespace pulse
{
    class SampleSpecification;
    class BufferAttributes;


    class PulseAudioHandler
    {
        typedef     pa_threaded_mainloop*   MainLoopPtr;
        typedef     pa_mainloop_api*        MainLoopApiPtr;
        typedef     pa_context*             ContextPtr;
        typedef     pa_operation*           OperationPtr;

        // Singleton notatin
        PulseAudioHandler();
        PulseAudioHandler(const PulseAudioHandler& );
        PulseAudioHandler& operator=(const PulseAudioHandler& );

        ~PulseAudioHandler();

        template <typename T>
        static void deviceInfo(ContextPtr context, const T* info, int eol, void *udata);
        static void stateChanged(ContextPtr context, void* userData);

    public:
        static PulseAudioHandler& instance();

    protected:
    private:
        void init();
        void doDeviceInfo() const;

        static MainLoopPtr  m_mainLoop;
        MainLoopApiPtr      m_mainLoopApi;
        ContextPtr          m_context;

        SampleSpecification*    m_sampleSpec;
        BufferAttributes*       m_bufferAttr;

    };


    template <typename T>
    void PulseAudioHandler::deviceInfo(ContextPtr context, const T* info, int eol, void *udata)
    {
        Q_UNUSED(context);
        Q_UNUSED(udata);

        if (eol != 0)
        {
            pa_threaded_mainloop_signal(m_mainLoop, 0);
            return;
        }

        qDebug() << "Device : " << info->name;
    }

}




