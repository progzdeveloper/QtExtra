#pragma once
#include <QObject>


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
#  define QT_QTPROPERTYBROWSER_EXPORT
#endif



//#define QT_META_TR(ClassName) static const char *ClassName##MetaInfoTr[] =

class QtPropertyProxyObjectPrivate;

class QT_QTPROPERTYBROWSER_EXPORT QtPropertyProxyObject :
	public QObject
{
	Q_OBJECT

public:
    explicit QtPropertyProxyObject(QObject *parent = Q_NULLPTR);
    virtual ~QtPropertyProxyObject();

public slots:
	virtual void apply();
	virtual void restoreDefaults();
};



