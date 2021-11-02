#ifndef QTATTRIBUTERESOURCE_H
#define QTATTRIBUTERESOURCE_H

#include <QStringList>
#include <QMetaObject>
#include <QMetaProperty>
#include <QJsonObject>

#include "qtvariantproperty.h"

class QIODevice;
class QtVariantProperty;

class QT_QTPROPERTYBROWSER_EXPORT QtAttributeResource
{
public:
    virtual ~QtAttributeResource();

    virtual bool read(const QMetaObject* metaObject) = 0;
    virtual bool read(QIODevice* device) = 0;
    bool read(const QString& path);

    virtual bool write(QIODevice* device) = 0;
    bool write(const QString& path);


    virtual void setup(const char* className, QtVariantProperty* p) = 0;
    inline void setup(const QMetaObject* metaObject, QtVariantProperty* p) {
        setup(metaObject->className(), p);
    }

    virtual bool contains(const char* className, const char* propertyName = Q_NULLPTR) const = 0;
    inline bool contains(const QMetaObject* metaObject, const char* propertyName = Q_NULLPTR) const {
        return contains(metaObject->className(), propertyName);
    }
    inline bool contains(const QMetaObject* metaObject, const QMetaProperty& property) const {
        return contains(metaObject->className(), property.name());
    }

    virtual void erase(const char* className) = 0;
    void erase(const QMetaObject* metaObject) {
        erase(metaObject->className());
    }

    virtual void clear() = 0;

    virtual QStringList keys() const;

    QString errorString() const;

protected:
    void setErrorText(const QString& text);

private:
    QString mErrorText;
};



class QT_QTPROPERTYBROWSER_EXPORT QtJsonAttributeResource :
        public QtAttributeResource
{
public:
    QtJsonAttributeResource();
    virtual ~QtJsonAttributeResource();

    // QtAttributeResource interface
public:
    bool read(const QMetaObject *metaObject) Q_DECL_OVERRIDE;
    bool read(QIODevice *device) Q_DECL_OVERRIDE;
    bool write(QIODevice *device) Q_DECL_OVERRIDE;
    void setup(const char *className, QtVariantProperty *p) Q_DECL_OVERRIDE;
    bool contains(const char *className, const char *propertyName = Q_NULLPTR) const Q_DECL_OVERRIDE;
    void erase(const char *className) Q_DECL_OVERRIDE;
    void clear() Q_DECL_OVERRIDE;
    QStringList keys() const Q_DECL_OVERRIDE;

private:
    bool verifyDevice(QIODevice* device,
                      QIODevice::OpenMode mode);
private:
    QJsonObject classMap;
};









#endif // QTATTRIBUTERESOURCE_H
