#ifndef _TYPES_H_
#define _TYPES_H_

#include <sys/types.h>

typedef int status_t;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 1
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // #ifndef M_PI

#ifndef NULL
#define NULL ((void*)0)
#endif // NULL

#define PTR_CAST(t, v) reinterpret_cast<t>(v)

#endif // #ifndef _TYPES_H_
