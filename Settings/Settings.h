#include <QString>
#include <QSettings>
#include <QVariant>


namespace pulse
{

    class Settings
    {
        // Singleton notatin
        explicit Settings(const QString &fileName = "");
        Settings(const Settings& );
        Settings& operator=(const Settings& );

        ~Settings() = default;

    public:
        static Settings &instance(const QString &fileName = "");
        static QVariant value(const QString &key, const QVariant &def = QVariant());

        static const QString pulseApplicationName;

        static const QString sampleFormat;
        static const QString sampleRate;
        static const QString sampleChannels;

        static const QString bufferMaxLength;
        static const QString bufferTLength;
        static const QString bufferPrebuf;
        static const QString bufferMinReq;
        static const QString bufferFragSize;

    private:
        void initDefaults();
        void setDefaultValue(const QString &key, const QVariant &defaultValue);

        QSettings                   m_settings;
        QHash<QString,QVariant>     m_defaults;
    };

}



