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
    pulse::PlaybackStream* stream1 = pulse::PulseAudioHandler::instance().createPlaybackStream("leftCh", pulse::StreamMapType::LeftChannel,
                                                              &sockIn);
    stream1->resume();

    NetSocket sockIn2("192.9.206.60", 11112);
    pulse::PlaybackStream* stream2 = pulse::PulseAudioHandler::instance().createPlaybackStream("rightCh", pulse::StreamMapType::RightChannel,
                                                              &sockIn2);
    stream2->resume();

    NetSocket sockIn3("192.9.206.60", 11113);
    pulse::PlaybackStream* stream3 = pulse::PulseAudioHandler::instance().createPlaybackStream("StereoCh", pulse::StreamMapType::StereoChannel,
                                                              &sockIn3);
    stream3->resume();



    // На отправку
    NetSocket sockSend("192.9.206.60", 22222);
    pulse::RecordingStream* rstream1 = pulse::PulseAudioHandler::instance().createRecordingStream("leftChRec", pulse::StreamMapType::StereoChannel, &sockSend);
    rstream1->pause();
    rstream1->addConsumer("192.9.206.60",11111);

    pulse::RecordingStream* rstream2 = pulse::PulseAudioHandler::instance().createRecordingStream("rightChRec", pulse::StreamMapType::StereoChannel, &sockSend);
    rstream2->pause();
    rstream2->addConsumer("192.9.206.60",11112);

    pulse::RecordingStream* rstream3 = pulse::PulseAudioHandler::instance().createRecordingStream("stereoChRec", pulse::StreamMapType::StereoChannel, &sockSend);
    rstream3->pause();
    rstream3->addConsumer("192.9.206.60",11113);



    MainWindow m(rstream1, rstream2, rstream3, nullptr);
    app.exec();
}

