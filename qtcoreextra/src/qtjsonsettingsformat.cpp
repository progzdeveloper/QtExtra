#include "qtjsonsettingsformat.h"
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QIODevice>

#include <QVector>
#include <QQueue>

#include <QDebug>


void writePath(QJsonObject& root, const QString& path, const QJsonValue& value)
{
    if (path.isEmpty())
        return;

    int index = path.indexOf('/', 0);
    QString key = path.mid(0, index);
    //qDebug() << key;

    QJsonObject json;
    if (root.contains(key))
        json = root[key].toObject();

    if (index >= 0) {
        writePath(json, path.mid(index + 1), value);
    }
    else {
        root[key] = value;
        return;
    }
    root[key] = json;
}


void readPath(const QJsonObject& root, QString& path, QSettings::SettingsMap &map)
{
    QJsonValue typeObject = root["type"];
    QJsonValue valueObject = root["value"];
    if (root.size() == 2 && !typeObject.isUndefined() && !valueObject.isUndefined()) {
        QString typeName = typeObject.toString();
        QVariant::Type type = QVariant::nameToType(typeName.toLatin1());
        QVariant value = valueObject.toVariant();
        if (value.type() != type) {
            if (type == QVariant::ByteArray) {
                value = QByteArray::fromBase64(value.toByteArray());
            } else {
                if (value.canConvert(type))
                    value.convert(type);
            }
        }
        map.insert(path, value);
    } else {
        for (auto it = root.begin(); it != root.end(); ++it) {
            if (!path.isEmpty())
                path += '/';
            path += it.key();
            readPath((*it).toObject(), path, map);
            int index = path.lastIndexOf('/');
            if (index > 0)
                path.truncate(index);
        }
        int index = path.lastIndexOf('/');
        if (index > 0)
            path.truncate(index);
        else
            path.clear();
    }
}


bool QtJsonSettingsFormat::read(QIODevice &device, QSettings::SettingsMap &map)
{
    QJsonParseError jsonError;
    QJsonDocument document = QJsonDocument::fromJson(device.readAll(), &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qCritical() << "failed to read settings: " << jsonError.errorString() << "at" << jsonError.offset;
        return false;
    }
    QJsonObject rootObject = document.object();
    QString path;
    readPath(rootObject, path, map);
    return true;
}


bool QtJsonSettingsFormat::write(QIODevice &device, const QSettings::SettingsMap &map)
{
    QJsonObject root;
    for (auto it = map.begin(); it != map.end(); ++it) {
         QJsonObject valueObject;
         QVariant value = it.value();
         switch (value.type()) {
         case QVariant::Bool:
         case QVariant::UInt:
         case QVariant::Int:
         case QVariant::ULongLong:
         case QVariant::LongLong:
         case QVariant::Double:
         case QVariant::String:
             valueObject["value"] = QJsonValue::fromVariant(value);
             break;
         case QVariant::ByteArray:
             valueObject["value"] = QString(value.toByteArray().toBase64());
             break;
         default:
             valueObject["value"] = value.toString();
             break;
         }
         valueObject["type"] = value.typeName();
         writePath(root, it.key(), valueObject);
    }
    device.write(QJsonDocument(root).toJson());
    return true;
}

void QtJsonSettingsFormat::registrateDefault(const QString& ext)
{
    QSettings::Format format = QSettings::registerFormat(ext, &QtJsonSettingsFormat::read, &QtJsonSettingsFormat::write);
    QSettings::setDefaultFormat(format);
}

