#include <QSettings>
#include <QApplication>
#include <QDebug>
#include <vector>
#include <queue>
#include <QThread>
#include <mutex>
#include <pulse/pulseaudio.h>
#include "Network/NetSocket.h"
#include "Audio/PulseAudioHandler.h"
#include "Audio/RecordingStream.h"
#include "Audio/PlaybackStream.h"
#include "mainwindow.h"

using namespace pulse;

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    QApplication::setOrganizationName( "Vniira" );
    QApplication::setApplicationName( "RadioSim" );

    PulseAudioHandler& pa =  PulseAudioHandler::instance();

    NetSocket sockIn("127.0.0.1", 11111);
    PlaybackStream* stream1 = pa.createPlaybackStream("out1", StreamMapType::LeftChannel,&sockIn);
    stream1->resume();

    NetSocket sockSend("127.0.0.1", 22222);
    RecordingStream* rstream1 = pa.createRecordingStream("in1", StreamMapType::StereoChannel, &sockSend);
    rstream1->addConsumer("127.0.0.1",11111);


    MainWindow m(rstream1/*, rstream2, rstream3*/);

    app.exec();
}

