#ifndef _ZLX_ASSERT_H
#define _ZLX_ASSERT_H

#include "base.h"
#include "log.h"

/** @defgroup assert Assertion
 *  Support for assertions.
 *
 *  The library is designed to support 3 major build configurations:
 *  release, debug, checked.
 *  
 *  Assertions are computed on debug or checked builds (_DEBUG or _CHECKED 
 *  defined as non-zero).
 *  @{
 */

/* zlx_abort ****************************************************************/
/**
 *  Function pointer to be called when aborting.
 *  The library, being freestanding, provides this as a NULL pointer which 
 *  should be enough to trigger an abort on most environments. However, the
 *  initialization code of a program can set this to something else.
 */
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
/*  ZLX_ASSERT  */
/**
 *  Evaluates the given expression and if it is not true (non-zero) it calls
 *  zlx_abort().
 *  @note the expression is evaluated only on debug (_DEBUG non-zero) or 
 *  checked (_CHECKED non-zero) builds.
 */
#define ZLX_ASSERT(_truth) ((void) 0)
#endif

/** @} */

#endif /* _ZLX_ASSERT_H */

