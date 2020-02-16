#include "qtjsonmapper.h"
#include <QJsonObject>
#include <QJsonValue>

#include <QMetaProperty>
#include <QMetaObject>

static const int qtJsonVariantMapping[] = {
    QVariant::Invalid,  // QJsonValue::Null = 0x0
    QVariant::Bool,     // QJsonValue::Bool = 0x1
    QVariant::Double,   // QJsonValue::Double = 0x2
    QVariant::String    // QJsonValue::String = 0x3
};

static const int qtJsonVariantMappingSize =
        sizeof(qtJsonVariantMapping) / sizeof(qtJsonVariantMapping[0]);



QtJsonMapper::QtJsonMapper(){
}

QtJsonMapper::~QtJsonMapper(){
}

bool QtJsonMapper::write(QJsonObject& json, QObject *obj, const QMetaProperty &p) const
{
    if (!p.isReadable())
        return false;

    QVariant value = p.read(obj);
    int type = value.type();
    if (type < 0 || type >= qtJsonVariantMappingSize)
        json[p.name()] = value.toString();
    else
        json[p.name()] = QJsonValue::fromVariant(value);

    return true;
}

bool QtJsonMapper::read(const QJsonObject& json, QObject *obj, const QMetaProperty &p) const
{
    if (!p.isWritable())
        return true;

    const char* name = p.name();
    QVariant value = json[QLatin1String(name)].toVariant();
    if (!value.convert(p.type()))
        return false; // failed to convert
    return (p.write(obj, value));
}

bool QtJsonMapper::validate(const QJsonObject& json, const QMetaProperty &p) const
{
    QJsonObject::const_iterator it = json.find(p.name());
    if (it == json.end())
        return false; // name not found

    int type = static_cast<int>((*it).type());
    if (type < 0 || type >= qtJsonVariantMappingSize)
        return false; // incorrect type

    return true;
}






QtTypedJsonMapper::QtTypedJsonMapper()
{
}

QtTypedJsonMapper::~QtTypedJsonMapper()
{
}

bool QtTypedJsonMapper::validate(const QJsonObject &jsonObject, const QMetaProperty &p) const
{
    QJsonObject::const_iterator it = jsonObject.find(p.name());
    if (it == jsonObject.end())
        return false;

    if ((*it).toObject()["type"].toString() != p.typeName())
        return false;

    return true;
}

bool QtTypedJsonMapper::write(QJsonObject &jsonObject, QObject *obj, const QMetaProperty &p) const
{
    if (!p.isReadable())
        return true;

    QJsonObject json;
    json["type"] = p.typeName();
    json["value"] = p.read(obj).toString();

    jsonObject[p.name()] = json;
    return true;
}

bool QtTypedJsonMapper::read(const QJsonObject& json, QObject *obj, const QMetaProperty &p) const
{
    if (!p.isWritable())
        return true;

    QJsonObject jsonObject = json[p.name()].toObject();
    QString typeName = jsonObject["type"].toString();
    QVariant value(jsonObject["value"].toString());
    value.convert(QVariant::nameToType(typeName.toLatin1()));
    return (p.write(obj, value));
}
