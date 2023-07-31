#pragma once
#include <QtCore/QtGlobal>
#include <QtCore/QScopedPointer>

#ifndef Q_NO_IMPL
# define Q_NO_IMPL(x) qWarning("(%s:%i) %s:%s is not yet implemented", __FILE__, __LINE__, Q_FUNC_INFO, #x);
#endif

#  if defined(QTCOREEXTRA_NODLL)
#    undef QTCOREEXTRA_MAKEDLL
#    undef QTCOREEXTRA_DLL
#  elif defined(QTCOREEXTRA_MAKEDLL)  /* create a DLL library */
#    if defined(QTCOREEXTRA_DLL)
#      undef QTCOREEXTRA_DLL
#    endif
#    if defined(QTCOREEXTRA_BUILD_LIB)
#      define QTCOREEXTRA_EXPORT Q_DECL_EXPORT
#    else
#      define QTCOREEXTRA_EXPORT Q_DECL_IMPORT
#    endif
#  elif defined(QTCOREEXTRA_DLL)       /* use a DLL library */
#    define QTCOREEXTRA_EXPORT Q_DECL_IMPORT
#  endif



