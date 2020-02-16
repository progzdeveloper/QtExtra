#include "qtattributeresource.h"
#include <QDebug>
#include <QFile>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include "qtpropertybrowser.h"

QtAttributeResource::~QtAttributeResource()
{
}

bool QtAttributeResource::read(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly)) {
        setErrorText(file.errorString());
        return false;
    }
    return read(&file);
}

bool QtAttributeResource::write(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::WriteOnly)) {
        setErrorText(file.errorString());
        return false;
    }
    return write(&file);
}

QStringList QtAttributeResource::keys() const
{
    return QStringList();
}


void QtAttributeResource::setErrorText(const QString &text)
{
    qDebug() << "[ERROR]: " << qPrintable(text);
    mErrorText = text;
}

QString QtAttributeResource::errorString() const
{
    return mErrorText;
}








QtJsonAttributeResource::QtJsonAttributeResource()
{
}

QtJsonAttributeResource::~QtJsonAttributeResource()
{
}

bool QtJsonAttributeResource::read(const QMetaObject *metaObject)
{
    if (!metaObject) {
        setErrorText("invalid meta-object reference");
        return false;
    }

    if (metaObject->classInfoCount() == 0) {
        return true; // fast exit: no attributes
    }

    const char* key = Q_NULLPTR;
    const char* value = Q_NULLPTR;
    const char* className = metaObject->className();
    int index = -1;

    QJsonObject jsonClass;
    QJsonParseError jsonErr;
    for(int i = 0; i < metaObject->propertyCount(); ++i)
    {
        key = metaObject->property(i).name();
        index = metaObject->indexOfClassInfo(key);
        if (index < 0)
            continue;
        value = metaObject->classInfo(index).value();
        QJsonObject json = QJsonDocument::fromJson(value, &jsonErr).object();
        if (jsonErr.error != QJsonParseError::NoError) {
            qWarning() << "failed to parse attributes for [" << className << '.' << key << "]:"
                       << qPrintable(jsonErr.errorString());
            continue;
        }
        jsonClass[key] = json;
    }

    index = metaObject->indexOfClassInfo(className);
    if (index >= 0) {
       jsonClass[className] = metaObject->classInfo(index).value();
    }

    classMap[className] = jsonClass;
    return true;
}

bool QtJsonAttributeResource::read(QIODevice *device)
{
    if (!verifyDevice(device, QIODevice::ReadOnly))
        return false;

    QJsonParseError jsonErr;
    classMap = QJsonDocument::fromJson(device->readAll(), &jsonErr).object();
    if (jsonErr.error != QJsonParseError::NoError) {
        setErrorText("Invalid JSON: " +
                     jsonErr.errorString() +
                     " at offset:" +
                     QString::number(jsonErr.offset));
        return false;
    }
    return true;
}

bool QtJsonAttributeResource::write(QIODevice *device)
{
    if (!verifyDevice(device, QIODevice::WriteOnly))
        return false;

    device->write(QJsonDocument(classMap).toJson(QJsonDocument::Compact));
    return true;
}

void QtJsonAttributeResource::setup(const char *className, QtVariantProperty *p)
{
    auto clsIt = classMap.find(className);
    if (clsIt == classMap.end()) { return; }

    QJsonObject clsObj = (*clsIt).toObject();
    auto attrIt = clsObj.find(p->key());
    if (attrIt == clsObj.end()) { return; }

    if (attrIt.key() == p->key()) { // class name
        p->setPropertyName((*attrIt).toString());
    }

    QJsonObject attributes = (*attrIt).toObject();
    for (auto it = attributes.begin(); it != attributes.end(); ++it)
    {
        if (it.key() == p->key())
            p->setPropertyName((*it).toString());
        else
            p->setAttribute(it.key(), (*it).toVariant());
    }
}

bool QtJsonAttributeResource::contains(const char *className, const char *propertyName) const
{
    auto it = classMap.find(className);
    if (propertyName == Q_NULLPTR) {
        return (it != classMap.end());
    } else {
        return (it != classMap.end() ? (*it).toObject().contains(propertyName) : false);
    }
}

void QtJsonAttributeResource::erase(const char *className)
{
    auto it = classMap.find(className);
    if (it != classMap.end()) {
        classMap.erase(it);
    }
}

void QtJsonAttributeResource::clear()
{
    classMap = QJsonObject();
}

QStringList QtJsonAttributeResource::keys() const
{
    return classMap.keys();
}

bool QtJsonAttributeResource::verifyDevice(QIODevice* device, QIODevice::OpenMode mode)
{
    if (!device) {
        setErrorText("invalid device reference");
        return false;
    }
    if (!device->isOpen()) {
        setErrorText("device was not open");
        return false;
    }
    if ((mode & QIODevice::ReadOnly) && !device->isReadable()) {
        setErrorText("device in not readable");
        return false;
    }
    if ((mode & QIODevice::WriteOnly) && !device->isWritable()) {
        setErrorText("device in not writable");
        return false;
    }
    return true;
}
