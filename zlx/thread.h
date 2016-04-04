#ifndef _ZLX_THREAD_H
#define _ZLX_THREAD_H

#include "base.h"

typedef enum zlx_thstat_enum zlx_thstat_t;
typedef uintptr_t zlx_tid_t;
typedef uint_fast8_t (ZLX_CALL * zlx_thread_func_t) (void * arg);
typedef struct zlx_mutex_s zlx_mutex_t;
typedef struct zlx_cond_s zlx_cond_t;

typedef struct zlx_thread_xfc_s zlx_thread_xfc_t;
typedef struct zlx_mutex_xfc_s zlx_mutex_xfc_t;
typedef struct zlx_cond_xfc_s zlx_cond_xfc_t;
typedef struct zlx_mth_xfc_s zlx_mth_xfc_t;

enum zlx_thstat_enum
{
    ZLX_THSTAT_OK = 0,
    ZLX_THSTAT_FAILED, // generic "failed" status
    ZLX_THSTAT_NO_SUP, // feature not supported
};

struct zlx_thread_xfc_s
{
    /*  create  */
    /**
     *  Creates a thread.
     *  @param tid_p [out]
     *      receives the thread ID
     *  @param func [in]
     *      function to run in the created thread
     *  @param arg [in]
     *      argument for the function
     *  @retval 0   all ok
     *  @retval 1   failed
     */
    unsigned int (ZLX_CALL * create)
        (
            zlx_tid_t * tid_p,
            zlx_thread_func_t func,
            void * arg
        );

    /*  join  */
    /**
     *  Waits for the given thread to end.
     *  @param tid [in]
     *      thread to wait for
     *  @param ret_val_p [out, opt]
     *      receives the return value of the thread function
     */
    unsigned int (ZLX_CALL * join)
        (
            zlx_tid_t tid,
            uint8_t * ret_val_p
        );
};

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
    zlx_thstat_t (ZLX_CALL * init) (zlx_cond_t * cond_p);
    void (ZLX_CALL * finish) (zlx_cond_t * cond_p);
    void (ZLX_CALL * signal) (zlx_cond_t * cond_p);
    void (ZLX_CALL * wait) (zlx_cond_t * cond_p, zlx_mutex_t * mutex_p);
    size_t size;
};

struct zlx_mth_xfc_s
{
    zlx_thread_xfc_t thread;
    zlx_mutex_xfc_t mutex;
    zlx_cond_xfc_t cond;
};

ZLX_API unsigned int ZLX_CALL zlx_nosup_thread_create
    (
        zlx_tid_t * tid_p,
        zlx_thread_func_t func,
        void * arg
    );

ZLX_API unsigned int ZLX_CALL zlx_nosup_thread_join
    (
        zlx_tid_t tid,
        uint8_t * ret_val_p
    );

ZLX_API void ZLX_CALL zlx_nop_mutex_op (zlx_mutex_t * mutex_p);
ZLX_API zlx_thstat_t ZLX_CALL zlx_nosup_cond_init (zlx_cond_t * cond_p);
ZLX_API void ZLX_CALL zlx_nop_cond_op (zlx_cond_t * cond_p);
ZLX_API void ZLX_CALL zlx_nop_cond_wait
    (
        zlx_cond_t * cond_p,
        zlx_mutex_t * mutex_p
    );

ZLX_API zlx_mth_xfc_t zlx_nosup_mth_xfc;

#endif /* _ZLX_THREAD_H */

