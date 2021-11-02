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
    bool write(QJsonObject& json, const QObject *obj, const QMetaProperty &p) const Q_DECL_OVERRIDE;
    bool read(const QJsonObject& json, QObject *obj, const QMetaProperty &p) const Q_DECL_OVERRIDE;
    bool validate(const QJsonObject& json, const QMetaProperty &p) const Q_DECL_OVERRIDE;
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
    bool write(QJsonObject& json, const QObject *obj, const QMetaProperty &p) const Q_DECL_OVERRIDE;
    bool read(const QJsonObject& json, QObject *obj, const QMetaProperty &p) const Q_DECL_OVERRIDE;
    bool validate(const QJsonObject& json, const QMetaProperty &p) const Q_DECL_OVERRIDE;
};

