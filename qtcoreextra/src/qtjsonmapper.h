#pragma once
#include <QtCoreExtra>
#include <QtObjectMapper>

struct QMetaObject;
class QMetaProperty;
class QObject;

class QJsonObject;


class QTCOREEXTRA_EXPORT QtJsonMapper :
        public QtObjectMapper<QJsonObject, const QJsonObject>
{
    Q_DISABLE_COPY(QtJsonMapper)

public:
    QtJsonMapper();
    ~QtJsonMapper();

    // QtObjectMapper interface
protected:
    virtual bool write(QJsonObject& json, QObject *obj, const QMetaProperty &p) const Q_DECL_OVERRIDE;
    virtual bool read(const QJsonObject& json, QObject *obj, const QMetaProperty &p) const Q_DECL_OVERRIDE;
    virtual bool validate(const QJsonObject& json, const QMetaProperty &p) const Q_DECL_OVERRIDE;
};



class QTCOREEXTRA_EXPORT QtTypedJsonMapper :
    public QtJsonMapper
{
    Q_DISABLE_COPY(QtTypedJsonMapper)

public:
    QtTypedJsonMapper();
    ~QtTypedJsonMapper();

    // QtJsonMapper interface
protected:
    virtual bool write(QJsonObject& json, QObject *obj, const QMetaProperty &p) const Q_DECL_OVERRIDE;
    virtual bool read(const QJsonObject& json, QObject *obj, const QMetaProperty &p) const Q_DECL_OVERRIDE;
    virtual bool validate(const QJsonObject& json, const QMetaProperty &p) const Q_DECL_OVERRIDE;
};

