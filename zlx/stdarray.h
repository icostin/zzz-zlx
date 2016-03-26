#ifndef _ZLX_STD_ARRAY_H
#define _ZLX_STD_ARRAY_H

#include "base.h"
#include "memalloc.h"

ZLX_API void zlx_u8a_copy
(
    uint8_t * ZLX_RESTRICT dest,
    uint8_t const * ZLX_RESTRICT src,
    size_t n
);

/* zlx_u8a_copy *************************************************************/
/**
 *  returns the location in dest where the zero terminator is.
 */
ZLX_API uint8_t * zlx_u8a_zcopy
(
    uint8_t * ZLX_RESTRICT dest,
    uint8_t const * ZLX_RESTRICT src
);

ZLX_API void zlx_u8a_set (uint8_t * arr, size_t n, uint8_t val);
ZLX_API int zlx_u8a_cmp (uint8_t const * a, uint8_t const * b, size_t n);
ZLX_API int zlx_u8a_zcmp (uint8_t const * a, uint8_t const * b);
ZLX_API uint8_t * zlx_u8a_scan (uint8_t const * a, uint8_t value);
ZLX_API uint8_t * zlx_u8a_search (uint8_t const * a, uint8_t const * end, uint8_t value);
ZLX_API size_t zlx_u8a_zlen (uint8_t const * a);

ZLX_API uint8_t * zlx_u8a_insert
(
    uint8_t * ZLX_RESTRICT * ZLX_RESTRICT ap,
    size_t * ZLX_RESTRICT np,
    size_t * ZLX_RESTRICT mp,
    size_t p,
    size_t q,
    zlx_ma_t * ma
);


ZLX_API void zlx_s8a_set (int8_t * arr, size_t n, int8_t val);
ZLX_API int zlx_s8a_cmp (int8_t const * a, int8_t const * b, size_t n);


#define T uint8_t
#define F(_n) zlx_u8a_##_n
#define CMP(_1, _2) ((int) (_1) - (int) (_2))
#define EQU(_1, _2) ((_1) == (_2))
#define FDP ZLX_API
#include "array.h"


#define T int8_t
#define F(_n) zlx_s8a_##_n
#define CMP(_1, _2) ((int) (_1) - (int) (_2))
#define EQU(_1, _2) ((_1) == (_2))
#define FDP ZLX_API
#include "array.h"

#endif /* _ZLX_STD_ARRAY_H */
