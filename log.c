#include "zlx/log.h"
#include "zlx/unicode.h"
#include "zlx/fmt.h"

zlx_log_t null_log =
{
    zlx_nop_write,
    NULL,
    ZLX_LL_NONE
};

ZLX_API zlx_log_t * zlx_default_log = &null_log;

/* zlx_vlog *****************************************************************/
ZLX_API int ZLX_CALL zlx_vlog 
(
    zlx_log_t * log,
    unsigned int level,
    char const * fmt,
    va_list va
)
{
    (void) level;
    return zlx_vfmt(log->write, log->obj, zlx_utf8_term_width, NULL, fmt, va);
}

/* zlx_log ******************************************************************/
ZLX_API int zlx_log 
(
    zlx_log_t * log,
    unsigned int level,
    char const * fmt,
    ...
)
{
    va_list va;
    unsigned int rc;

    va_start(va, fmt);
    rc = zlx_vlog(log, level, fmt, va);
    va_end(va);
    return rc;
}

