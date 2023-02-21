#include "def.h"

class NetSocket;

namespace pulse
{
    class IStream;
    class BasicStream;
    class RecordingStream;
    class PlaybackStream;

    class IHandler
    {
    public:
        explicit IHandler() {};
        virtual ~IHandler() {};

        virtual void                initialize() = 0;

        virtual MainLoopPtr         mainLoop() const = 0;

        virtual QString             nameByStream        (StreamPtr s) const = 0;
        virtual BasicStream*        basicStreamByStream (StreamPtr s) const = 0;

        virtual void                connectConsumer(IStream* source, IStream* consumer) = 0;
        virtual IStream*            newStream(const QString &name, StreamType type, StreamMapType mtype, NetSocket* socket) = 0;

    protected:
    private:
    };
}


