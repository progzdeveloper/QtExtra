#ifndef QTGLOBALEXTRA_H
#define QTGLOBALEXTRA_H

#include <QtGlobal>
#include <QString>
#include <QObject>
#include <QMetaObject>

namespace Qt
{
    static inline const QMetaObject* metaObject(QObject* object) {
        return object->metaObject();
    }

    template<class T>
    static inline const QMetaObject* metaObject() {
        return &T::staticMetaObject;
    }


#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    template<class _Base>
    static inline bool inherits(const QMetaObject* metaObject) {
        return metaObject->inherits(Qt::metaObject<_Base>());
    }
#endif

    template<class _Base>
    static inline bool inherits(QObject* object) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
        return Qt::inherits<_Base>(object->metaObject());
#else
        return object->inherits(Qt::className<_Base>());
#endif
    }


    static inline const char* className(const QMetaObject *metaObject) {
        return (metaObject == Q_NULLPTR ? "" : metaObject->className());
    }

    static inline const char* className(const QObject* object) {
        return (object == Q_NULLPTR ? "" : Qt::className(object->metaObject()));
    }

    template<class T>
    static inline const char* className() {
        return className(Qt::metaObject<T>());
    }
}



#endif // QTGLOBALEXTRA_H
