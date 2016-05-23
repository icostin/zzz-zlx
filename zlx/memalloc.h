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
        zlx_ma_t * ZLX_RESTRICT ma
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
            zlx_ma_t * ZLX_RESTRICT ma,
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
            zlx_ma_t * ZLX_RESTRICT ma,
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
    zlx_ma_t * ZLX_RESTRICT ma,
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
    zlx_ma_t * ZLX_RESTRICT ma,
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
    zlx_ma_t * ZLX_RESTRICT ma,
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
    zlx_ma_t * ZLX_RESTRICT ma
);

ZLX_API void ZLX_CALL zlx_ma_nop_info_set
(
    zlx_ma_t * ZLX_RESTRICT ma,
    void * ptr,
    char const * src,
    unsigned int line,
    char const * func,
    char const * info
);

ZLX_API void ZLX_CALL zlx_ma_nop_check
(
    zlx_ma_t * ZLX_RESTRICT ma,
    void * ptr,
    size_t size,
    char const * src,
    unsigned int line,
    char const * func
);

/* ZLX_ARRAY_ALLOC **********************************************************/
/**
 *  Macro to allocate an array of items.
 *  @param _arr [out]
 *      variable that will be filled with the address of the array of items;
 *      the size of the items is inferred from the array type
 *  @param _ma [in, out]
 *      pointer to memory allocator instance (#zlx_ma_t *)
 *  @param _count [in]
 *      number of elements to be allocated
 *  @param _info [in]
 *      debug-only static string describing the allocated array
 */
#define ZLX_ARRAY_ALLOC(_arr, _ma, _count, _info) \
    ((_arr) = ((_count) <= SIZE_MAX / sizeof(*(_arr)) \
               ? zlx_alloc((_ma), (_count) * sizeof(*(_arr)), (_info)) \
               : NULL))

/* ZLX_ARRAY_FREE ***********************************************************/
/**
 *  Macro to free an array of items.
 */
#define ZLX_ARRAY_FREE(_arr, _ma, _count) \
    (zlx_free((_ma), (_arr), (_count) * sizeof(*(_arr))))

#endif /* _ZLX_MEM_ALLOC_H */

