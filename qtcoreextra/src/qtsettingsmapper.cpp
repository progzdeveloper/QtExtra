#include <QObject>
#include <QSettings>
#include <QMetaObject>
#include <QMetaProperty>
#include <QHash>

#include "qtsettingsmapper.h"



QtSettingsMapper::QtSettingsMapper()
{

}

QtSettingsMapper::~QtSettingsMapper()
{

}

bool QtSettingsMapper::write(QSettings &settings, QObject *obj, const QMetaProperty &p) const
{
    const char* className = obj->metaObject()->className();
    QString path;
    path += className;
    path += '/';
    path += p.name();
    settings.setValue(path, p.read(obj));
    return true;
}

bool QtSettingsMapper::read(const QSettings &settings, QObject *obj, const QMetaProperty &p) const
{
    const char* className = obj->metaObject()->className();
    QString path;
    path += className;
    path += '/';
    path += p.name();
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
