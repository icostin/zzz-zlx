#ifndef _ZLX_H
#define _ZLX_H

/** @mainpage Zalmoxis
 *
 *  This is a freestanding library containing various utility functions and
 *  macros:
 *      - doubly-linked lists
 *      - macro-templetized arrays
 *      - unicode conversion functions
 *      - memory allocation interface
 *      - memory allocation tracker
 *      - logging interface
 *      - string formatting (printf-like but with different escapes)
 *      - basic multithreading interface
 *      - lookaside list element allocator
 *      - etc.
 *
 *  The library does not depend on any external function and only uses standard
 *  headers documented in the C99 standard as available in freestanding
 *  environments.
 *
 *  @section License
 *
 *  Copyright (c) 2016, Costin Ionescu <costin.ionescu@gmail.com>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *  
 *  Note: ISC license (functionally equivalent to simplified BSD and MIT/Expat)
 *
 */
#ifdef __cplusplus
extern "C" {
#endif

#include "zlx/arch.h"
#include "zlx/base.h"

/** @defgroup misc Miscellaneous
 *  Freestanding utility library.
 *  @{
 */

/* zlx_lib_name *************************************************************/
/**
 *  static C-string that identifies the flavour of zlx library.
 *  The string is in the format: 
 *      "zlx-<static|dynamic>-<release|checked|debug>-<arch>-<kernel>-<os/environment>-<compiler_id>"
 */
extern ZLX_API char const * const zlx_lib_name;

/** @} */


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
#include "zlx/thread.h"
#include "zlx/elal.h"

#ifdef __cplusplus
}
#endif

#endif /* _ZLX_H */

