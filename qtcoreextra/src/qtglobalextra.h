#ifndef QTGLOBALEXTRA_H
#define QTGLOBALEXTRA_H

#include <QtGlobal>
#include <QString>
#include <QObject>
#include <QMetaObject>

namespace QtExtra
{
    static inline const QMetaObject* metaObject(const QObject* object) {
        return object->metaObject();
    }

    template<class T>
    static inline const QMetaObject* metaObject() {
        return &T::staticMetaObject;
    }

    static inline const char* className(const QMetaObject *metaObject) {
        return (metaObject == Q_NULLPTR ? "" : metaObject->className());
    }

    static inline const char* className(const QObject* object) {
        return (object == Q_NULLPTR ? "" : QtExtra::className(object->metaObject()));
    }

    template<class T>
    static inline const char* className() {
        return QtExtra::className(QtExtra::metaObject<T>());
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
    template<class _Base>
    static inline bool inherits(const QMetaObject* metaObject) {
        return metaObject->inherits(QtExtra::metaObject<_Base>());
    }
#endif

    template<class _Base>
    static inline bool inherits(const QObject* object) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 7, 0))
        return QtExtra::inherits<_Base>(object->metaObject());
#else
        return object->inherits(QtExtra::className<_Base>());
#endif
    }



}



#endif // QTGLOBALEXTRA_H
