#ifndef _ZLX_ELEMENT_LOOKASIDE_LIST_H
#define _ZLX_ELEMENT_LOOKASIDE_LIST_H

/** @addtogroup elal Element lookaside list allocator
 *  @{ */
/* implements a look-aside list for element allocation */

#include "base.h"
#include "memalloc.h"
#include "thread.h"

/*, (_elem)  zlx_elal_t  */
/**
 *  Element look-aside list instance structure.
 */
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
 *  @param mutex [in, opt]
 *      mutex to be used to lock/unlock around performing the operations;
 *      if this is NULL, a new mutex is allocated (using the given memory
 *      allocator) and initialized
 *  @param elem_size [in]
 *      element size
 *  @param max_chain_len [in]
 *      max number of items to store in the lookaside list
 *  @retval 0 init ok
 *  @retval 1 failed to allocate mutex
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

/* zlx_elal_finish **********************************************************/
/**
 *  Frees all resources held by the lookaside list.
 */
ZLX_API void ZLX_CALL zlx_elal_finish
(
    zlx_elal_t * ZLX_RESTRICT ea
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
#define zlx_elal_free(_ea, _elem) (zlxd_elal_free((_ea), (_elem), __FILE__, __LINE__, __FUNCTION__))
#elif _CHECKED
#define zlx_elal_alloc(_ea, _info) (zlxd_elal_alloc((_ea), NULL, 0, NULL, NULL))
#define zlx_elal_free(_ea, _elem) (zlxd_elal_free((_ea), (_elem), NULL, 0, NULL))
#else
/* zlx_elal_alloc ***********************************************************/
/**
 *  Allocates one element.
 *  @param ea [in]
 *      instance of the initialized lookaside list
 *  @returns address of the element or NULL on error
 */
#define zlx_elal_alloc(_ea, _info) (zlxi_elal_alloc((_ea)))

/* zlx_elal_free ************************************************************/
/**
 *  Frees the element.
 *  @param ea [in]
 *      instance of the initialized lookaside list
 *  @param elem [in]
 *      element to free
 */
#define zlx_elal_free(_ea, _elem) (zlxi_elal_free((_ea), (_elem)))

#endif

/** @} */

#endif /* _ZLX_ELEMENT_LOOKASIDE_LIST_H */

