#include "base.h"
#include "memalloc.h"

/** @defgroup array Array generator
 *  Defines and implements arrays of arbitrary types based on other macros
 *  to do operations on elements.
 *
 *  The typical use of this component:
 *  - in a header file, define macros that describe the item and help 
 *    declare generated functions then include "zlx/array.h" to generate 
 *    the header declarations
 *  - in a C source file define the same macros as above, define ZLX_BODY
 *    and include again "zlx/array.h" to generate function bodies.
 *
 *  Macros that affect the generated source:
 *  - T: item type
 *  - F(n): function name decorator
 *  - CMP: macro to compare two items, returning 0 for equal, -1 or +1 for
 *    less or more
 *  - EQU: macro that compares two items returning non-zero if items are equal
 *    and 0 if not
 *  - COPY: macro that copies one item
 *  - FDP: function decorator prefix
 *  - FDS: function decorator suffix
 *  - ZERO: macro representing a "zero" item
 *
 *  @{ */

#ifndef T
#error T must be defined to represent the item type
#endif

#ifndef F
#define F(_n) (ZLX_TP2D(ZLX_TP2B(T, _array_), _n))
#endif

#ifndef CMP
#define CMP(_1, _2) ((_1) == (_2) ? 0 : ((_1) < (_2) ? -1 : +1))
#endif

#ifndef EQU
#define EQU(_1, _2) ((_1) == (_2))
#endif

#ifndef COPY
#define COPY(_d, _s) (_d) = (_s)
#endif

#ifndef FDP
#define FDP
#endif

#ifndef FDS
#define FDS
#endif

#ifndef ZERO
#define ZERO ((T) 0)
#endif

FDP void F(set) (T * arr, size_t n, T val) FDS;
FDP void F(copy) (T * restrict a, T const * restrict b, size_t n) FDS;
FDP T * F(zcopy) (T * restrict a, T const * restrict b) FDS;

FDP int F(cmp) (T const * a, T const * b, size_t n) FDS;
FDP T * F(scan) (T const * arr, T value) FDS;
FDP T * F(search) (T const * a, T const * end, T value) FDS;
FDP T * F(insert)
(
    T * restrict * restrict ap,
    size_t * restrict np,
    size_t * restrict mp,
    size_t p,
    size_t q,
    zlx_ma_t * ma
) FDS;

FDP size_t F(zlen) (T const * restrict a) FDS;
FDP int F(zcmp) (T const * a, T const * b) FDS;


#ifdef ZLX_BODY
FDP void F(copy) (T * restrict a, T const * restrict b, size_t n) FDS
{
    size_t i;
    for (i = 0; i < n; ++i)
    {
        COPY(a[i], b[i]);
    }
}

FDP T * F(zcopy) (T * restrict a, T const * restrict b) FDS
{
    size_t i;
    for (i = 0;; ++i)
    {
        COPY(a[i], b[i]);
        if (EQU(b[i], ZERO)) break;
    }
    return a + i;
}

FDP void F(set) (T * arr, size_t n, T val) FDS
{
    size_t i;
    for (i = 0; i < n; ++i) arr[i] = val;
}

FDP int F(cmp) (T const * a, T const * b, size_t n) FDS
{
    size_t i;
    for (i = 0; i < n; ++i)
    {
        int c = CMP(a[i], b[i]);
        if (c) return c;
    }
    return 0;
}

FDP T * F(scan) (T const * arr, T value) FDS
{
    while (!EQU(*arr, value)) ++arr;
    return (T *) arr;
}

FDP size_t F(zlen) (T const * restrict a) FDS
{
    return F(scan)(a, ZERO) - a;
}

FDP T * F(search) (T const * a, T const * end, T value) FDS
{
    while (a != end && !EQU(*a, value)) ++a;
    return (T *) a;
}

FDP T * F(insert)
(
    T * restrict * restrict ap,
    size_t * restrict np,
    size_t * restrict mp,
    size_t p,
    size_t q,
    zlx_ma_t * restrict ma
) FDS
{
    T * a;
    size_t n, m, i;
    n = *np + q;
    if (n < q) return NULL;
    if (n > SIZE_MAX / sizeof(T)) return NULL;
    if (n > *mp)
    {
        m = ((size_t) 1 << zlx_size_log2_ceil(n * sizeof(T))) / sizeof(T);
        a = zlx_realloc(ma, *ap, *mp * sizeof(T), m * sizeof(T));
        if (!a) return NULL;
        *ap = a;
        *mp = m;
    }
    else a = *ap;
    for (i = *np; i > p; )
    {
        --i;
        COPY(a[i + q], a[i]);
    }
    *np = n;
    return a + p;
}

FDP int F(zcmp) (T const * a, T const * b) FDS
{
    for (;; ++a, ++b)
    {
        int c;
        c = CMP(*a, *b);
        if (c) return c;
        if (EQU(*a, ZERO)) return 0;
    }
}

#endif /* ZLX_BODY */

#undef FDP
#undef FDS
#undef CMP
#undef EQU
#undef COPY
#undef ZERO
#undef F
#undef T

/** @} */

