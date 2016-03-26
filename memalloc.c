#include "zlx.h"

ZLX_API void * ZLX_CALL zlx_ma_nop_realloc
(
    void * old_ptr,
    size_t old_size,
    size_t new_size,
    zlx_ma_t * ZLX_RESTRICT ma
)
{
    (void) old_ptr; (void) old_size; (void) new_size; (void) ma;
    return NULL;
}

ZLX_API void ZLX_CALL zlx_ma_nop_info_set
(
    zlx_ma_t * ZLX_RESTRICT ma,
    void * ptr,
    char const * src,
    unsigned int line,
    char const * func,
    char const * info
)
{
    (void) ptr; (void) src; (void) line; (void) func; (void) info; (void) ma;
}


ZLX_API void ZLX_CALL zlx_ma_nop_check
(
    zlx_ma_t * ZLX_RESTRICT ma,
    void * ptr,
    size_t size,
    char const * src,
    unsigned int line,
    char const * func
)
{
    (void) ma; (void) ptr; (void) size; (void) src; (void) line; (void) func;
}


