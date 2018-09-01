#ifndef LIBSYREM_GLOBAL_H
#define LIBSYREM_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(SYREM_LIBRARY)
#  define LIB_SYREM_EXPORT Q_DECL_EXPORT
#else
#  define LIB_SYREM_EXPORT Q_DECL_IMPORT
#endif

#endif // LIBSYREM_GLOBAL_H
