#ifndef _ZLX_H
#define _ZLX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "zlx/base.h"

/* zlx_lib_name *************************************************************/
/**
 *  static C-string that identifies the flavor of zlx library.
 *  The string is in the format: 
 *      "zlx-<static|dynamic>-<release|checked|debug>-<arch>-<kernel>-<os/environment>-<compiler_id>"
 */
extern ZLX_API char const * const zlx_lib_name;

#include "zlx/stdarray.h"

#ifdef __cplusplus
}
#endif

#endif /* _ZLX_H */

