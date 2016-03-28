#include "zlx/thread.h"
#include "zlx/assert.h"

/* zlx_nosup_thread_xfc *****************************************************/
ZLX_API zlx_thread_xfc_t zlx_nosup_thread_xfc =
{
    zlx_nosup_thread_create,
    zlx_nosup_thread_join
};

/* zlx_nop_mutex_xfc ********************************************************/
ZLX_API zlx_mutex_xfc_t zlx_nop_mutex_xfc =
{
    zlx_nop_mutex_op,
    zlx_nop_mutex_op,
    zlx_nop_mutex_op,
    zlx_nop_mutex_op,
    1
};

/* zlx_nosup_cond_xfc *******************************************************/
ZLX_API zlx_cond_xfc_t zlx_nosup_cond_xfc =
{
    zlx_nosup_cond_init,
    zlx_nop_cond_op,
    zlx_nop_cond_op,
    zlx_nop_cond_wait,
    1
};

/* zlx_nosup_thread_create **************************************************/
ZLX_API unsigned int ZLX_CALL zlx_nosup_thread_create
    (
        zlx_tid_t * tid_p,
        zlx_thread_func_t func,
        void * arg
    )
{
    (void) tid_p, (void) func, (void) arg;
    return ZLX_THSTAT_NO_SUP;
}

/* zlx_nosup_thread_join ****************************************************/
ZLX_API unsigned int ZLX_CALL zlx_nosup_thread_join
    (
        zlx_tid_t tid,
        uint8_t * ret_val_p
    )
{
    (void) tid;
    (void) ret_val_p;
    return ZLX_THSTAT_NO_SUP;
}

/* zlx_nop_mutex_op *********************************************************/
ZLX_API void ZLX_CALL zlx_nop_mutex_op (zlx_mutex_t * mutex_p)
{
    (void) mutex_p;
}

/* zlx_nosup_cond_init ******************************************************/
ZLX_API zlx_thstat_t ZLX_CALL zlx_nosup_cond_init (zlx_cond_t * cond_p)
{
    (void) cond_p;
    return ZLX_THSTAT_NO_SUP;
}

/* zlx_nop_cond_op **********************************************************/
ZLX_API void ZLX_CALL zlx_nop_cond_op (zlx_cond_t * cond_p)
{
    (void) cond_p;
}

/* zlx_nop_cond_wait ********************************************************/
ZLX_API void ZLX_CALL zlx_nop_cond_wait
    (
        zlx_cond_t * cond_p,
        zlx_mutex_t * mutex_p
    )
{
    (void) cond_p, (void) mutex_p;
}

