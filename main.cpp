#include <QApplication>
#include <QDebug>

#include "Audio/PulseAudioHandler.h"
#include "Network/NetSocket.h"

#include "mainwindow.h"

using namespace pulse;

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    QApplication::setOrganizationName( "Vniira" );
    QApplication::setApplicationName( "RadioSim" );

    IHandler* handler = newHandler();

    NetSocket sockIn("127.0.0.1", 11111);
    IStream* s1 = handler->newStream("out1",StreamType::Playback, StreamMapType::LeftChannel,&sockIn);
    s1->resume();

    NetSocket sockSend("127.0.0.1", 22222);
    IStream* s2 = handler->newStream("out1",StreamType::Recording, StreamMapType::LeftChannel,&sockSend);

    handler->connectConsumer(s2,s1);

    MainWindow m(s2);

    app.exec();
}

