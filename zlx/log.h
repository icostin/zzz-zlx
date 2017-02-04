#ifndef _ZLX_LOG_H
#define _ZLX_LOG_H

#include <stdarg.h>

#include "base.h"
#include "writer.h"

/** @defgroup log Logging
 *  @{ */

/** No logging */
#define ZLX_LL_NONE 0

/** Logging only fatal messages */
#define ZLX_LL_FATAL 1

/** Logging only error messages */
#define ZLX_LL_ERROR 2

/** Logging only warning messages */
#define ZLX_LL_WARNING 3

/** Logging only info messages */
#define ZLX_LL_INFO 4

/** Logging only debug messages */
#define ZLX_LL_DEBUG 5


/*  zlx_log_t  */
/**
 *  Base logger instance.
 */
typedef struct zlx_log_s zlx_log_t;
struct zlx_log_s
{
    /** Writer function to send data to the log */
    zlx_write_func_t write;
    
    /** Context for the writer */
    void * obj;

    /** Current level for logging. Only messages at a level lower or equal
     *  this value are written to the log. */
    unsigned int level;
};

/* zlx_vlog *****************************************************************/
/**
 *  Logs a message.
 *  @param log [in, out]
 *      logger instance
 *  @param level [in]
 *      importance level for the message; see zlx_log_t#level.
 *  @param fmt [in]
 *      format string; see zlx_vfmt()
 */
ZLX_API int ZLX_CALL zlx_vlog
(
    zlx_log_t * log,
    unsigned int level,
    char const * fmt,
    va_list va
);

/* zlx_log ******************************************************************/
/**
 *  Logs a message.
 *  @param log [in, out]
 *      logger instance
 *  @param level [in]
 *      importance level for the message
 *  @param fmt [in]
 *      format string; see zlx_vfmt()
 *  This function unconditionally formats and writes the message to the log.
 *  Normally, users do not need to call this function directly, instead they
 *  should use one of the macros: #ZLX_LF, #ZLX_LE, #ZLX_LW, #ZLX_LI, #ZLX_LD
 *  or #ZLX_LOG.
 */
ZLX_API int zlx_log
(
    zlx_log_t * log,
    unsigned int level,
    char const * fmt,
    ...
);

/*  ZLX_LOG  */
/**
 *  Evaluates whether to send the message to the log or not and sends it
 *  if the message log has the same or higher importance than the logging 
 *  level.
 *  @param _log [in, out]
 *      a pointer to a zlx_log_t instance
 *  @param _level [in]
 *      level for the message
 */
#define ZLX_LOG(_log, _level, ...) \
    ((void) ((_level) <= (_log)->level && zlx_log((_log), (_level), __VA_ARGS__)))

/** Logs a fault-level message. */
#define ZLX_LF(_log, ...) (ZLX_LOG((_log), ZLX_LL_FATAL, __VA_ARGS__))

/** Logs an error-level message. */
#define ZLX_LE(_log, ...) (ZLX_LOG((_log), ZLX_LL_ERROR, __VA_ARGS__))

/** Logs a warning-level message */
#define ZLX_LW(_log, ...) (ZLX_LOG((_log), ZLX_LL_WARNING, __VA_ARGS__))

/** Logs an informational-level message */
#define ZLX_LI(_log, ...) (ZLX_LOG((_log), ZLX_LL_INFO, __VA_ARGS__))

#if _DEBUG
#define ZLX_LD(_log, ...) (ZLX_LOG((_log), ZLX_LL_DEBUG, __VA_ARGS__))
#else
/** Logs a debug-level message.
 *  This gets evaluated only for debug builds */
#define ZLX_LD(_log, ...) ((void) 0)
#endif


#define ZLX_LDM(_log, _fmt, ...) \
    (ZLX_LD((_log), "$s:$i@$s(): " _fmt "\n", \
            __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__))

#define ZLX_LDMX(_log, ...) ((void) 0)

/* zlx_default_log **********************************************************/
/**
 *  Global logger instance.
 *  This is pre-initialized with a "null" logger instance, but can be replaced
 *  by the app.
 *  Some of the components in zlx make use of this instance to log their 
 *  activity.
 */
extern ZLX_API zlx_log_t * zlx_default_log;

/** @} */

#endif /* _ZLX_LOG_H */

