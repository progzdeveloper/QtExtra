#ifndef QTOBJECTMAPPER_H
#define QTOBJECTMAPPER_H

#include <QtCoreExtra>
#include <QScopedPointer>
#include <QObject>

struct QMetaObject;
class QMetaProperty;

class QRegExp;



class QTCOREEXTRA_EXPORT QtAbstractObjectMapper
{
    Q_DISABLE_COPY(QtAbstractObjectMapper)
public:
    QtAbstractObjectMapper();
    virtual ~QtAbstractObjectMapper();

    void setClassFilter(const QString& pattern);
    QString classFilter() const;

    void setFinal(bool on = true);
    bool isFinal() const;

protected:
    virtual bool accepted(const QString &className) const;

    virtual bool write(void* w, const QObject* obj, const QMetaObject* metaClass) const;
    virtual bool write(void* w, const QObject *obj, const QMetaProperty& p) const;

    virtual bool read(void* r, QObject* obj, const QMetaObject* metaClass) const;
    virtual bool read(void* r, QObject *obj, const QMetaProperty& p) const;

    virtual bool validate(void* v, const QMetaObject* metaClass) const;
    virtual bool validate(void* v, const QMetaProperty& p) const;

private:
    QT_PIMPL(QtAbstractObjectMapper)
};


template<
    class _Writer,
    class _Reader
>
class QtObjectMapper :
        public QtAbstractObjectMapper
{
public:
    QtObjectMapper() :
        QtAbstractObjectMapper() {
    }

    ~QtObjectMapper() {
    }

    bool serialize(_Writer& w, const QObject *obj) const;
    bool unserialize(_Reader& r, QObject *obj) const;

    template<class _Object>
    inline bool validateObject(_Reader &r) const {
        return QtAbstractObjectMapper::validate(const_cast<void*>((void*)&r), &_Object::staticMetaObject);
    }

protected:
    virtual bool write(_Writer& w, const QObject *obj, const QMetaProperty &p) const = 0;
    virtual bool read(_Reader& r, QObject *obj, const QMetaProperty &p) const = 0;
    virtual bool validate(_Reader& v, const QMetaProperty &p) const = 0;

    // QtAbstractObjectMapper interface
private:
    bool write(void *w, const QObject *obj, const QMetaProperty &p) const Q_DECL_OVERRIDE;
    bool read(void *r, QObject *obj, const QMetaProperty &p) const Q_DECL_OVERRIDE;
    bool validate(void *v, const QMetaProperty &p) const Q_DECL_OVERRIDE;
};



template<class _Writer, class _Reader>
bool QtObjectMapper<_Writer, _Reader>::serialize(_Writer &w, const QObject *obj) const
{
    if (!obj)
        return false;
    return QtAbstractObjectMapper::write(&w, obj, obj->metaObject());
}

template<class _Writer, class _Reader>
bool QtObjectMapper<_Writer, _Reader>::unserialize(_Reader &r, QObject *obj) const
{
    if (!obj)
        return false;
    return QtAbstractObjectMapper::read(const_cast<void*>((void*)&r), obj, obj->metaObject());
}


template<class _Writer, class _Reader>
bool QtObjectMapper<_Writer, _Reader>::write(void *w, const QObject *obj, const QMetaProperty &p) const
{
    Q_ASSERT(w != Q_NULLPTR);
    return this->write(*reinterpret_cast<_Writer*>(w), obj, p);
}

template<class _Writer, class _Reader>
bool QtObjectMapper<_Writer, _Reader>::read(void *r, QObject *obj, const QMetaProperty &p) const
{
    Q_ASSERT(r != Q_NULLPTR);
    return this->read(*reinterpret_cast<_Reader*>(r), obj, p);
}

template<class _Writer, class _Reader>
bool QtObjectMapper<_Writer, _Reader>::validate(void *v, const QMetaProperty &p) const
{
    Q_ASSERT(v != Q_NULLPTR);
    return this->validate(*reinterpret_cast<_Reader*>(v), p);
}





#endif // QTOBJECTMAPPER_H
