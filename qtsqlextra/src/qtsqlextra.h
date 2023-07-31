#ifndef QTSQLEXTRA_GLOBAL_H
#define QTSQLEXTRA_GLOBAL_H

#include <QtCore/QtGlobal>
#include <QtCore/QScopedPointer>

#ifndef Q_NO_IMPL
# define Q_NO_IMPL(x) qWarning("(%s:%d) %s:%s is not yet implemented", __FILE__, __LINE__, Q_FUNC_INFO, #x);
#endif

#if defined(QTSQLEXTRA_NODLL)
#    undef QTSQLEXTRA_MAKEDLL
#    undef QTSQLEXTRA_DLL
#elif defined(QTSQLEXTRA_MAKEDLL)  /* create a DLL library */
#    if defined(QTSQLEXTRA_DLL)
#      undef QTSQLEXTRA_DLL
#    endif
#    if defined(QTSQLEXTRA_BUILD_LIB)
#      define QTSQLEXTRA_EXPORT Q_DECL_EXPORT
#    else
#      define QTSQLEXTRA_EXPORT Q_DECL_IMPORT
#    endif
#elif defined(QTSQLEXTRA_DLL)       /* use a DLL library */
#    define QTSQLEXTRA_EXPORT Q_DECL_IMPORT
#endif

#endif // QTSQLEXTRA_GLOBAL_H
