#ifndef _ZLX_MEM_ALLOC_H
#define _ZLX_MEM_ALLOC_H

#include "base.h"

/* zlx_ma_t *****************************************************************/
/**
 *  Memory allocator instance base structure.
 */
typedef struct zlx_ma_s zlx_ma_t;

/* zlx_realloc_func_t *******************************************************/
/**
 *  Function to allocate, reallocate or free a block of memory.
 *  @param old_ptr [in]
 *      pointer of allocated block of memory, or NULL for allocating a new block
 *  @param old_size [in]
 *      size of the already allocated block of memory, or 0 for allocating
 *      a new block
 *  @param new_size [in]
 *      new size requested for the block, or 0 for freeing the existent block
 *  @param ma [in, out]
 *      memory allocator instance
 *  @returns 
 *      the new block or NULL if the block was freed or there was an error
 *      allocating the block
 */
typedef void * (ZLX_CALL * zlx_realloc_func_t) 
    (
        void * old_ptr,
        size_t old_size,
        size_t new_size,
        zlx_ma_t * restrict ma
    );

struct zlx_ma_s
{
    /** Function to do the reallocation. */
    zlx_realloc_func_t realloc;
    
    /** Function to store information about an allocated block.
     *  This is intended to be used by memory tracker allocators to show
     *  some meaningful information to the programmer about memory leaks.
     *  A memory allocator that does not track the allocated blocks can
     *  simply use for this member the function zlx_ma_nop_info_set().
     */
    void (ZLX_CALL * info_set)
        (
            zlx_ma_t * restrict ma,
            void * ptr,
            char const * src,
            unsigned int line,
            char const * func,
            char const * info
        );

    /** Function that validates if the given block has been previously
     *  allocated by this allocator and not freed since.
     */
    void (ZLX_CALL * check)
        (
            zlx_ma_t * restrict ma,
            void * ptr,
            size_t size,
            char const * src,
            unsigned int line,
            char const * func
        );
};

/* zlxi_alloc ***************************************************************/
ZLX_INLINE void * zlxi_alloc
(
    zlx_ma_t * restrict ma,
    size_t size
#if _DEBUG
    , char const * src
    , unsigned int line
    , char const * func
    , char const * info
#endif
)
{
    void * new_ptr;
    new_ptr = ma->realloc(NULL, 0, size, ma);
#if _DEBUG
    if (new_ptr) ma->info_set(ma, new_ptr, src, line, func, info);
#endif
    return new_ptr;
}

/* zlxi_realloc *************************************************************/
ZLX_INLINE void * zlxi_realloc
(
    zlx_ma_t * restrict ma,
    void * old_ptr,
    size_t old_size,
    size_t new_size
#if _DEBUG
    , char const * src
    , unsigned int line
    , char const * func
    , char const * info
#endif
)
{
    void * new_ptr;
#if _DEBUG || _CHECKED
    ma->check(ma, old_ptr, old_size, 
#if _DEBUG
              src, line, func
#else
              NULL, 0, NULL
#endif
              );
#endif
    new_ptr = ma->realloc(old_ptr, old_size, new_size, ma);
#if _DEBUG
    if (new_ptr) ma->info_set(ma, new_ptr, src, line, func, info);
#endif
    return new_ptr;
}

/* zlxi_free ****************************************************************/
ZLX_INLINE void zlxi_free
(
    zlx_ma_t * restrict ma,
    void * ptr,
    size_t size
#if _DEBUG
    , char const * src
    , unsigned int line
    , char const * func
#endif
)
{
#if _DEBUG || _CHECKED
    ma->check(ma, ptr, size, 
#if _DEBUG
              src, line, func
#else
              NULL, 0, NULL
#endif
             );
#endif
    ma->realloc(ptr, size, 0, ma);
}

#if _DEBUG
#define zlx_alloc(_ma, _size, _info) \
    (zlxi_alloc((_ma), (_size), __FILE__, __LINE__, __FUNCTION__, (_info)))

#define zlx_realloc(_ma, _old_ptr, _old_size, _new_size) \
    (zlxi_realloc((_ma), (_old_ptr), (_old_size), (_new_size), \
                  __FILE__, __LINE__, __FUNCTION__, NULL))

#define zlx_free(_ma, _ptr, _size) \
    (zlxi_free((_ma), (_ptr), (_size), __FILE__, __LINE__, __FUNCTION__))
#else
/*  zlx_alloc  */
/**
 *  Allocates a memory block.
 *  @param _ma [in, out]
 *      a pointer to a zlx_ma_t structure
 *  @param _size [in]
 *      size for the requested block
 *  @param _info [in]
 *      string evaluated only on debug builds that gives a hint to the 
 *      programmer for what the memory block is indended for
 */
#define zlx_alloc(_ma, _size, _info) (zlxi_alloc((_ma), (_size)))

#define zlx_realloc(_ma, _old_ptr, _old_size, _new_size) \
    (zlxi_realloc((_ma), (_old_ptr), (_old_size), (_new_size)))

#define zlx_free(_ma, _ptr, _size) (zlxi_free((_ma), (_ptr), (_size)))
#endif

ZLX_API void * ZLX_CALL zlx_ma_nop_realloc
(
    void * old_ptr,
    size_t old_size,
    size_t new_size,
    zlx_ma_t * restrict ma
);

ZLX_API void ZLX_CALL zlx_ma_nop_info_set
(
    zlx_ma_t * restrict ma,
    void * restrict ptr,
    char const * restrict src,
    unsigned int line,
    char const * restrict func,
    char const * restrict info
);

ZLX_API void ZLX_CALL zlx_ma_nop_check
(
    zlx_ma_t * restrict ma,
    void * restrict ptr,
    size_t size,
    char const * restrict src,
    unsigned int line,
    char const * restrict func
);

/* zlxi_array_alloc *********************************************************/
ZLX_INLINE int zlxi_array_alloc
(
    zlx_ma_t * restrict ma,
    void * * restrict ptr,
    size_t * restrict count,
    size_t new_count,
    size_t item_size
#if _DEBUG
    , char const * src
    , unsigned int line
    , char const * func
    , char const * info
#endif
)
{
    void * new_ptr;
    if (new_count > PTRDIFF_MAX / item_size) 
    {
        *ptr = NULL;
        *count = 0;
        return 1;
    }
    new_ptr = ma->realloc(NULL, 0, new_count * item_size, ma);
    if (new_ptr)
    {
#if _DEBUG
        ma->info_set(ma, new_ptr, src, line, func, info);
#endif
        *ptr = new_ptr;
        *count = new_count;
    }
    else
    {
        *ptr = NULL;
        *count = 0;
    }

    return !new_ptr;
}

/* zlxi_array_realloc *******************************************************/
/**
 *  Resizes a block updating the pointer and size on success.
 *  @retval 0 resized ok
 *  @retval 1 failed
 */
ZLX_INLINE int zlxi_array_realloc
(
    zlx_ma_t * restrict ma,
    void * * restrict ptr,
    size_t * restrict count,
    size_t new_count,
    size_t item_size
#if _DEBUG
    , char const * src
    , unsigned int line
    , char const * func
    , char const * info
#endif
)
{
    void * new_ptr;
#if _DEBUG || _CHECKED
    ma->check(ma, *ptr, *count * item_size, 
# if _DEBUG
              src, line, func
# else
              NULL, 0, NULL
# endif
              );
#endif
    if (new_count > PTRDIFF_MAX / item_size) return 1;
    new_ptr = ma->realloc(*ptr, *count * item_size, new_count * item_size, ma);
    if (new_ptr)
    {
#if _DEBUG
        ma->info_set(ma, new_ptr, src, line, func, info);
#endif
        *ptr = new_ptr;
        *count = new_count;
    }
    return !new_ptr;
}

#if _DEBUG
#define ZLX_ARRAY_ALLOC(_ma, _arr_var, _count_var, _count, _info) \
    (zlxi_array_alloc(_ma, (void * *) &(_arr_var), &(_count_var), (_count), \
                      sizeof((_arr_var)[0]), \
                      __FILE__, __LINE__, __FUNCTION__, (_info)))
#define ZLX_ARRAY_REALLOC(_ma, _arr_var, _count_var, _count, _info) \
    (zlxi_array_realloc(_ma, (void * *) &(_arr_var), &(_count_var), (_count), \
                      sizeof((_arr_var)[0]), \
                      __FILE__, __LINE__, __FUNCTION__, (_info)))
#else
/* ZLX_ARRAY_ALLOC **********************************************************/
/**
 *  Allocates data for an array of items.
 *  The size of items is inferred from given array variable.
 *
 *  @param _ma [in, out] mem allocator
 *  @param _arr_var [out] variable of type pointer to items
 *  @param _count_var [out] variable to hold the size (must be size_t)
 *  @param _count [in] how many items to allocate
 *  @param _info [in] debug-only info string describing the array
 *  @retval 0 ok
 *  @retval 1 failed; the array is inited as NULL for data, 0 for size
 */
#define ZLX_ARRAY_ALLOC(_ma, _arr_var, _count_var, _count, _info) \
    (zlxi_array_alloc(_ma, (void * *) &(_arr_var), &(_count_var), (_count), \
                      sizeof((_arr_var)[0])))

/* ZLX_ARRAY_REALLOC ********************************************************/
/**
 *  Reallocates data for an array of items.
 *  @retval 0 ok
 *  @retval 1 failed; @a _arr_var and @a _count_var are left unchanged
 */
#define ZLX_ARRAY_REALLOC(_ma, _arr_var, _count_var, _count, _info) \
    (zlxi_array_realloc(_ma, (void * *) &(_arr_var), &(_count_var), (_count), \
                      sizeof((_arr_var)[0])))
#endif

/* ZLX_ARRAY_INIT ***********************************************************/
/**
 *  Initializes to (NULL, 0) the pair (pointer, size) representing an array.
 */
#define ZLX_ARRAY_INIT(_arr_var, _count_var) \
    ((_arr_var) = NULL, (_count_var) = 0)

/* ZLX_ARRAY_FREE ***********************************************************/
/**
 *  Macro to free an array of items.
 */
#define ZLX_ARRAY_FREE(_ma, _arr, _count) \
    (zlx_free((_ma), (_arr), (_count) * sizeof(*(_arr))))




#endif /* _ZLX_MEM_ALLOC_H */

