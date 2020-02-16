#ifndef QTWIDGETSEXTRA_H
#define QTWIDGETSEXTRA_H

#include <QtCore/QtGlobal>
#include <QtCore/QScopedPointer>

#ifndef Q_NO_IMPL
# define Q_NO_IMPL(x) qWarning("(%s:%d) %s:%s is not yet implemented", __FILE__, __LINE__, Q_FUNC_INFO, #x);
#endif

#ifndef QT_PIMPL
#define QT_PIMPL(_Class)                          \
    QScopedPointer<class _Class##Private> d_ptr;  \
    Q_DECLARE_PRIVATE(_Class)
#endif


#if defined(QTWIDGETSEXTRA_NODLL)
#    undef QTWIDGETSEXTRA_MAKEDLL
#    undef QTWIDGETSEXTRA_DLL
#elif defined(QTWIDGETSEXTRA_MAKEDLL)  /* create a DLL library */
#    if defined(QTWIDGETSEXTRA_DLL)
#      undef QTWIDGETSEXTRA_DLL
#    endif
#    if defined(QTWIDGETSEXTRA_BUILD_LIB)
#      define QTWIDGETSEXTRA_EXPORT Q_DECL_EXPORT
#    else
#      define QTWIDGETSEXTRA_EXPORT Q_DECL_IMPORT
#    endif
#elif defined(QTWIDGETSEXTRA_DLL)       /* use a DLL library */
#    define QTWIDGETSEXTRA_EXPORT Q_DECL_IMPORT
#endif

#endif


