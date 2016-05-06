#ifndef _ZLX_THREAD_H
#define _ZLX_THREAD_H

#include "base.h"

/*  zlx_mth_status_t  */
/**
 *  Enum for thread create/join statuses.
 */

typedef enum zlx_thstat_enum zlx_mth_status_t;
/*  zlx_tid_t  */
/**
 *  Thread identifier.
 */
typedef uintptr_t zlx_tid_t;

/*  zlx_thread_func_t  */
/**
 *  Thread function pointer.
 */
typedef uint_fast8_t (ZLX_CALL * zlx_thread_func_t) (void * arg);

/*  zlx_mutex_t  */
/**
 *  Opaque mutex type.
 */
typedef struct zlx_mutex_s zlx_mutex_t;

/*  zlx_cond_t  */
/**
 *  Opaque condition variable.
 */
typedef struct zlx_cond_s zlx_cond_t;

/*  zlx_thread_xfc_t  */
/**
 *  Interface for thread operations.
 */
typedef struct zlx_thread_xfc_s zlx_thread_xfc_t;

/*  zlx_mutex_xfc_t  */
/**
 *  Interface for mutex operations.
 */
typedef struct zlx_mutex_xfc_s zlx_mutex_xfc_t;

/*  zlx_cond_xfc_t  */
/**
 *  Interface for condition variable operations.
 */
typedef struct zlx_cond_xfc_s zlx_cond_xfc_t;

/*  zlx_mth_xfc_t  */
/**
 *  Collection of all multithreading related interfaces.
 */
typedef struct zlx_mth_xfc_s zlx_mth_xfc_t;

enum zlx_thstat_enum
{
    /** Ok */
    ZLX_MTH_OK = 0,

    /** generic "failed" status */
    ZLX_MTH_FAILED,

    /** not enough memory to complete the thread operation */
    ZLX_MTH_NO_MEM,

    /** not enough resources */
    ZLX_MTH_NO_RES,

    /** deadlock */
    ZLX_MTH_DEADLOCK,

    /** thread already joining */
    ZLX_MTH_ALREADY_JOINING,

    /** no thread */
    ZLX_MTH_NO_THREAD,

    /** feature not supported */
    ZLX_MTH_NO_SUP,

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
    zlx_mth_status_t (ZLX_CALL * create)
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
    zlx_mth_status_t (ZLX_CALL * join)
        (
            zlx_tid_t tid,
            uint8_t * ret_val_p
        );
};

struct zlx_mutex_xfc_s
{
    /** Initializes a mutex. */
    void (ZLX_CALL * init) (zlx_mutex_t * mutex_p);

    /** Finishes a mutex. */
    void (ZLX_CALL * finish) (zlx_mutex_t * mutex_p);

    /** Locks a mutex. */
    void (ZLX_CALL * lock) (zlx_mutex_t * mutex_p);

    /** Unlocks a mutex. */
    void (ZLX_CALL * unlock) (zlx_mutex_t * mutex_p);

    /** Size of the mutex instance. */
    size_t size;
};

struct zlx_cond_xfc_s
{
    /** Initializes a condition variable.  */
    zlx_mth_status_t (ZLX_CALL * init) (zlx_cond_t * cond_p);

    /** Finishes a condition variable */
    void (ZLX_CALL * finish) (zlx_cond_t * cond_p);

    /** Signals a condition variable. This operation wakes one of the threads
     *  waiting on this condition variable */
    void (ZLX_CALL * signal) (zlx_cond_t * cond_p);

    /** Waits on a condition variable.
     *  The function blocks until another thread calls zlx_cond_xfc_t#signal
     *  on the same condition. If multiple threads are waiting on the same
     *  condition variable then only one thread is guaranteed to end its
     *  wait. */
    void (ZLX_CALL * wait) (zlx_cond_t * cond_p, zlx_mutex_t * mutex_p);

    /** Size of the condition variable instance. */
    size_t size;
};

struct zlx_mth_xfc_s
{
    /** Interface for thread operations. */
    zlx_thread_xfc_t thread;

    /** Interface for mutex operations. */
    zlx_mutex_xfc_t mutex;

    /** Interface for condition variable operations. */
    zlx_cond_xfc_t cond;
};

ZLX_API zlx_mth_status_t ZLX_CALL zlx_nosup_thread_create
    (
        zlx_tid_t * tid_p,
        zlx_thread_func_t func,
        void * arg
    );

ZLX_API zlx_mth_status_t ZLX_CALL zlx_nosup_thread_join
    (
        zlx_tid_t tid,
        uint8_t * ret_val_p
    );

ZLX_API void ZLX_CALL zlx_nop_mutex_op (zlx_mutex_t * mutex_p);
ZLX_API zlx_mth_status_t ZLX_CALL zlx_nosup_cond_init (zlx_cond_t * cond_p);
ZLX_API void ZLX_CALL zlx_nop_cond_op (zlx_cond_t * cond_p);
ZLX_API void ZLX_CALL zlx_nop_cond_wait
    (
        zlx_cond_t * cond_p,
        zlx_mutex_t * mutex_p
    );

/** Dummy interface for multithreading that does not offer support for any
 *  real operations.
 *
 *  Functions that return a status code are set to return #ZLX_MTH_NO_SUP.
 *  
 *  This interface can be useful when a component requires a mutex interface
 *  but the caller does not intend to use the component from multiple threads
 *  concurently therefore providing a no-op mutex interface would satisfy the
 *  requirements of the component while introducing no performance penalty 
 *  during execution.
 *
 *  Other uses include providing a component the required thread or condition
 *  variable interfaces but preventing the component to create such resources.
 */
ZLX_API zlx_mth_xfc_t zlx_nosup_mth_xfc;

#endif /* _ZLX_THREAD_H */

