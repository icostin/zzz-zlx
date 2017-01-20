#include "zlx/elal.h"

/* zlx_elal_init ************************************************************/
ZLX_API unsigned int ZLX_CALL zlx_elal_init
(
    zlx_elal_t * restrict ea,
    zlx_ma_t * restrict ma,
    zlx_mutex_xfc_t * restrict mutex_xfc,
    zlx_mutex_t * mutex,
    size_t elem_size,
    uint32_t max_chain_len
)
{
    if (elem_size < sizeof(void *)) elem_size = sizeof(void *);
    if (!mutex_xfc) mutex_xfc = &zlx_nosup_mth_xfc.mutex;
    if (mutex_xfc->size && !mutex)
    {
        mutex = zlx_alloc(ma, mutex_xfc->size, "elal mutex");
        if (!mutex) return 1;
        mutex_xfc->init(mutex);
        ea->mutex_allocated = 1;
    }
    else ea->mutex_allocated = 0;
    ea->mutex = mutex;
    ea->mutex_xfc = mutex_xfc;
    ea->ma = ma;
    ea->elem_size = elem_size;
    ea->chain = NULL;
    ea->chain_len = 0;
    ea->max_chain_len = max_chain_len;
    return 0;
}

/* zlx_elal_finish **********************************************************/
ZLX_API void ZLX_CALL zlx_elal_finish
(
    zlx_elal_t * restrict ea
)
{
    void * * cp;
    void * c;

    if (ea->mutex_allocated) ea->mutex_xfc->finish(ea->mutex);
    c = ea->chain;
    while (c)
    {
        cp = c;
        c = *cp;
        zlx_free(ea->ma, cp, ea->elem_size);
    }
}

/* zlxi_elal_alloc **********************************************************/
ZLX_API void * ZLX_CALL zlxi_elal_alloc
(
    zlx_elal_t * restrict ea
)
{
    zlx_mutex_xfc_t * restrict mx = ea->mutex_xfc;
    void * * e;
    mx->lock(ea->mutex);
    if (ea->chain_len)
    {
        ea->chain_len--;
        e = ea->chain;
        ea->chain = *e;
        mx->unlock(ea->mutex);
        return e;
    }
    mx->unlock(ea->mutex);
    return zlx_alloc(ea->ma, ea->elem_size, "elal");
}

/* zlxi_elal_free ***********************************************************/
ZLX_API void ZLX_CALL zlxi_elal_free
(
    zlx_elal_t * restrict ea,
    void * elem
)
{
    zlx_mutex_xfc_t * restrict mx = ea->mutex_xfc;
    mx->lock(ea->mutex);
    if (ea->chain_len < ea->max_chain_len)
    {
        void * * e = elem;
        ea->chain_len++;
        *e = ea->chain;
        ea->chain = e;
        mx->unlock(ea->mutex);
    }
    else
    {
        mx->unlock(ea->mutex);
        zlx_free(ea->ma, elem, ea->elem_size);
    }
}

