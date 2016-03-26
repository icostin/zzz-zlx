#include "zlx/thread.h"

/* zlx_nop_mutex_op *********************************************************/
ZLX_API void ZLX_CALL zlx_nop_mutex_op (zlx_mutex_t * mutex_p)
{
    (void) mutex_p;
}

/* zlx_nop_mutex_xfc ********************************************************/
ZLX_API zlx_mutex_xfc_t zlx_nop_mutex_xfc =
{
    zlx_nop_mutex_op,
    zlx_nop_mutex_op,
    zlx_nop_mutex_op,
    zlx_nop_mutex_op,
    1
};
