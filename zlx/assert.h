#ifndef _ZLX_ASSERT_H
#define _ZLX_ASSERT_H

#include "base.h"
#include "log.h"

extern ZLX_API void (* zlx_abort)();

#if _DEBUG
#define ZLX_ASSERT(_truth) if ((_truth)) ; else do { \
    ZLX_LF(zlx_default_log, "*** BUG *** $s:$i:$s(): ASSERTION FAILED: $s\n", __FILE__, __LINE__, __FUNCTION__, #_truth); \
    zlx_abort(); } while (0)
#elif _CHECKED
#define ZLX_ASSERT(_truth) if ((_truth)) ; else do { \
    ZLX_LF(zlx_default_log, "*** BUG *** $s:$i: ASSERTION FAILED!\n", __FILE__, __LINE__); \
    zlx_abort(); } while (0)
#else
#define ZLX_ASSERT(_truth) ((void) 0)
#endif

#endif /* _ZLX_ASSERT_H */

