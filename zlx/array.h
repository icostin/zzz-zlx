#include "base.h"

#ifndef T
#error T must be defined to represent the item type
#endif

#ifndef F
#define F(_n) ZLX_TP2D(ZLX_TP2B(T, _array_), _n)
#endif

#ifndef CMP
#define CMP(_1, _2) ((_1) == (_2) ? 0 : ((_1) < (_2) ? -1 : +1))
#endif

#ifndef EQU
#define EQU(_1, _2) ((CMP((_1), (_2)) == 0)
#endif

#ifndef FDP
#define FDP
#endif

#ifndef FDS
#define FDS
#endif

FDP void F(set) (T * arr, size_t n, T val) FDS;
FDP int F(cmp) (T const * a, T const * b, size_t n) FDS;

#ifdef ZLX_BODY
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

#endif /* ZLX_BODY */

#undef FDP
#undef FDS
#undef CMP
#undef EQU
#undef F
#undef T

