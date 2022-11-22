#include "Settings.h"


using namespace pulse;


const QString Settings::pulseApplicationName = "pulseApplicationName";

const QString Settings::sampleFormat = "sample/Format";
const QString Settings::sampleRate = "sample/Rate";
const QString Settings::sampleChannels = "sample/Channels";

const QString Settings::bufferMaxLength = "buffer/MaxLength";
const QString Settings::bufferTLength = "buffer/TLength";
const QString Settings::bufferPrebuf = "buffer/Prebuf";
const QString Settings::bufferMinReq = "buffer/MinReq";
const QString Settings::bufferFragSize = "buffer/FragSize";


Settings::Settings(const QString &fileName)
    : m_settings (fileName, QSettings::IniFormat)
{
    initDefaults();
}

Settings &Settings::instance(const QString &fileName)
{
    static Settings theSingleInstance(fileName);
    return theSingleInstance;
}

void Settings::initDefaults()
{
    setDefaultValue(pulseApplicationName, "RadioSim");

    setDefaultValue(sampleFormat, "PA_SAMPLE_U8");
    setDefaultValue(sampleRate, 48000);
    setDefaultValue(sampleChannels, 2);

    setDefaultValue(bufferMaxLength, "default");
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
