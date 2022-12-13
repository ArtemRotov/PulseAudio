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
#LIBS += -lpulse-simple

HEADERS +=                              \
    $$PWD/Network/NetSocket.h           \
    $$PWD/Gui/mainwindow.h              \
    $$PWD/Audio/BufferAttributes.h      \
    $$PWD/Audio/MainLoopLocker.h        \
    $$PWD/Audio/PulseAudioHandler.h     \
    $$PWD/Audio/SampleSpecification.h   \
    $$PWD/Audio/IStream.h               \
    $$PWD/Settings/Settings.h \
    Audio/BasicStream.h \
    Audio/PlaybackStream.h \
    Audio/RecordingStream.h \
    def.h

SOURCES +=                              \
    $$PWD/Network/NetSocket.cpp         \
    $$PWD/Gui/mainwindow.cpp            \
    $$PWD/main.cpp                      \
    $$PWD/Audio/BufferAttributes.cpp    \
    $$PWD/Audio/MainLoopLocker.cpp      \
    $$PWD/Audio/PulseAudioHandler.cpp   \
    $$PWD/Audio/SampleSpecification.cpp \
    $$PWD/example.cpp                   \
    $$PWD/PA_DocumentationTranslate.cpp \
    $$PWD/Settings/Settings.cpp \
    Audio/BasicStream.cpp \
    Audio/PlaybackStream.cpp \
    Audio/RecordingStream.cpp


FORMS += \
    $$PWD/Gui/mainwindow.ui

DISTFILES +=

#QMAKE_CFLAGS += -fpermissive
#QMAKE_CXXFLAGS += -fpermissive
#