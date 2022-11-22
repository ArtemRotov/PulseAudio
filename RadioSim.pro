TEMPLATE = app
TARGET   = RadioSim

CONFIG += c++14

QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += multimedia

#DEFINES +=
#INCLUDEPATH += . ..

DEPENDPATH += .

MY_TMP_DIR = $$PWD/tmp/$${TARGET}/
MY_BIN_DIR = $$PWD/bin/
#MY_LIB_DIR =

OBJECTS_DIR = $${MY_TMP_DIR}$${MY_CONFIG}$${MY_TOOLSET}$${MY_PLATFORM}
MOC_DIR     = $${MY_TMP_DIR}moc/
RCC_DIR     = $${MY_TMP_DIR}rcc/
UI_DIR      = $${MY_TMP_DIR}uic/

#contains(CONFIG, staticlib) {
#  DESTDIR   = $${MY_LIB}
#} else {
DESTDIR   = $${MY_BIN_DIR}$${MY_CONFIG}$${MY_TOOLSET}$${MY_PLATFORM}
#}

INCLUDEPATH += $${UI_DIR}
INCLUDEPATH += $${MOC_DIR}


LIBS += -lpulse
LIBS += -lpulse-simple

HEADERS +=                              \
    $$PWD/Network/NetSocket.h           \
    $$PWD/Gui/mainwindow.h \
    Audio/MainLoopLocker.h \
    Audio/PulseAudioHandler.h \
    Settings/Settings.h

SOURCES +=                              \
    $$PWD/Network/NetSocket.cpp         \
    $$PWD/Gui/mainwindow.cpp            \
    $$PWD/Audio/examplePCMPlayback.cpp  \
    $$PWD/Audio/main2.cpp               \
    $$PWD/main.cpp \
    Audio/MainLoopLocker.cpp \
    Audio/PulseAudioHandler.cpp \
    Audio/example.cpp \
    PA_DocumentationTranslate.cpp \
    Settings/Settings.cpp


FORMS += \
    $$PWD/Gui/mainwindow.ui

DISTFILES +=

#QMAKE_CFLAGS += -fpermissive
#QMAKE_CXXFLAGS += -fpermissive


