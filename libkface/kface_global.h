#ifndef KFACE_GLOBAL_H
#define KFACE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(KFACE_LIBRARY)
#  define KFACESHARED_EXPORT Q_DECL_EXPORT
#else
#  define KFACESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // KFACE_GLOBAL_H
