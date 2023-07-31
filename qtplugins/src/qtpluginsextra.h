#pragma once
#include <QtCore/QtGlobal>

#ifndef Q_NO_IMPL
# define Q_NO_IMPL(x) qWarning("(%s:%d) %s:%s is not yet implemented", __FILE__, __LINE__, Q_FUNC_INFO, #x);
#endif

#  if defined(QTPLUGINS_NODLL)
#    undef QTPLUGINS_MAKEDLL
#    undef QTPLUGINS_DLL
#  elif defined(QTPLUGINS_MAKEDLL)  /* create a DLL library */
#    if defined(QTPLUGINS_DLL)
#      undef QTPLUGINS_DLL
#    endif
#    if defined(QTPLUGINS_BUILD_LIB)
#      define QTPLUGINS_EXPORT Q_DECL_EXPORT
#    else
#      define QTPLUGINS_EXPORT Q_DECL_IMPORT
#    endif
#  elif defined(QTPLUGINS_DLL)       /* use a DLL library */
#    define QTPLUGINS_EXPORT Q_DECL_IMPORT
#  endif



