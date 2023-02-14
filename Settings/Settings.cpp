#include "Settings.h"


using namespace pulse;


const QString Settings::pulseApplicationName = "pulseApplicationName";
const QString Settings::usePlaybackAsyncAccessModel = "usePlaybackAsyncAccessModel";
const QString Settings::useRecordAsyncAccessModel = "useRecordAsyncAccessModel";

const QString Settings::sampleFormat = "sample/format";
const QString Settings::sampleRate = "sample/rate";
const QString Settings::sampleChannels = "sample/channels";

const QString Settings::bufferMaxLength = "buffer/maxLength";
const QString Settings::bufferTLength = "buffer/tLength";
const QString Settings::bufferPrebuf = "buffer/prebuf";
const QString Settings::bufferMinReq = "buffer/minReq";
const QString Settings::bufferFragSize = "buffer/fragSize";


Settings::Settings(const QString &fileName)
    : m_settings (fileName, QSettings::IniFormat)
    , m_defaults()
{
    initDefaults();
}

Settings &Settings::instance()
{
    static Settings theSingleInstance(QSettings().fileName());
    return theSingleInstance;
}

void Settings::initDefaults()
{
    setDefaultValue(pulseApplicationName, "RadioSim");
    setDefaultValue(usePlaybackAsyncAccessModel, false);
    setDefaultValue(useRecordAsyncAccessModel, true);

    setDefaultValue(sampleFormat, 3);
    setDefaultValue(sampleRate, 48000);
    setDefaultValue(sampleChannels, 2);

    setDefaultValue(bufferMaxLength, (uint32_t) - 1);
    setDefaultValue(bufferTLength, 1024);
    setDefaultValue(bufferPrebuf, 0);
    setDefaultValue(bufferMinReq, 1024);
    setDefaultValue(bufferFragSize, 1024);
}

void Settings::setDefaultValue(const QString &key, const QVariant &defaultValue)
{
    m_defaults.insert(key,defaultValue);
    if( !m_settings.contains(key) )
        m_settings.setValue(key,defaultValue);
}

QVariant Settings::value(const QString & key, const QVariant &def)
{
    if( instance().m_settings.contains(key) )
        return instance().m_settings.value(key);
    else if( instance().m_defaults.contains( key ) )
       return instance().m_defaults.value(key);
    else
        return def;
}
