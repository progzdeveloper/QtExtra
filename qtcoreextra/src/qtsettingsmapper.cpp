#include <QObject>
#include <QSettings>
#include <QMetaObject>
#include <QMetaProperty>
#include <QHash>

#include "qtsettingsmapper.h"

#include "qtglobalextra.h"

QtSettingsMapper::QtSettingsMapper()
{

}

QtSettingsMapper::~QtSettingsMapper()
{

}

bool QtSettingsMapper::write(QSettings &settings, const QObject *obj, const QMetaProperty &p) const
{
    const QByteArray className = QtExtra::className(obj);
    const QByteArray propertyName = p.name();
    QString path;
    path.reserve(className.size() + propertyName.size() + 1);
    path += className;
    path += '/';
    path += propertyName;
    settings.setValue(path, p.read(obj));
    return true;
}

bool QtSettingsMapper::read(const QSettings &settings, QObject *obj, const QMetaProperty &p) const
{
    const QByteArray className = QtExtra::className(obj);
    const QByteArray propertyName = p.name();
    QString path;
    path.reserve(className.size() + propertyName.size() + 1);
    path += className;
    path += '/';
    path += propertyName;
    return p.write(obj, settings.value(path));
}

bool QtSettingsMapper::validate(const QSettings &, const QMetaProperty &) const
{
    /*const char* className = obj->metaObject()->className();
    QString path;
    path += className;
    path += '/';
    path += p.name();
    return settings.contains(path);*/
    return false;
}
