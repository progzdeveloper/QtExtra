#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QMetaClassInfo>
#include <QDebug>

#include "qtmethodinvoker.h"



QtMethodInvoker::QtMethodInvoker()
{
}

QtMethodInvoker::QtMethodInvoker(const QMetaObject *metaObject)
{
    setMetaObject(metaObject);
}

QtMethodInvoker::~QtMethodInvoker()
{
}

void QtMethodInvoker::resetMapping(const QMetaObject *metaObject)
{
    mapping.clear();
    if (metaObject == Q_NULLPTR)
        return;

    QMetaMethod method;
    QHash<QByteArray, int> methods; // temporary method mapping
    for (int i = 0, n = metaObject->methodCount(); i < n; i++)
    {
        method = metaObject->method(i);
        methods.insert(method.name(), i);
    }

    QMetaClassInfo attribute;
    for (int idx = 0, n = metaObject->classInfoCount(); idx < n; idx++)
    {
        attribute = metaObject->classInfo(idx); // find attribute
        const char* methodKey = attribute.name(); // extract key
        auto it = methods.constFind(attribute.value());
        if (it != methods.cend())
            mapping.insert(methodKey, metaObject->method(it.value())); // update mapping
    }
}

QtMethodInvoker::MethodMap::const_iterator QtMethodInvoker::lookup(const QString &key) const
{
    return mapping.constFind(key.toLatin1());
}

void QtMethodInvoker::setMetaObject(const QMetaObject *metaObject)
{
    mMetaObject = metaObject;
    resetMapping(metaObject);
}

void QtMethodInvoker::setMetaObject(QObject *object)
{
    if (object)
        setMetaObject(object->metaObject());
}

const QMetaObject *QtMethodInvoker::metaObject() const
{
    return mMetaObject;
}


bool QtMethodInvoker::invoke(QObject* object, const QString &key, const QVariantList &args, QVariant &result) const
{
    auto it = lookup(key.toLatin1()); // lookup for a key
    if (it == mapping.cend()) {
        qWarning() << "requested method key" << key << "not found";
        return false;
    }
    if (it->parameterCount() != args.size()) { // incorrect number of args
        qWarning() << "incorrect number of arguemnts while invoking" << key;
        return false;
    }
    QGenericArgument ga[10];
    int i = 0;

    // prepare arguments
    for (auto argIt = args.cbegin(); argIt != args.cend(); ++argIt, ++i)
    {
        int t = static_cast<int>(argIt->type());
        if (t != it->parameterType(i)) {
            qWarning() << "argument [" << i << "] type mismatch: (expected:"
                       << QMetaType::typeName(it->parameterType(i))
                       << "actual:" << QMetaType::typeName(t)
                       << ") while invoking" << it.value().name()
                       << "method {aka" << it.key() << "}";
            return false;
        }
        ga[i] = QGenericArgument(argIt->typeName(), argIt->data());
    }
    int type = QMetaType::type(it->typeName());
    result = QVariant(type);
    QGenericReturnArgument resultArg( it->typeName(), const_cast<void*>(result.data()) );
#ifndef QT_NO_DEBUG
    qDebug() << "INVOKING:" << it->methodSignature();
#endif
    // do invoke
    return it->invoke(object, Qt::AutoConnection, resultArg,
                      ga[0], ga[1], ga[2], ga[3], ga[4],
                      ga[5], ga[6], ga[7], ga[8], ga[9]);
}

bool QtMethodInvoker::invoke(QObject* object, const QString &key, const QVariantList &args) const
{
    auto it = lookup(key.toLatin1());
    if (it == mapping.cend()) {
        qWarning() << "requested method key" << key << "not found";
        return false;
    }
    if (it->parameterCount() != args.size()) {
        qWarning() << "incorrect number of arguemnts while invoking" << key;
        return false;
    }

    QGenericArgument ga[10];
    int i = 0;
    for (auto argIt = args.cbegin(); argIt != args.cend(); ++argIt, ++i)
    {
        int t = static_cast<int>(argIt->type());
        int p = it->parameterType(i);
        if (t != p) {
            qWarning() << "argument [" << i << "] type mismatch: (expected:"
                       << QMetaType::typeName(p)
                       << "actual:" << QMetaType::typeName(t)
                       << ") while invoking" << it->name()
                       << "method {aka" << it.key() << "}";
            return false;
        }
        ga[i] = QGenericArgument(argIt->typeName(), argIt->data());
    }

#ifndef QT_NO_DEBUG
    qDebug() << "INVOKING:" << it->methodSignature();
#endif
    return it->invoke(object, Qt::AutoConnection,
                      ga[0], ga[1], ga[2], ga[3], ga[4],
                      ga[5], ga[6], ga[7], ga[8], ga[9]);
}

bool QtMethodInvoker::invoke(QObject *object, const QString &key,
                             QGenericReturnArgument returnValue,
                             QGenericArgument val0, QGenericArgument val1,
                             QGenericArgument val2, QGenericArgument val3,
                             QGenericArgument val4, QGenericArgument val5,
                             QGenericArgument val6, QGenericArgument val7,
                             QGenericArgument val8, QGenericArgument val9) const
{
    auto it = lookup(key.toLatin1());
    if (it == mapping.cend()) {
        qWarning() << "requested method key" << key << "not found";
        return false;
    }
#ifndef QT_NO_DEBUG
    qDebug() << "INVOKING:" << it->methodSignature();
#endif
    return it->invoke(object, Qt::AutoConnection, returnValue,
                      val0, val1, val2, val3, val4,
                      val5, val6, val7, val8, val9);
}

bool QtMethodInvoker::invoke(QObject *object, const QString &key,
                             QGenericArgument val0, QGenericArgument val1,
                             QGenericArgument val2, QGenericArgument val3,
                             QGenericArgument val4, QGenericArgument val5,
                             QGenericArgument val6, QGenericArgument val7,
                             QGenericArgument val8, QGenericArgument val9) const
{
    auto it = lookup(key.toLatin1());
    if (it == mapping.cend()) {
        qWarning() << "requested method key" << key << "not found";
        return false;
    }
#ifndef QT_NO_DEBUG
    qDebug() << "INVOKING:" << it->methodSignature();
#endif
    return it->invoke(object, Qt::AutoConnection,
                      val0, val1, val2, val3, val4,
                      val5, val6, val7, val8, val9);
}
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
bool QtMethodInvoker::invokeMethod(void *gadget, const QString &key,
                                   QGenericReturnArgument returnValue,
                                   QGenericArgument val0, QGenericArgument val1,
                                   QGenericArgument val2, QGenericArgument val3,
                                   QGenericArgument val4, QGenericArgument val5,
                                   QGenericArgument val6, QGenericArgument val7,
                                   QGenericArgument val8, QGenericArgument val9) const
{
    auto it = lookup(key.toLatin1());
    if (it == mapping.cend()) {
        qWarning() << "requested method key" << key << "not found";
        return false;
    }
#ifndef QT_NO_DEBUG
    qDebug() << "INVOKING:" << it->methodSignature();
#endif
    return it->invokeOnGadget(gadget, returnValue,
                      val0, val1, val2, val3, val4,
                      val5, val6, val7, val8, val9);
}

bool QtMethodInvoker::invokeMethod(void *gadget, const QString &key,
                                   QGenericArgument val0, QGenericArgument val1,
                                   QGenericArgument val2, QGenericArgument val3,
                                   QGenericArgument val4, QGenericArgument val5,
                                   QGenericArgument val6, QGenericArgument val7,
                                   QGenericArgument val8, QGenericArgument val9) const
{
    auto it = lookup(key.toLatin1());
    if (it == mapping.cend()) {
        qWarning() << "requested method key" << key << "not found";
        return false;
    }
#ifndef QT_NO_DEBUG
    qDebug() << "INVOKING:" << it->methodSignature();
#endif
    return it->invokeOnGadget(gadget,
                      val0, val1, val2, val3, val4,
                      val5, val6, val7, val8, val9);
}
#endif

bool QtMethodInvoker::createArgs(const QString &key, QVariantList &args) const
{
    auto it = lookup(key.toLatin1());
    if (it == mapping.cend())
        return false;

    args.clear();
    for (int i = 0, n = it->parameterCount(); i < n; i++)
    {
        QVariant::Type type = static_cast<QVariant::Type>(it->parameterType(i));
        args.push_back(QVariant(type));
    }
    return true;
}

