#ifndef _ZLX_THREAD_H
#define _ZLX_THREAD_H

#include "base.h"

typedef struct zlx_mutex_s zlx_mutex_t;
typedef struct zlx_cond_s zlx_cond_t;
typedef struct zlx_mutex_xfc_s zlx_mutex_xfc_t;

struct zlx_mutex_xfc_s
{
    void (ZLX_CALL * init) (zlx_mutex_t * mutex_p);
    void (ZLX_CALL * finish) (zlx_mutex_t * mutex_p);
    void (ZLX_CALL * lock) (zlx_mutex_t * mutex_p);
    void (ZLX_CALL * unlock) (zlx_mutex_t * mutex_p);
    size_t size;
};

struct zlx_cond_xfc_s
{
    /*  init  */
    /**
     *  @retval 0 ok
     *  @retval 1 failed
     */
    unsigned int (ZLX_CALL * init) (zlx_cond_t * cond_p);
    void (ZLX_CALL * finish) (zlx_cond_t * cond_p);
    void (ZLX_CALL * signal) (zlx_cond_t * cond_p);
    void (ZLX_CALL * wait) (zlx_cond_t * cond_p, zlx_mutex_t * mutex_p);
    size_t size;
};

ZLX_API void ZLX_CALL zlx_nop_mutex_op (zlx_mutex_t * mutex_p);
ZLX_API zlx_mutex_xfc_t zlx_nop_mutex_xfc;

#endif /* _ZLX_THREAD_H */
