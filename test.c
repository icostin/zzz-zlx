#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlx.h>

#define ZLX_BODY
// #define T uint8_t
// #define F(_x) zlx_u8a_##_x

#define T uint8_t
#include <zlx/array.h>

#define T char
#define F(_n) zlx_cha_##_n
#include <zlx/array.h>

typedef struct irbt_node_s irbt_node_t;
struct irbt_node_s {
    irbt_node_t * links[2];
    unsigned int color;
    unsigned int key;
};

#define NODE_REF_T irbt_node_t *
#define PATH_T irbt_path_t
#define KEY_T unsigned int
#define NODE_FMT "%u"
#define NODE_PRINT_ARG(_n) ((_n)->key)
#define FNAME(_n) irbt_##_n
#define KCMP(_k, _n) ((_k) == (_n)->key ? 0 : ((_k) < (_n)->key ? -1 : +1))
#define GET_CHILD(_node, _side) ((_node)->links[(_side)])
#define SET_CHILD(_node, _side, _child) ((_node)->links[(_side)] = (_child))
#define GET_COLOR(_node) ((_node)->color)
#define SET_COLOR(_node, _color) ((_node)->color = (_color))
#include <zlx/rbtree.h>

#if _DEBUG
#define P(...) printf(__VA_ARGS__)
#else
#define P(...) ((void) 0)
#endif

/* irbt_test ****************************************************************/
int irbt_test ()
{
    irbt_path_t path;
    irbt_node_t guard;
    irbt_node_t * t, * m;
    unsigned int s, i, n = 10 * 1000 * 1000, k;

    t = malloc(sizeof(irbt_node_t) * n);
    if (!t) return 2;
    memset(t, -1, sizeof(irbt_node_t) * n);
    irbt_init(&guard);
    for (i = 0; i < n; ++i) {
        k = (i * 31 + 1) % n;
        P("- search for %u: ", k);
        s = irbt_search(&path, &guard, k);
        irbt_dbg_print_path(&path);
        if (s != ZLX_RBTREE_NOT_FOUND) return 1;
        t[i].key = k;
        P("- inserting %u\n", k);
        irbt_insert(&path, t + i);
        irbt_dbg_print(guard.links[0], 0, "root");
    }

    for (i = 0, m = irbt_first(&path, &guard); m; m = irbt_next(&path), ++i) {
        P("- checking %u\n", i);
        if (m->key != i) {
            fprintf(stderr, "rbtree test: expecting key %u, not %u\n", 
                    i, m->key);
            return 1;
        }
    }
    for (i = 0; i < n; ++i) {
        P("- deleting %u\n", i);
        s = irbt_search(&path, &guard, i);
        if (s != ZLX_RBTREE_FOUND) return 1;
        irbt_delete(&path);
        irbt_dbg_print(guard.links[0], 0, "root");
    }
    if (i != n) {
        fprintf(stderr, "rbtree test: expecting %u nodes, not %u\n", n, i);
        return 1;
    }

    free(t);
    return 0;
}

/* array_test ***************************************************************/
int array_test ()
{
    char c[9];
    uint8_t b[0x10];

    zlx_cha_set(c, 8, 'a');
    c[8] = 0;
    if (!(zlx_cha_cmp(c, "aaaaaaa@", 8) > 0)) return 1;
    zlx_u8a_set(b, 4, 'b');
    // printf("c: \"%s\"\n", c);
    // printf("cmp: %d\n", );
    return 0;
}


/* main *********************************************************************/
int main ()
{
    int r = 0, t;

    printf("zlx test using %s\n", zlx_lib_name);
    t = array_test(); r |= t; printf("array_test: %u\n", t);
    t = irbt_test(); r |= t; printf("irbt_test: %u\n", t);
    return 0;
}

