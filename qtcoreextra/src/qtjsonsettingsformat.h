#ifndef QTJSONSETTINGSFORMAT_H
#define QTJSONSETTINGSFORMAT_H

#include <QtCoreExtra>

#include <QSettings>

class QTCOREEXTRA_EXPORT QtJsonSettingsFormat
{
    Q_DISABLE_COPY(QtJsonSettingsFormat)

    QtJsonSettingsFormat();
    ~QtJsonSettingsFormat();

public:
    static bool read(QIODevice &device, QSettings::SettingsMap &map);
    static bool write(QIODevice &device, const QSettings::SettingsMap &map);
    static void registrateDefault(const QString &ext = "json");
};


#endif // QTJSONSETTINGSFORMAT_H
