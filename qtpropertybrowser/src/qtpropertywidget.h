#pragma once
#include <QWidget>

#if defined(Q_OS_WIN)
#  if !defined(QT_QTPROPERTYBROWSER_EXPORT) && !defined(QT_QTPROPERTYBROWSER_IMPORT)
#    define QT_QTPROPERTYBROWSER_EXPORT
#  elif defined(QT_QTPROPERTYBROWSER_IMPORT)
#    if defined(QT_QTPROPERTYBROWSER_EXPORT)
#      undef QT_QTPROPERTYBROWSER_EXPORT
#    endif
#    define QT_QTPROPERTYBROWSER_EXPORT __declspec(dllimport)
#  elif defined(QT_QTPROPERTYBROWSER_EXPORT)
#    undef QT_QTPROPERTYBROWSER_EXPORT
#    define QT_QTPROPERTYBROWSER_EXPORT __declspec(dllexport)
#  endif
#else
#  if !defined(QT_QTPROPERTYBROWSER_EXPORT)
#    define QT_QTPROPERTYBROWSER_EXPORT
#  endif
#endif



#if QT_VERSION >= 0x040400
QT_BEGIN_NAMESPACE
#endif

#ifdef Q_CC_GNU
#define QT_EXT_DECL_USED __attribute__((used))
#else
#define QT_EXT_DECL_USED
#endif

#ifndef QT_METAINFO_TR
#define QT_METAINFO_TR(_ClassName) static const char * _ClassName##MetaInfoTr[] QT_EXT_DECL_USED =
#endif

#ifndef QT_TR_META
#define QT_TR_META(_ClassName, _Text) QT_TRANSLATE_NOOP(#_ClassName, _Text)
#endif


struct QMetaObject;

class QtProperty;

class QtAttributeResource;

class QT_QTPROPERTYBROWSER_EXPORT QtPropertyWidget :
    public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString classFilter READ classFilter WRITE setClassFilter NOTIFY classFilterChanged)
    Q_PROPERTY(QString propertyFilter READ propertyFilter WRITE setPropertyFilter NOTIFY propertyFilterChanged)
    Q_PROPERTY(SubmitPolicy submitPolicy READ submitPolicy WRITE setSubmitPolicy)
    Q_PROPERTY(bool isFinal READ isFinal WRITE setFinal)
    Q_PROPERTY(ViewType viewType READ viewType WRITE setViewType)
public:
    enum ViewType
    {
        TreeView = 0,
        GroupView = 1,
        ButtonView = 2
    };
    Q_ENUM(ViewType)

    enum SubmitPolicy
    {
        AutoSubmit   = 0,
        ManualSubmit = 1
    };
    Q_ENUM(SubmitPolicy)

    explicit QtPropertyWidget(QWidget *parent = Q_NULLPTR);
    ~QtPropertyWidget();

    void setResource(QtAttributeResource* resource);
    QtAttributeResource* resource() const;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 2, 0))
    template<class T>
    inline void setObject(T* object)
    {
        if (isQObject(&(T::staticMetaObject)))
            setupObject(reinterpret_cast<QObject*>(object));
        else
            setupGadget(object, &(T::staticMetaObject));
    }

    inline void setObject(std::nullptr_t) {
        setupObject((QObject*)0);
    }

    inline void setGadget(void* object, const QMetaObject* metaObject) {
        setupGadget(object, metaObject);
    }

    void* gadget() const;

    bool isGadget() const;
    bool isObject() const;
#else
    void setObject(QObject* object);
#endif
    QObject *object() const;

    bool isFinal() const;
    ViewType viewType() const;
    SubmitPolicy submitPolicy() const;

    QString propertyFilter() const;
    QString classFilter() const;

    bool notificationsEnabled() const;

public Q_SLOTS:
    void setFinal(bool on);
    void setViewType(ViewType type);
    void setSubmitPolicy(SubmitPolicy policy);
    void setPropertyFilter(const QString& pattern);
    void setClassFilter(const QString& pattern);
    void enableNotifications(bool on);

    void submit();
    void revert();
    void refresh();

private Q_SLOTS:
    void slotValueChanged(QtProperty *, const QVariant &);
    void objectDestroyed(QObject *object);

Q_SIGNALS:
    void propertyFilterChanged(const QString&);
    void classFilterChanged(const QString&);
    void propertyChanged(const QString& propertyName, const QVariant& value);

protected:
    void paintEvent(QPaintEvent* event);

private:
    static bool isQObject(const QMetaObject* metaObject);
    void setupObject(QObject* object);
    void setupGadget(void* gadget, const QMetaObject* metaObject);

private:
    QScopedPointer<class QtPropertyWidgetPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QtPropertyWidget)
    Q_DISABLE_COPY(QtPropertyWidget)
};



#if QT_VERSION >= 0x040400
QT_END_NAMESPACE
#endif

