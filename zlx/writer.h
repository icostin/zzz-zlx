#ifndef _ZLX_WRITER_H
#define _ZLX_WRITER_H

#include "base.h"

/* zlx_writer_t *************************************************************/
/**
 *  Generic simple writer function.
 *  @param obj [in, out]
 *      context for the writer
 *  @param data [in]
 *      pointer to byte array to write
 *  @param size [in]
 *      size of byte array; should always be strictly greater than 0
 *  @returns 
 *      number of bytes written; a value other than @a size indicates some
 *      error and implementations are encouraged to store the error in the
 *      structure pointed by @a ctx
 */
typedef ptrdiff_t (ZLX_CALL * zlx_write_func_t) 
    (
        void * ZLX_RESTRICT obj,
        uint8_t const * ZLX_RESTRICT data,
        size_t size
    );

/* zlx_sbw_t ****************************************************************/
/**
 *  Single-buffer writer context structure.
 */
typedef struct zlx_sbw_s zlx_sbw_t;
struct zlx_sbw_s
{
    uint8_t * ZLX_RESTRICT data; /**< pointer to buffer. */
    size_t limit; /**< number of bytes available at @a data. */
    size_t size; /**< how much was written to the buffer; this will accurately
                   count the bytes written although anything beyond @a limit
                   will not be stored */
};

/* zlx_sbw_init *************************************************************/
/**
 *  Initializes the context structure for sbw writer.
 *  See zlx_sbw_t and zlx_sbw_write().
 *  @returns @a sbw
 */
ZLX_API zlx_sbw_t * ZLX_CALL zlx_sbw_init
(
    zlx_sbw_t * ZLX_RESTRICT sbw,
    uint8_t * ZLX_RESTRICT data,
    size_t limit
);

/* zlx_sbw_write ************************************************************/
/**
 *  Single-buffer writer processor function.
 *  See #zlx_sbw_t and #zlx_write_func_t.
 *  @returns the input @a len except when the updating the counter causes
 *      integer overflow in which case the function returns -1.
 */
ZLX_API ptrdiff_t ZLX_CALL zlx_sbw_write
(
    void * ZLX_RESTRICT sbw,
    uint8_t const * ZLX_RESTRICT data,
    size_t len
);

/* zlx_nop_write ************************************************************/
/**
 *  'nop' write.
 *  @returns input argument @a size
 */
ZLX_API ptrdiff_t ZLX_CALL zlx_nop_write
(
    void * ZLX_RESTRICT obj,
    uint8_t const * ZLX_RESTRICT data,
    size_t size
);

#endif
