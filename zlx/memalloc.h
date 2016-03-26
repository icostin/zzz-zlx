#ifndef _ZLX_MEM_ALLOC_H
#define _ZLX_MEM_ALLOC_H

#include "base.h"

typedef struct zlx_ma_s zlx_ma_t;

typedef void * (ZLX_CALL * zlx_realloc_func_t) 
    (
        void * old_ptr,
        size_t old_size,
        size_t new_size,
        zlx_ma_t * ZLX_RESTRICT ma
    );

struct zlx_ma_s
{
    zlx_realloc_func_t realloc;
    void (ZLX_CALL * info_set)
        (
            zlx_ma_t * ZLX_RESTRICT ma,
            void * ptr,
            char const * src,
            unsigned int line,
            char const * func,
            char const * info
        );
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

#endif /* _ZLX_MEM_ALLOC_H */

