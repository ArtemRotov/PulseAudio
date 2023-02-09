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

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    QApplication::setOrganizationName( "Vniira" );
    QApplication::setApplicationName( "RadioSim" );

    pulse::initialize();

    // На прием
    NetSocket sockIn("192.9.206.60", 11111);
    pulse::PlaybackStream* stream1 = pulse::PulseAudioHandler::instance().createPlaybackStream(pulse::StreamMapType::LeftChannel,
                                                              &sockIn);
    stream1->resume();

    // На отправку
    NetSocket sockSend("192.9.206.60", 11112);
    pulse::RecordingStream* stream = pulse::PulseAudioHandler::instance().createRecordingStream(pulse::StreamMapType::StereoChannel,
                                                                                                &sockSend);

    stream->addSubscriber("192.9.206.60",11111);
    stream->pause();


    MainWindow m(stream,nullptr);
    app.exec();
}

