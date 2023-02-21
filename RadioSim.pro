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

HEADERS +=                              \
    $$PWD/Network/NetSocket.h           \
    $$PWD/Audio/BufferAttributes.h      \
    $$PWD/Audio/MainLoopLocker.h        \
    $$PWD/Audio/PulseAudioHandler.h     \
    $$PWD/Audio/SampleSpecification.h   \
    $$PWD/Audio/IStream.h               \
    $$PWD/Settings/Settings.h           \
    $$PWD/Audio/BasicStream.h           \
    $$PWD/Audio/PlaybackStream.h        \
    $$PWD/Audio/RecordingStream.h       \
    $$PWD/def.h \
    Audio/IHandler.h \
    mainwindow.h

SOURCES +=                              \
    $$PWD/Network/NetSocket.cpp         \
    $$PWD/main.cpp                      \
    $$PWD/Audio/BufferAttributes.cpp    \
    $$PWD/Audio/MainLoopLocker.cpp      \
    $$PWD/Audio/PulseAudioHandler.cpp   \
    $$PWD/Audio/SampleSpecification.cpp \
    $$PWD/Settings/Settings.cpp         \
    $$PWD/Audio/BasicStream.cpp         \
    $$PWD/Audio/PlaybackStream.cpp      \
    $$PWD/Audio/RecordingStream.cpp \
    mainwindow.cpp

OTHER_FILES += $$PWD/example.cpp

#DISTFILES +=

#QMAKE_CFLAGS += -fpermissive
#QMAKE_CXXFLAGS += -fpermissive

FORMS += \
    mainwindow.ui
