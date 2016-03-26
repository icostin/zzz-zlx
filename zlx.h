#ifndef _ZLX_H
#define _ZLX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "zlx/arch.h"
#include "zlx/base.h"

/* zlx_lib_name *************************************************************/
/**
 *  static C-string that identifies the flavour of zlx library.
 *  The string is in the format: 
 *      "zlx-<static|dynamic>-<release|checked|debug>-<arch>-<kernel>-<os/environment>-<compiler_id>"
 */
extern ZLX_API char const * const zlx_lib_name;

#include "zlx/dlist.h"
#include "zlx/stdarray.h"
#include "zlx/unicode.h"
#include "zlx/memalloc.h"
#include "zlx/alloctrk.h"
#include "zlx/writer.h"
#include "zlx/log.h"
#include "zlx/clconv.h"
#include "zlx/fmt.h"
#include "zlx/file.h"
#include "zlx/assert.h"

#ifdef __cplusplus
}
#endif

#endif /* _ZLX_H */

