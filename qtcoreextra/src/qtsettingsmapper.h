#pragma once
#include <QtCoreExtra>
#include <QtObjectMapper>

class QObject;
class QSettings;

class QTCOREEXTRA_EXPORT QtSettingsMapper :
        public QtObjectMapper<QSettings, const QSettings>
{
    Q_DISABLE_COPY(QtSettingsMapper)
public:
    QtSettingsMapper();
    ~QtSettingsMapper();


    // QtObjectMapper interface
protected:
    virtual bool write(QSettings &settings, QObject *obj, const QMetaProperty &p) const Q_DECL_OVERRIDE;
    virtual bool read(const QSettings &settings, QObject *obj, const QMetaProperty &p) const Q_DECL_OVERRIDE;
    virtual bool validate(const QSettings &settings, const QMetaProperty &p) const Q_DECL_OVERRIDE;
};



