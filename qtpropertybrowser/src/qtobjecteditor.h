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

#ifndef QT_METAINFO_TR
#define QT_METAINFO_TR(_ClassName) static const char * _ClassName##MetaInfoTr[] =
#endif

#ifndef QT_TR_META
#define QT_TR_META(_ClassName, _Text) QT_TRANSLATE_NOOP(#_ClassName, _Text)
#endif

class QtProperty;

class QT_QTPROPERTYBROWSER_EXPORT QtObjectEditor :
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

    QtObjectEditor(QWidget *parent = 0);
    ~QtObjectEditor();

    void setObject(QObject *object);
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

    void filterProperties(const QRegExp& rx);
    void filterClasses(const QRegExp& rx);
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

private:
    class QtObjectEditorPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QtObjectEditor)
    Q_DISABLE_COPY(QtObjectEditor)
};

#if QT_VERSION >= 0x040400
QT_END_NAMESPACE
#endif

