#ifndef _ZLX_DLIST_H
#define _ZLX_DLIST_H

#include "base.h"

#define ZLX_NEXT 0
#define ZLX_PREV 1

typedef union zlx_np_u zlx_np_t;

union zlx_np_u
{
    struct
    {
        zlx_np_t * next;
        zlx_np_t * prev;
    };
    zlx_np_t * a[2];
};

/* zlx_dlist_init ***********************************************************/
ZLX_INLINE zlx_np_t * zlx_dlist_init (zlx_np_t * list_p)
{
    list_p->next = list_p->prev = list_p;
    return list_p;
}

/* zlx_dlist_is_empty *******************************************************/
ZLX_INLINE int zlx_dlist_is_empty (zlx_np_t * list_p)
{
    return list_p->next == list_p;
}

/* zlx_dlist_ins ************************************************************/
ZLX_INLINE void zlx_dlist_ins 
(
    zlx_np_t * anchor_p, 
    zlx_np_t * to_ins_p, 
    int dir
)
{
    zlx_np_t * n;
    to_ins_p->a[dir] = n = anchor_p->a[dir];
    to_ins_p->a[dir ^ 1] = anchor_p;
    n->a[dir ^ 1] = to_ins_p;
    anchor_p->a[dir] = to_ins_p;
}

/* zlx_dlist_extend *********************************************************/
ZLX_INLINE void zlx_dlist_extend
(
    zlx_np_t * dest,
    zlx_np_t * src,
    int dir
)
{
    int rev = dir ^ 1;
    zlx_np_t * de = dest->a[rev];
    zlx_np_t * sb = src->a[dir];
    zlx_np_t * se = src->a[rev];
    de->a[dir] = sb;
    sb->a[rev] = de;
    se->a[dir] = dest;
    dest->a[rev] = se;
}

/* zlx_dlist_del ************************************************************/
ZLX_INLINE void zlx_dlist_del
(
    zlx_np_t * to_del_p
)
{
    zlx_np_t * p;
    zlx_np_t * n;
    n = to_del_p->next;
    p = to_del_p->prev;
    n->prev = p;
    p->next = n;
}

/* ZLX_DLIST_APPEND *********************************************************/
#define ZLX_DLIST_APPEND(_list, _obj_p, _np_field) \
    (zlx_dlist_ins(&(_list), &(_obj_p)->_np_field, ZLX_PREV))

/* ZLX_DLIST_PREPEND ********************************************************/
#define ZLX_DLIST_PREPEND(_list, _obj_p, _np_field) \
    (zlx_dlist_ins(&(_list), &(_obj_p)->_np_field, ZLX_NEXT))

#endif /* _ZLX_DLIST_H */

