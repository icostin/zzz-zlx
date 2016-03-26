#include "zlx/stdarray.h"
#include "zlx/writer.h"

/* zlx_nop_write ************************************************************/
ZLX_API ptrdiff_t ZLX_CALL zlx_nop_write
(
    void * ZLX_RESTRICT obj,
    uint8_t const * ZLX_RESTRICT data,
    size_t size
)
{
    (void) obj; (void) data; (void) size;
    return size;
}


/* zlx_sbw_init *************************************************************/
ZLX_API zlx_sbw_t * ZLX_CALL zlx_sbw_init
(
    zlx_sbw_t * ZLX_RESTRICT sbw,
    uint8_t * ZLX_RESTRICT data,
    size_t limit
)
{
    sbw->data = data;
    sbw->limit = limit;
    sbw->size = 0;
    return sbw;
}

/* zlx_sbw_write ************************************************************/
ZLX_API ptrdiff_t ZLX_CALL zlx_sbw_write
(
    void * ZLX_RESTRICT sbw,
    uint8_t const * ZLX_RESTRICT data,
    size_t len
)
{
    zlx_sbw_t * ZLX_RESTRICT s = sbw;
    if (s->size + len < len) return -1; // int overflow
    if (s->size < s->limit)
    {
        size_t left = s->limit - s->size;
        if (left > len) left = len;
        zlx_u8a_copy(s->data + s->size, data, left);
    }
    s->size += len;
    return len;
}

