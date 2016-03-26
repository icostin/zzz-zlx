#ifndef _ZLX_LOG_H
#define _ZLX_LOG_H

#include <stdarg.h>

#include "base.h"
#include "writer.h"

#define ZLX_LL_NONE 0
#define ZLX_LL_FATAL 1
#define ZLX_LL_ERROR 2
#define ZLX_LL_WARNING 3
#define ZLX_LL_INFO 4
#define ZLX_LL_DEBUG 5

typedef struct zlx_log_s zlx_log_t;
struct zlx_log_s
{
    zlx_write_func_t write;
    void * obj;
    unsigned int level;
};

ZLX_API int ZLX_CALL zlx_vlog 
(
    zlx_log_t * log,
    unsigned int level,
    char const * fmt,
    va_list va
);

ZLX_API int zlx_log 
(
    zlx_log_t * log,
    unsigned int level,
    char const * fmt,
    ...
);

#define ZLX_LOG(_log, _level, ...) \
    ((void) ((_level) <= (_log)->level && zlx_log((_log), (_level), __VA_ARGS__)))

#define ZLX_LF(_log, ...) (ZLX_LOG((_log), ZLX_LL_FATAL, __VA_ARGS__))
#define ZLX_LE(_log, ...) (ZLX_LOG((_log), ZLX_LL_ERROR, __VA_ARGS__))
#define ZLX_LW(_log, ...) (ZLX_LOG((_log), ZLX_LL_WARNING, __VA_ARGS__))
#define ZLX_LI(_log, ...) (ZLX_LOG((_log), ZLX_LL_INFO, __VA_ARGS__))

#if _DEBUG
#define ZLX_LD(_log, ...) (ZLX_LOG((_log), ZLX_LL_DEBUG, __VA_ARGS__))
#else
#define ZLX_LD(_log, ...) ((void) 0)
#endif

extern ZLX_API zlx_log_t * zlx_default_log;

#endif /* _ZLX_LOG_H */

