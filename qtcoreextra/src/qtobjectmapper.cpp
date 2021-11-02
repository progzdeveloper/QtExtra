#include "qtobjectmapper.h"
#include <QRegExp>
#include <QMetaObject>
#include <QMetaProperty>

class QtAbstractObjectMapperPrivate
{
public:
    QRegExp regExp;
    bool isFinal;
};

QtAbstractObjectMapper::QtAbstractObjectMapper() :
    d_ptr(new QtAbstractObjectMapperPrivate)
{
    d_ptr->isFinal = false;
}

QtAbstractObjectMapper::~QtAbstractObjectMapper()
{
}

void QtAbstractObjectMapper::setClassFilter(const QString &pattern)
{
    Q_D(QtAbstractObjectMapper);
    d->regExp.setPattern(pattern);
}

QString QtAbstractObjectMapper::classFilter() const
{
    Q_D(const QtAbstractObjectMapper);
    return d->regExp.pattern();
}

void QtAbstractObjectMapper::setFinal(bool on)
{
    Q_D(QtAbstractObjectMapper);
    d->isFinal = on;
}

bool QtAbstractObjectMapper::isFinal() const
{
    Q_D(const QtAbstractObjectMapper);
    return d->isFinal;
}

bool QtAbstractObjectMapper::accepted(const QString& className) const
{
    Q_D(const QtAbstractObjectMapper);
    return (d->regExp.isValid() && className.contains(d->regExp));
}


bool QtAbstractObjectMapper::write(void *w, const QObject *obj, const QMetaObject *metaClass) const
{
    for (; metaClass != Q_NULLPTR; metaClass = metaClass->superClass())
    {
        if (!accepted(metaClass->className()))
            continue;

        for (int i = metaClass->propertyOffset(), n = metaClass->propertyCount(); i < n; i++) {
            if (!this->write(w, obj, metaClass->property(i))) {
                return false;
            }
        }

        if (isFinal())
            break;
    }
    return true;
}

bool QtAbstractObjectMapper::write(void*, const QObject*, const QMetaProperty&) const
{
    return false;
}

bool QtAbstractObjectMapper::read(void *r, QObject *obj, const QMetaObject *metaClass) const
{
    for (; metaClass != Q_NULLPTR; metaClass = metaClass->superClass())
    {
        if (!accepted(metaClass->className()))
            continue;

        for (int i = metaClass->propertyOffset(), n = metaClass->propertyCount(); i < n; i++) {
            if (!this->read(r, obj, metaClass->property(i))) {
                return false;
            }
        }

        if (isFinal())
            break;
    }
    return true;
}

bool QtAbstractObjectMapper::read(void*, QObject*, const QMetaProperty&) const
{
    return false;
}

bool QtAbstractObjectMapper::validate(void *v, const QMetaObject *metaClass) const
{
    for (; metaClass != Q_NULLPTR; metaClass = metaClass->superClass())
    {
        if (!accepted(metaClass->className()))
            continue;

        for (int i = metaClass->propertyOffset(), n = metaClass->propertyCount(); i < n; i++) {
            if (!this->validate(v, metaClass->property(i))) {
                return false;
            }
        }

        if (isFinal())
            break;
    }
    return true;
}

bool QtAbstractObjectMapper::validate(void *, const QMetaProperty &) const
{
    return false;
}




