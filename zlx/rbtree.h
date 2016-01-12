#ifndef ZLX_RBTREE_H
#define ZLX_RBTREE_H

#include "base.h"

#define ZLX_DEPTH_LIMIT 0x40

#define ZLX_RBTREE_BLACK 0
#define ZLX_RBTREE_RED 1

#define ZLX_RBTREE_LEFT 0
#define ZLX_RBTREE_RIGHT 1
#define ZLX_RBTREE_MATCH 2

#define ZLX_RBTREE_FOUND 0
#define ZLX_RBTREE_NOT_FOUND 1

#endif /* defined(ZLX_RBTREE_H) */

/*
 * NODE_REF_T    node reference type
 * PATH_T    tree path type to be defined
 * KEY_T    key type
 * GET_CHILD(node)
 * SET_CHILD(node, side, child)
 * GET_COLOR(node)
 * SET_COLOR(node, color)
 * KCMP(key, node) compares the given key with the one from the given node
 *
 * the guard that some functions receive is a fake element with no key and
 * whose left child is the real root
 */

#ifndef NODE_REF_T
#error NODE_REF_T must be defined to the node type
#endif

#ifndef NODE_NULL_REF
#define NODE_NULL_REF (NULL)
#endif

#ifndef PATH_T
#error PATH_T must be defined to the tree path type
#endif

#ifndef KEY_T
#error KEY_T must be defined to the key type
#endif

#ifndef FNAME
#error FNAME(suffix) must be defined to a function macro that generates function names
#endif

#ifndef GET_CHILD
#error GET_CHILD(node, side) must be defined
#endif

#ifndef SET_CHILD
#error SET_CHILD(node, side, child) must be defined
#endif

#ifndef GET_COLOR
#error GET_COLOR(node) must be defined
#endif

#ifndef SET_COLOR
#error SET_COLOR(node, color) must be defined
#endif

#ifndef FDP
#define FDP
#endif

#ifndef FDS
#define FDS
#endif

typedef struct ZLX_TP2A(PATH_T, _s) PATH_T;
struct ZLX_TP2A(PATH_T, _s) {
    NODE_REF_T nodes[ZLX_DEPTH_LIMIT];
    uint8_t sides[ZLX_DEPTH_LIMIT];
    unsigned int depth;
};

FDP void FNAME(init) (NODE_REF_T guard) FDS;

FDP unsigned int FNAME(search) (PATH_T * ZLX_RESTRICT path, 
                            NODE_REF_T guard, 
                            KEY_T key) FDS;

FDP NODE_REF_T FNAME(extreme) (PATH_T * ZLX_RESTRICT path, 
                               NODE_REF_T guard,
                               unsigned int side) FDS;

ZLX_INLINE NODE_REF_T FNAME(first) 
    (
        PATH_T * ZLX_RESTRICT path, 
        NODE_REF_T guard
    )
{
    return FNAME(extreme)(path, guard, ZLX_RBTREE_LEFT);
}

ZLX_INLINE NODE_REF_T FNAME(last) 
    (PATH_T * ZLX_RESTRICT path, NODE_REF_T guard) {
    return FNAME(extreme)(path, guard, ZLX_RBTREE_RIGHT);
}

FDP NODE_REF_T FNAME(np) (PATH_T * ZLX_RESTRICT path, unsigned int side) FDS;

ZLX_INLINE NODE_REF_T FNAME(next) (PATH_T * ZLX_RESTRICT path) {
    return FNAME(np)(path, ZLX_RBTREE_RIGHT);
}

ZLX_INLINE NODE_REF_T FNAME(prev) (PATH_T * ZLX_RESTRICT path) {
    return FNAME(np)(path, ZLX_RBTREE_LEFT);
}

ZLX_INLINE void FNAME(path_copy) (PATH_T * ZLX_RESTRICT out, 
                                  PATH_T * ZLX_RESTRICT in) {
    unsigned int i;
    out->depth = in->depth;
    for (i = 0; i <= in->depth; ++i) {
        out->nodes[i] = in->nodes[i];
        out->sides[i] = in->sides[i];
    }
}

FDP void FNAME(insert) (PATH_T * ZLX_RESTRICT path, NODE_REF_T node) FDS;

FDP void FNAME(delete) (PATH_T * ZLX_RESTRICT path) FDS;

#if !_DEBUG
ZLX_INLINE void FNAME(dbg_print) (NODE_REF_T n, int depth, char * pfx) 
{ (void) n, (void) depth, (void) pfx; }
ZLX_INLINE void FNAME(dbg_print_path) (PATH_T * ZLX_RESTRICT p) 
{ (void) p; }
#endif

/* ZLX_BODY *****************************************************************/
#ifdef ZLX_BODY

#define Q printf("%s:%u\n", __FILE__, __LINE__);

#define IS_RED(_n) (GET_COLOR((_n)) != ZLX_RBTREE_BLACK)
#define IS_BLACK(_n) (GET_COLOR((_n)) == ZLX_RBTREE_BLACK)

#define IS_RED_SAFE(_n) \
    ((_n) != NODE_NULL_REF && GET_COLOR((_n)) != ZLX_RBTREE_BLACK)

#define IS_BLACK_SAFE(_n) \
    ((_n) == NODE_NULL_REF || GET_COLOR((_n)) == ZLX_RBTREE_BLACK)

/* init *********************************************************************/
FDP void FNAME(init) (NODE_REF_T guard) FDS
{
    SET_CHILD(guard, ZLX_RBTREE_LEFT, NODE_NULL_REF);
    SET_CHILD(guard, ZLX_RBTREE_RIGHT, NODE_NULL_REF);
    SET_COLOR(guard, ZLX_RBTREE_BLACK);
}

/* search *******************************************************************/
FDP unsigned int FNAME(search) (PATH_T * ZLX_RESTRICT path, 
                                NODE_REF_T guard, 
                                KEY_T key) FDS
{
    NODE_REF_T n;
    unsigned int i;

    path->nodes[0] = guard;
    path->sides[0] = ZLX_RBTREE_LEFT;
    for (i = 0, n = GET_CHILD(guard, ZLX_RBTREE_LEFT); n; )
    {
        int c, d;
        path->nodes[++i] = n;
        c = KCMP(key, n);
        if (!c) {
            path->sides[i] = c;
            path->depth = i;
            return ZLX_RBTREE_FOUND;
        }
        d = (c > 0);
        path->sides[i] = d;
        n = GET_CHILD(n, d);
    }
    path->depth = i;
    return ZLX_RBTREE_NOT_FOUND;
}

/* extreme ******************************************************************/
FDP NODE_REF_T FNAME(extreme) (PATH_T * ZLX_RESTRICT path, 
                               NODE_REF_T guard,
                               unsigned int side) FDS
{
    NODE_REF_T n;
    NODE_REF_T m;
    unsigned int i;

    path->nodes[0] = guard;
    path->sides[0] = side;
    for (i = 0, m = NODE_NULL_REF, n = GET_CHILD(guard, ZLX_RBTREE_BLACK); n; 
         n = GET_CHILD(n, side))
     {
         path->nodes[++i] = m = n;
         path->sides[i] = side;
     }
    path->sides[path->depth = i] = ZLX_RBTREE_MATCH;
    return m;
}

/* np ***********************************************************************/
FDP NODE_REF_T FNAME(np) (PATH_T * ZLX_RESTRICT path, unsigned int side) FDS
{
    NODE_REF_T n;
    NODE_REF_T m;
    unsigned int d;

    d = path->depth;
    n = path->nodes[d];
    m = GET_CHILD(n, side);
    if (m) {
        path->sides[d] = side;
        side ^= 1;
        while (m) {
            path->nodes[++d] = m;
            path->sides[d] = side;
            m = GET_CHILD(m, side);
        }
    } else {
        do { --d; } while (d && path->sides[d] == side);
        if (!d) return NODE_NULL_REF;
    }
    path->sides[d] = ZLX_RBTREE_MATCH;
    path->depth = d;
    return path->nodes[d];
}

/* insert *******************************************************************/
FDP void FNAME(insert) (PATH_T * ZLX_RESTRICT path, NODE_REF_T node) FDS
{
    NODE_REF_T parent;
    NODE_REF_T grandpa;
    NODE_REF_T uncle;
    NODE_REF_T tmp;
    unsigned int i, gs, ps, ts;

    i = path->depth;
    SET_CHILD(node, ZLX_RBTREE_LEFT, NODE_NULL_REF);
    SET_CHILD(node, ZLX_RBTREE_RIGHT, NODE_NULL_REF);
    SET_COLOR(node, ZLX_RBTREE_RED);
    ts = path->sides[i];
    SET_CHILD(path->nodes[i], ts, node);

    while (i) {
        parent = path->nodes[i];
        if (IS_BLACK(parent)) return;
        // parent is red (thus it is not the root)
        grandpa = path->nodes[--i];
        uncle = GET_CHILD(grandpa, path->sides[i] ^ 1);
        if (uncle && IS_RED(uncle)) {
            SET_COLOR(grandpa, ZLX_RBTREE_RED);
            SET_COLOR(parent, ZLX_RBTREE_BLACK);
            SET_COLOR(uncle, ZLX_RBTREE_BLACK);
            node = grandpa;
            --i;
            continue;
        }
        // parent is red, uncle is black or NULL
        gs = path->sides[i];
        ps = path->sides[i + 1];
        if (gs != ps) {
            // node and parent are on different sides
            SET_CHILD(grandpa, gs, node);
            tmp = GET_CHILD(node, gs);
            SET_CHILD(parent, ps, tmp);
            SET_CHILD(node, gs, parent);
            tmp = node;
            node = parent;
            parent = tmp;
        } else ps = gs ^ 1;
        // node and parent are on same side: gs; ps is set to the 'other' side
        tmp = GET_CHILD(parent, ps);
        SET_CHILD(parent, ps, grandpa);
        SET_COLOR(parent, ZLX_RBTREE_BLACK);
        SET_CHILD(grandpa, gs, tmp);
        SET_COLOR(grandpa, ZLX_RBTREE_RED);
        tmp = path->nodes[i - 1];
        ts = path->sides[i - 1];
        SET_CHILD(tmp, ts, parent);
        return;
    }
    // if we processed 'til the top of the path then root is changed
    SET_COLOR(node, ZLX_RBTREE_BLACK);
}

/* delete *******************************************************************/
FDP void FNAME(delete) (PATH_T * ZLX_RESTRICT path) FDS
{
    NODE_REF_T o;
    NODE_REF_T d;
    NODE_REF_T p;
    NODE_REF_T c;
    NODE_REF_T s;
    NODE_REF_T sl;
    NODE_REF_T sr;
    NODE_REF_T t;
    NODE_REF_T tl;
    NODE_REF_T tr;
    unsigned int cs, ds, od, dd, ns, pd, ps, ss, os, tc;

    od = path->depth;
    o = path->nodes[od]; // the node we want to delete
    if (GET_CHILD(o, ZLX_RBTREE_LEFT) != NODE_NULL_REF && 
        GET_CHILD(o, ZLX_RBTREE_RIGHT) != NODE_NULL_REF) {
        ds = path->sides[od - 1];
        d = FNAME(np)(path, ds);
        // now must delete d which has at most 1 non-null child
        dd = path->depth;
        tl = GET_CHILD(o, ZLX_RBTREE_LEFT);
        tr = GET_CHILD(o, ZLX_RBTREE_RIGHT);
        tc = GET_COLOR(o);

        SET_CHILD(o, ZLX_RBTREE_LEFT, GET_CHILD(d, ZLX_RBTREE_LEFT));
        SET_CHILD(o, ZLX_RBTREE_RIGHT, GET_CHILD(d, ZLX_RBTREE_RIGHT));
        SET_COLOR(o, GET_COLOR(d));

        SET_CHILD(d, ZLX_RBTREE_LEFT, tl);
        SET_CHILD(d, ZLX_RBTREE_RIGHT, tr);
        SET_COLOR(d, tc);

        path->nodes[od] = d;
        t = path->nodes[od - 1];
        os = path->sides[od - 1];
        SET_CHILD(t, os, d);
    } else dd = od;

    d = o;

    cs = (GET_CHILD(d, ZLX_RBTREE_LEFT) != NODE_NULL_REF)
        ? ZLX_RBTREE_LEFT : ZLX_RBTREE_RIGHT;
    c = GET_CHILD(d, cs); // c is the only child of d or NULL
    ds = path->sides[dd - 1];
    p = path->nodes[dd - 1];
    if (IS_RED(d)) {
        // d has no children since it has at most 1 non-null child and
        // both paths must have same ammount of black nodes
        SET_CHILD(p, ds, NODE_NULL_REF);
        return;
    }
    // d is black; it has either no children, or a single red child with no
    // children of its own

    if (c != NODE_NULL_REF) { // same as: (c && c->red)
        SET_CHILD(p, ds, c);
        SET_COLOR(c, ZLX_RBTREE_BLACK);
        return;
    }

    // d is black and has no children (if c is black and its sibling is NULL 
    // then it must be NULL itself)
    SET_CHILD(p, ds, NODE_NULL_REF);

    for (pd = dd - 1; pd; p = path->nodes[--pd]) {
        // n is always black (in first iteration it is NULL)
        ns = path->sides[pd];
        ss = ns ^ 1;
        s = GET_CHILD(p, ss); // this is a non-NULL node
        if (IS_RED(s)) { // implies p is black
            /* del case 2 */
            t = path->nodes[pd - 1];
            ps = path->sides[pd - 1];
            SET_CHILD(t, ps, s);
            sl = GET_CHILD(s, ns);
            SET_CHILD(p, ss, sl);
            SET_COLOR(p, ZLX_RBTREE_RED);
            SET_CHILD(s, ns, p);
            SET_COLOR(s, ZLX_RBTREE_BLACK);
            path->nodes[pd] = s;
            ++pd;
            s = sl; /* new s is black */
        } else { // s is black
            if (IS_BLACK(p) && 
                IS_BLACK_SAFE(GET_CHILD(s, ZLX_RBTREE_LEFT)) &&
                IS_BLACK_SAFE(GET_CHILD(s, ZLX_RBTREE_RIGHT))) {
                /* del case 3 */
                SET_COLOR(s, ZLX_RBTREE_RED);
                continue;
            }
        }
        // s must be black
        sl = GET_CHILD(s, ns);
        sr = GET_CHILD(s, ss);
        if (IS_RED(p) && IS_BLACK_SAFE(sl) && IS_BLACK_SAFE(sr)) {
            /* del case 4 */
            SET_COLOR(p, ZLX_RBTREE_BLACK);
            SET_COLOR(s, ZLX_RBTREE_RED);
            break;
        }
        if (IS_RED_SAFE(sl) && IS_BLACK_SAFE(sr)) {
            SET_CHILD(p, ss, sl);
            SET_CHILD(s, ns, GET_CHILD(sl, ss));
            SET_COLOR(s, ZLX_RBTREE_RED);
            SET_CHILD(sl, ss, s);
            SET_COLOR(sl, ZLX_RBTREE_BLACK);
            s = sl; // again new s is black
            sl = GET_CHILD(s, ns);
            sr = GET_CHILD(s, ss);
        }
        if (IS_RED_SAFE(sr)) {
            t = path->nodes[pd - 1];
            ps = path->sides[pd - 1];
            SET_CHILD(t, ps, s);
            SET_CHILD(p, ss, sl);
            SET_COLOR(s, GET_COLOR(p));
            SET_COLOR(p, ZLX_RBTREE_BLACK);
            SET_COLOR(sr, ZLX_RBTREE_BLACK);
            SET_CHILD(s, ns, p);
        }
        break;
    }
}

#if _DEBUG
FDP void FNAME(dbg_print) (NODE_REF_T n, int depth, char * pfx) FDS {
    if (n == NODE_NULL_REF) return;
    printf("%.*s - %s: "NODE_FMT" (%s)\n", depth * 2, 
           "                                                                ",
           pfx, NODE_PRINT_ARG(n), IS_RED(n) ? "red" : "black");
    if (GET_CHILD(n, ZLX_RBTREE_LEFT) != NODE_NULL_REF)
        FNAME(dbg_print)(GET_CHILD(n, ZLX_RBTREE_LEFT), depth + 1, "left");
    if (GET_CHILD(n, ZLX_RBTREE_RIGHT) != NODE_NULL_REF)
        FNAME(dbg_print)(GET_CHILD(n, ZLX_RBTREE_RIGHT), depth + 1, "right");
}

FDP void FNAME(dbg_print_path) (PATH_T * ZLX_RESTRICT path) FDS {
    unsigned int i;
    printf("[");
    for (i = 1; i <= path->depth; ++i)
        printf(""NODE_FMT"%s", NODE_PRINT_ARG(path->nodes[i]),
               path->sides[i] == ZLX_RBTREE_MATCH ? "" : (path->sides[i] == ZLX_RBTREE_LEFT ? " -left-> " : " -right-> "));
    printf("]\n");
}

#endif

#undef IS_RED
#undef IS_BLACK

#endif

#undef NODE_REF_T
#undef PATH_T
#undef KEY_T
#undef FDP
#undef FDS
#undef FNAME
#undef GET_CHILD
#undef SET_CHILD
#undef GET_COLOR
#undef SET_COLOR
