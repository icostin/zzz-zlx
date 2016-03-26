#include "zlx.h"

#define FILLER ((uint8_t) 0xFE)
#define KEY ((uintptr_t) UINT64_C(0xA5A5A5A5A5A5A5A5))

typedef struct zlx_alloctrk_s zlx_alloctrk_t;
typedef struct zlx_alloctrk_header_s zlx_alloctrk_header_t;

struct zlx_alloctrk_s
{
    zlx_ma_t base;
    zlx_ma_t * ma;
    zlx_np_t list;
    size_t count;
    uintptr_t total, peak;
    zlx_log_t * log;
};

struct zlx_alloctrk_header_s
{
    zlx_np_t links;
#if _DEBUG
    char const * src;
    char const * func;
    char const * info;
    unsigned int line;
#endif
    size_t size;
    uintptr_t mark;
};

#if _DEBUG
/* alloctrk_info_set ********************************************************/
static void ZLX_CALL alloctrk_info_set
(
    zlx_ma_t * ZLX_RESTRICT ma,
    void * ptr,
    char const * src,
    unsigned int line,
    char const * func,
    char const * info
)
{
    zlx_alloctrk_header_t * ath;
    (void) ma;
    ath = ptr;
    --ath;
    if (ptr && ath->mark == (KEY ^ (uintptr_t) ptr))
    {
        ath->src = src;
        ath->line = line;
        ath->func = func;
        if (info) ath->info = info;
    }
}
#endif

/* alloctrk_check ***********************************************************/
static void ZLX_CALL alloctrk_check
(
    zlx_ma_t * ZLX_RESTRICT ma,
    void * ptr,
    size_t size,
    char const * src,
    unsigned int line,
    char const * func
)
{
    zlx_alloctrk_t * zat = (zlx_alloctrk_t *) ma;
    zlx_alloctrk_header_t * h;

#if !_DEBUG
    (void) src; (void) line; (void) func;
#endif
    if (!ptr)
    {
        if (!size) return;
        ZLX_LF(zat->log, 
#if _DEBUG
               "$s:$i@$s(): "
#endif
               "*** BUG *** BAD BLOCK: ptr=NULL size=$z expected_size=0\n",
#if _DEBUG
               src, line, func,
#endif
               size);
        zlx_abort();
    }
    h = ptr;
    h--;
    if (h->mark != (KEY ^ (uintptr_t) ptr))
    {
        ZLX_LF(zat->log,
#if _DEBUG
               "$s:$i@$s(): "
#endif
               "*** BUG *** BAD BLOCK: "
               "ptr=$xp size=$z marker=$xp expected_marker=$xp\n",
#if _DEBUG
               src, line, func,
#endif
               ptr, size, h->mark, (KEY ^ (uintptr_t) ptr));
        zlx_abort();
    }
    if (h->size != size)
    {
        ZLX_LF(zat->log,
#if _DEBUG
               "$s:$i@$s(): "
#endif
               "*** BUG *** BAD BLOCK: "
               "ptr=$xp size=$z expected_size=$z\n",
#if _DEBUG
               src, line, func,
#endif
               ptr, size, h->size);
        zlx_abort();
    }
    if (zlx_u8a_cmp((uint8_t *) ptr + size, (uint8_t *) &h->mark, 
                    sizeof(uintptr_t)))
    {
        uintptr_t mark;
        zlx_u8a_copy((uint8_t *) &mark, (uint8_t *) ptr + size, 
                     sizeof(uintptr_t));
        ZLX_LF(zat->log,
#if _DEBUG
               "$s:$i@$s(): "
#endif
               "*** BUG *** BAD BLOCK: "
               "ptr=$xp size=$z end_marker=$xp expected_marker=$xp\n",
#if _DEBUG
               src, line, func,
#endif
               ptr, size, mark, (KEY ^ (uintptr_t) ptr));
        zlx_abort();
    }
}

/* alloctrk_realloc *********************************************************/
static void * ZLX_CALL alloctrk_realloc
(
    void * old_ptr,
    size_t old_size,
    size_t new_size,
    zlx_ma_t * ma
)
{
    zlx_alloctrk_t * zat = (zlx_alloctrk_t *) ma;
    zlx_alloctrk_header_t * h, * rh;
    uint8_t * p;

    if (!old_size)
    {
        size_t z;

        if (!new_size) return NULL;
        /* alloc */
        z = sizeof(zlx_alloctrk_header_t) + sizeof(uintptr_t) + new_size;
        if (z < sizeof(zlx_alloctrk_header_t) + sizeof(uintptr_t))
            return NULL;
        h = zat->ma->realloc(NULL, 0, z, zat->ma);
        if (!h) return NULL;
        zat->count++;
        zlx_u8a_set((uint8_t *) (h + 1), new_size, FILLER);
#if _DEBUG
        h->src = NULL;
        h->line = 0;
        h->func = NULL;
        h->info = NULL;
#endif
    }
    else
    {
        size_t oz, nz;
        /* realloc or free */
        h = old_ptr;
        h--;
        if (old_size == new_size) return old_ptr;

        zlx_dlist_del(&h->links);

        oz = sizeof(zlx_alloctrk_header_t) + sizeof(uintptr_t) + old_size;
        if (!new_size)
        {
            /* free */
            p = zat->ma->realloc(h, oz, 0, zat->ma);
            ZLX_ASSERT(p == NULL);
            ZLX_ASSERT(zat->total >= old_size);
            zat->total -= old_size;
            ZLX_ASSERT(zat->count > 0);
            zat->count--;
            ZLX_LD(zat->log, "alloctrk free: count=$z, total=$Np, peak=$Np\n",
                   zat->count, zat->total, zat->peak);
            return NULL;
        }
        /* realloc - old buffer is unlinked, now do the realloc */
        nz = sizeof(zlx_alloctrk_header_t) + sizeof(uintptr_t) + new_size;
        rh = zat->ma->realloc(h, oz, nz, zat->ma);
        if (!rh)
        {
            ZLX_LD(zat->log, "alloctrk: realloc failed (oz=$z, nz=$z)\n", 
                   oz, nz);
            ZLX_DLIST_APPEND(zat->list, h, links);
            return NULL;
        }
        h = rh;
        if (new_size > old_size)
            zlx_u8a_set((uint8_t *) (h + 1) + old_size, 
                        new_size - old_size, FILLER);
    }

    /* alloc or realloc - common setup of allocated buffer */
    p = (uint8_t *) (h + 1);
    h->size = new_size;
    h->mark = KEY ^ (uintptr_t) p;
    zlx_u8a_copy(p + new_size, (uint8_t *) &h->mark, sizeof(uintptr_t));
    ZLX_DLIST_APPEND(zat->list, h, links);
    zat->total += new_size - old_size;
    if (zat->total > zat->peak) zat->peak = zat->total;
    ZLX_LD(zat->log, "alloctrk (alloc/realloc): op=$xp, os=$xz, np=$xp, ns=$xz, count=$z, total=$xp, peak=$xp\n", old_ptr, old_size, p, new_size, zat->count, zat->total, zat->peak);
    return p;
}

/* zlx_alloctrk_create ******************************************************/
ZLX_API zlx_ma_t * ZLX_CALL zlx_alloctrk_create
(
    zlx_ma_t * ma,
    zlx_log_t * log
)
{
    zlx_alloctrk_t * zat;
    zat = ma->realloc(NULL, 0, sizeof(*zat), ma);
    if (!zat) return NULL;
    zat->base.realloc = alloctrk_realloc;
    zat->base.check = alloctrk_check;
    zat->base.info_set = 
#if _DEBUG
        alloctrk_info_set
#else
        zlx_ma_nop_info_set
#endif
        ;
    zat->ma = ma;
    zlx_dlist_init(&zat->list);
    zat->count = 0;
    zat->total = 0;
    zat->peak = 0;
    zat->log = log;
    return &zat->base;
}

/* zlx_alloctrk_dump ********************************************************/
ZLX_API void ZLX_CALL zlx_alloctrk_dump
(
    zlx_ma_t * ma
)
{
    zlx_alloctrk_t * zat = (zlx_alloctrk_t *) ma;
    zlx_np_t * p;
    zlx_alloctrk_header_t * h;
    ZLX_LF(zat->log, "alloc tracker state: total=$xp peak=$xp count=$z\n",
           zat->total, zat->peak, zat->count);
    for (p = zat->list.next; p != &zat->list; p = p->next)
    {
        h = (zlx_alloctrk_header_t *) p;
        ZLX_LF(zat->log, "[$xp, +$xz]", h + 1, h->size);
#if _DEBUG
        ZLX_LF(zat->log, " $s:$i@$s()", h->src, h->line, h->func);
        if (h->info) ZLX_LF(zat->log, ":$s", h->info);
#endif
        ZLX_LF(zat->log, " = $.*xs$s\n", (h->size > 16 ? 16 : h->size),
               h + 1, (h->size > 16 ? "..." : ""));
    }
}

/* zlx_alloctrk_destroy *****************************************************/
ZLX_API zlx_ma_t * ZLX_CALL zlx_alloctrk_destroy
(
    zlx_ma_t * ma
)
{
    zlx_alloctrk_t * zat = (zlx_alloctrk_t *) ma;
    zlx_ma_t * bma;
    zlx_np_t * p;
    zlx_alloctrk_header_t * h;

    bma = zat->ma;
    for (p = zat->list.next; p != &zat->list; )
    {
        h = (zlx_alloctrk_header_t *) p;
        p = p->next;
        bma->realloc(h, 
            h->size + sizeof(zlx_alloctrk_header_t) + sizeof(uintptr_t), 
            0, bma);
    }
    return bma;
}

