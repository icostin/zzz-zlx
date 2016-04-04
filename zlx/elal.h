#ifndef _ZLX_ELEMENT_LOOKASIDE_LIST_H
#define _ZLX_ELEMENT_LOOKASIDE_LIST_H

/* implements a look-aside list for element allocation */

#include "base.h"
#include "memalloc.h"
#include "thread.h"

typedef struct zlx_elal_s zlx_elal_t;
struct zlx_elal_s
{
    void * * chain;
    zlx_mutex_t * mutex;
    zlx_ma_t * ma;
    zlx_mutex_xfc_t * mutex_xfc;
    size_t elem_size;
    uint32_t chain_len;
    uint32_t max_chain_len;
    uint8_t mutex_allocated;
};

/* zlx_elal_init ************************************************************/
/**
 *  Initializes an element lookaside list.
 *  @param ea [out]
 *      list to initialize
 *  @param ma [in]
 *      mem allocator
 *  @param mutex_xfc [in, opt]
 *      mutex interface; if NULL a dummy interface will be used
 */
ZLX_API unsigned int ZLX_CALL zlx_elal_init
(
    zlx_elal_t * ZLX_RESTRICT ea,
    zlx_ma_t * ZLX_RESTRICT ma,
    zlx_mutex_xfc_t * ZLX_RESTRICT mutex_xfc,
    zlx_mutex_t * mutex,
    size_t elem_size,
    uint32_t max_chain_len
);

ZLX_API void * ZLX_CALL zlxi_elal_alloc
(
    zlx_elal_t * ZLX_RESTRICT ea
);

ZLX_API void ZLX_CALL zlxi_elal_free
(
    zlx_elal_t * ZLX_RESTRICT ea,
    void * elem
);

#if _DEBUG || _CHECKED
ZLX_INLINE void * zlxd_elal_alloc
(
    zlx_elal_t * ZLX_RESTRICT ea,
    char const * src,
    unsigned int line,
    char const * func,
    char const * info
)
{
    void * e = zlxi_elal_alloc(ea);
    if (e) ea->ma->info_set(ea->ma, e, src, line, func, info);
    return e;
}

ZLX_INLINE void zlxd_elal_free
(
    zlx_elal_t * ZLX_RESTRICT ea,
    void * e,
    char const * src,
    unsigned int line,
    char const * func
)
{
    ea->ma->check(ea->ma, e, ea->elem_size, src, line, func);
    zlxi_elal_free(ea, e);
}

#endif


#if _DEBUG
#define zlx_elal_alloc(_ea, _info) (zlxd_elal_alloc((_ea), __FILE__, __LINE__, __FUNCTION__, (_info)))
#define zlx_elal_free(_ea) (zlxd_elal_free((_ea), __FILE__, __LINE__, __FUNCTION__))
#elif _CHECKED
#define zlx_elal_alloc(_ea, _info) (zlxd_elal_alloc((_ea), NULL, 0, NULL, NULL))
#define zlx_elal_free(_ea) (zlxd_elal_free((_ea), NULL, 0, NULL))
#else
#define zlx_elal_alloc(_ea, _info) (zlxi_elal_alloc((_ea)))
#define zlx_elal_free(_ea, _info) (zlxi_elal_free((_ea)))
#endif

#endif /* _ZLX_ELEMENT_LOOKASIDE_LIST_H */

