#ifndef QTINVOKER_H
#define QTINVOKER_H

#include <QObject>
#include <QHash>

#include <QtCoreExtra>

class QVariant;

/*!
 * \brief The QtMethodInvoker class
 * Helper class for invoking meta-methods on
 * QObject-based classes.
 *
 * The target QObject-derived class must have methods
 * maked  with Q_INVOKABLE macro, and must provide binding
 * information using Q_CLASSINFO macro for binding method
 * name to some arbitrary string key. This string key is
 * then used during invocation to find method which
 * must be called.
 *
 * When user of this class setups meta-object, the binding
 * information of method is extracted from meta-object
 * automatically and stored internally.
 *
 * The invocation of method by key is done in three steps:
 * 1) Key search in internal hash table
 * 2) If key was found the value that associated with key
 * is used as method name.
 * 3) Method arguments is preparing and method invocation
 * takes place through Qt Meta-Object System.
 *
 */
class QTCOREEXTRA_EXPORT QtMethodInvoker
{
    Q_DISABLE_COPY(QtMethodInvoker)
public:

    /*!
     * \brief MethodMap type of method binding map
     */
    typedef QHash<QByteArray, QMetaMethod> MethodMap;

    /*!
     * \brief QtMethodInvoker
     *
     * Default constructor
     */
    QtMethodInvoker();
    /*!
     * \brief QtMethodInvoker
     *
     * Construct QtMethodInvoker class and
     * setup the meta-object to invoke methods
     * \param metaObject
     */
    QtMethodInvoker(const QMetaObject* metaObject);

    /*!
     * \brief ~QtMethodInvoker
     */
    virtual ~QtMethodInvoker();

    /*!
     * \brief setMetaObject
     *
     * Reset the meta-object
     * \param metaObject
     */
    void setMetaObject(const QMetaObject* metaObject);
    /*!
     * \brief setMetaObject
     *
     * Reset the meta-object
     * \param object
     */
    void setMetaObject(QObject* object);

    /*!
     * \brief metaObject
     * \return meta-object that currently in use
     */
    const QMetaObject *metaObject() const;

    // invoke methods

    /*!
     * \brief invoke
     *
     * Invoke method specified by \a key on object \a object with arguments \a args
     * and store invokation reslt in \a result.
     * \param object object instance to invoke method on
     * \param key key of method
     * \param args method arguments packed as QVariantList
     * \param result method invokation result
     * \return true on successfull invokation, otherwise return false
     */
    bool invoke(QObject* object, const QString& key, const QVariantList& args, QVariant& result) const;
    /*!
     * \brief invoke
     *
     * Invoke method specified by \a key on object \a object with arguments \a args.
     * Result of method invokation is ignored.
     * \param object object instance to invoke method on
     * \param key key of method
     * \param args method arguments packed as QVariantList
     * \param result method result
     * \return true on successfull invokation, otherwise return false
     */
    bool invoke(QObject* object, const QString &key, const QVariantList &args) const;

    /*!
     * \brief invoke
     * Invoke method specified by \a key on object \a object with arguments
     * val0 ... val9
     * \param object object instance to invoke method on
     * \param key key of method
     * \param returnValue method invokation result
     * \param val0 argument with index 0
     * \param val1 argument with index 1
     * \param val2 argument with index 2
     * \param val3 argument with index 3
     * \param val4 argument with index 4
     * \param val5 argument with index 5
     * \param val6 argument with index 6
     * \param val7 argument with index 7
     * \param val8 argument with index 8
     * \param val9 argument with index 9
     * \return true on successfull invokation, otherwise return false
     */
    bool invoke(QObject *object, const QString &key,
                QGenericReturnArgument returnValue,
                QGenericArgument val0 = QGenericArgument(Q_NULLPTR),
                QGenericArgument val1 = QGenericArgument(),
                QGenericArgument val2 = QGenericArgument(),
                QGenericArgument val3 = QGenericArgument(),
                QGenericArgument val4 = QGenericArgument(),
                QGenericArgument val5 = QGenericArgument(),
                QGenericArgument val6 = QGenericArgument(),
                QGenericArgument val7 = QGenericArgument(),
                QGenericArgument val8 = QGenericArgument(),
                QGenericArgument val9 = QGenericArgument()) const;

    /*!
     * \brief invoke
     * Invoke method specified by \a key on object \a object with arguments
     * val0 ... val9.
     * Result of method invokation is ignored.
     * \param object object instance to invoke method on
     * \param key key of method
     * \param val0 argument with index 0
     * \param val1 argument with index 1
     * \param val2 argument with index 2
     * \param val3 argument with index 3
     * \param val4 argument with index 4
     * \param val5 argument with index 5
     * \param val6 argument with index 6
     * \param val7 argument with index 7
     * \param val8 argument with index 8
     * \param val9 argument with index 9
     * \return true on successfull invokation, otherwise return false
     */
    bool invoke(QObject *object, const QString &key,
                QGenericArgument val0 = QGenericArgument(Q_NULLPTR),
                QGenericArgument val1 = QGenericArgument(),
                QGenericArgument val2 = QGenericArgument(),
                QGenericArgument val3 = QGenericArgument(),
                QGenericArgument val4 = QGenericArgument(),
                QGenericArgument val5 = QGenericArgument(),
                QGenericArgument val6 = QGenericArgument(),
                QGenericArgument val7 = QGenericArgument(),
                QGenericArgument val8 = QGenericArgument(),
                QGenericArgument val9 = QGenericArgument()) const;


#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)

    /*!
     * \brief invoke
     * Invoke method specified by \a key on gadget \a gadget with arguments
     * val0 ... val9.
     * \param object object instance to invoke method on
     * \param key key of method
     * \param returnValue method invokation result
     * \param val0 argument with index 0
     * \param val1 argument with index 1
     * \param val2 argument with index 2
     * \param val3 argument with index 3
     * \param val4 argument with index 4
     * \param val5 argument with index 5
     * \param val6 argument with index 6
     * \param val7 argument with index 7
     * \param val8 argument with index 8
     * \param val9 argument with index 9
     * \return true on successfull invokation, otherwise return false
     * \note this function is supported for Qt 5.5.0 and higher
     * \warning target \a gadget must have Q_GADGET macro
     */
    bool invokeMethod(void* gadget, const QString &key,
                      QGenericReturnArgument returnValue,
                      QGenericArgument val0 = QGenericArgument(Q_NULLPTR),
                      QGenericArgument val1 = QGenericArgument(),
                      QGenericArgument val2 = QGenericArgument(),
                      QGenericArgument val3 = QGenericArgument(),
                      QGenericArgument val4 = QGenericArgument(),
                      QGenericArgument val5 = QGenericArgument(),
                      QGenericArgument val6 = QGenericArgument(),
                      QGenericArgument val7 = QGenericArgument(),
                      QGenericArgument val8 = QGenericArgument(),
                      QGenericArgument val9 = QGenericArgument()) const;

    /*!
     * \brief invoke
     * Invoke method specified by \a key on gadget \a gadget with arguments
     * val0 ... val9.
     * Result of method invokation is ignored.
     * \param object object instance to invoke method on
     * \param key key of method
     * \param val0 arguiment with index 0
     * \param val1 arguiment with index 1
     * \param val2 arguiment with index 2
     * \param val3 arguiment with index 3
     * \param val4 arguiment with index 4
     * \param val5 arguiment with index 5
     * \param val6 arguiment with index 6
     * \param val7 arguiment with index 7
     * \param val8 arguiment with index 8
     * \param val9 arguiment with index 9
     * \return true on successfull invokation, otherwise return false
     * \note this function is supported for Qt 5.5.0 and higher
     * \warning target \a gadget must have Q_GADGET macro
     */
    bool invokeMethod(void *gadget, const QString &key,
                      QGenericArgument val0 = QGenericArgument(Q_NULLPTR),
                      QGenericArgument val1 = QGenericArgument(),
                      QGenericArgument val2 = QGenericArgument(),
                      QGenericArgument val3 = QGenericArgument(),
                      QGenericArgument val4 = QGenericArgument(),
                      QGenericArgument val5 = QGenericArgument(),
                      QGenericArgument val6 = QGenericArgument(),
                      QGenericArgument val7 = QGenericArgument(),
                      QGenericArgument val8 = QGenericArgument(),
                      QGenericArgument val9 = QGenericArgument()) const;
#endif

    /*!
     * \brief createArgs
     *
     * Prepare and packs arguments as QVariantList
     * for method with key \a key
     * \param key
     * \param args
     * \return
     */
    bool createArgs(const QString& key, QVariantList& args) const;

    /*!
     * \brief methodMapping
     * \return method binding mapping
     */
    const MethodMap& methodMapping() const {
        return mapping;
    }

protected:
    /*!
     * \brief resetMapping
     * \param metaObject meta-object
     *
     * reset internal method binding mapping
     * for meta-object \a metaObject
     */
    virtual void resetMapping(const QMetaObject* metaObject);

    /*!
     * \brief lookup
     *
     * Lookup for a \a key in internal method binding mapping
     * \param key method key
     * \return iterator of method binding mapping pointing
     * to key-value pair if searching was successfull, otherwise
     * return end-iterator
     */
    virtual MethodMap::const_iterator lookup(const QString& key) const;

private:
    MethodMap mapping;
    const QMetaObject* mMetaObject;
};





#endif // QTINVOKER_H
