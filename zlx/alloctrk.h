#ifndef _ZLX_ALLOC_TRACKER_H
#define _ZLX_ALLOC_TRACKER_H

#include "memalloc.h"
#include "log.h"

/**
 *  Creates a new alloc tracker instance.
 *  The pointer returned by this function can be used as @a context arg for
 *  zlx_alloctrk_realloc().
 *
 *  The allocation tracker will maintain a list of all allocated blocks to
 *  detect leaks. When allocating a block a larger size is used in order to
 *  maintain block information and markers for detecting out of bounds writes.
 */
ZLX_API zlx_ma_t * ZLX_CALL zlx_alloctrk_create
(
    zlx_ma_t * ma,
    zlx_log_t * log
);

/* zlx_alloctrk_destroy *****************************************************/
/**
 *  Destroys the tracker instance.
 *  @returns backing allocator for the tracker that was passed during
 *      zlx_alloctrk_realloc().
 */
ZLX_API zlx_ma_t * ZLX_CALL zlx_alloctrk_destroy
(
    zlx_ma_t * ma
);

ZLX_API void ZLX_CALL zlx_alloctrk_dump
(
    zlx_ma_t * ma
);

#endif

