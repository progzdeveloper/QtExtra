#ifndef QTSQLWIDGETS_GLOBAL_H
#define QTSQLWIDGETS_GLOBAL_H

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


#if defined(QTSQLWIDGETS_NODLL)
#    undef QTSQLWIDGETS_MAKEDLL
#    undef QTSQLWIDGETS_DLL
#elif defined(QTSQLWIDGETS_MAKEDLL)  /* create a DLL library */
#    if defined(QTSQLWIDGETS_DLL)
#      undef QTSQLWIDGETS_DLL
#    endif
#    if defined(QTSQLWIDGETS_BUILD_LIB)
#      define QTSQLWIDGETS_EXPORT Q_DECL_EXPORT
#    else
#      define QTSQLWIDGETS_EXPORT Q_DECL_IMPORT
#    endif
#elif defined(QTSQLWIDGETS_DLL)       /* use a DLL library */
#    define QTSQLWIDGETS_EXPORT Q_DECL_IMPORT
#endif

#endif // QTSQLWIDGETS_GLOBAL_H
