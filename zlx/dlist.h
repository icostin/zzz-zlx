#ifndef _ZLX_DLIST_H
#define _ZLX_DLIST_H

#include "base.h"

/** @defgroup dlist Doubly-linked list
 *  @{ */

/** Position of zlx_np_t#next in the union'ed zlx_np_t#a */
#define ZLX_NEXT 0

/** Position of zlx_np_t#prev in the union'ed zlx_np_t#a */
#define ZLX_PREV 1

/*  zlx_np_t  */
/**
 *  Generic type for next/previous links to implement a doubly-linked list
 *  of arbitrary type.
 */
typedef union zlx_np_u zlx_np_t;

union zlx_np_u
{
    struct
    {
        zlx_np_t * next; /**< next #zlx_np_t cell */
        zlx_np_t * prev; /**< previous #zlx_np_t cell */
    };
    zlx_np_t * a[2]; /**< the two links next/prev as an array */
};

/* zlx_dlist_init ***********************************************************/
/**
 *  Initializes a doubly-linked list by setting both links of the list to
 *  point to itself.
 *  @param list_p [out]
 *      list to initialize
 *  @returns the list passed as parameter
 */
ZLX_INLINE zlx_np_t * zlx_dlist_init (zlx_np_t * list_p)
{
    list_p->next = list_p->prev = list_p;
    return list_p;
}

/* zlx_dlist_is_empty *******************************************************/
/**
 *  Tells is a list is empty.
 *  @param list_p [in]
 *      list to check
 *  @retval 1 the list is empty
 *  @retval 0 the list has elements
 */
ZLX_INLINE int zlx_dlist_is_empty (zlx_np_t * list_p)
{
    return list_p->next == list_p;
}

/* zlx_dlist_ins ************************************************************/
/**
 *  Inserts an element in the specified direction of the given anchor element.
 *  @param anchor_p [in, out]
 *      anchor item relative to which the new item will be inserted
 *  @param to_ins_p [in, out]
 *      element to insert
 *  @param dir [in]
 *      direction where to insert, relative from @a anchor_p; this must be
 *      one of #ZLX_NEXT or #ZLX_PREV
 *
 */
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
/**
 *  Extends a list with the elements from another list.
 *  @param dest [in, out]
 *      list where to add the elements from @a src
 *  @param src [in, out]
 *      the list from which the elements are taken; the list element itself
 *      is not transferred to the destination list, just the items chained to it
 *  @param dir [in]
 *      direction where to insert, relative from @a dest; this must be
 *      one of #ZLX_NEXT or #ZLX_PREV
 */
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
/**
 *  Deletes an element from a list.
 *  @param to_del_p [in, out]
 *      element to unlink
 */
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
/**
 *  Appends an object to a list.
 *  @param _list [in, out]
 *      a list variable of type #zlx_np_t
 *  @param _obj_p [in, out]
 *      a structure object to be appended
 *  @param _np_field [in]
 *      the name of the list entry field that is to be linked in the given list
 */
#define ZLX_DLIST_APPEND(_list, _obj_p, _np_field) \
    (zlx_dlist_ins(&(_list), &(_obj_p)->_np_field, ZLX_PREV))

/* ZLX_DLIST_PREPEND ********************************************************/
/**
 *  Prepends an object to a list.
 *  @param _list [in, out]
 *      a list variable of type #zlx_np_t
 *  @param _obj_p [in, out]
 *      a structure object to be appended
 *  @param _np_field [in]
 *      the name of the list entry field that is to be linked in the given list
 */
#define ZLX_DLIST_PREPEND(_list, _obj_p, _np_field) \
    (zlx_dlist_ins(&(_list), &(_obj_p)->_np_field, ZLX_NEXT))

#define ZLX_DLIST_ENTRY_TO_ITEM(_entry, _item_type, _entry_member) \
    ((_item_type *) ((uint8_t *) (_entry) \
                     - ZLX_FIELD_OFS(_item_type, _entry_member)))

#define ZLX_DLIST_EACH(_list, _entry) \
    for (zlx_np_t * _entry = (_list).next, \
         * _entry##_next = _entry->next; \
         _entry != &(_list); \
         _entry = _entry##_next, _entry##_next = _entry->next)


/** @} */

#endif /* _ZLX_DLIST_H */

