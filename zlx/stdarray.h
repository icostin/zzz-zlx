#ifndef _ZLX_STD_ARRAY_H
#define _ZLX_STD_ARRAY_H

#include "base.h"

ZLX_API void zlx_u8a_set (uint8_t * arr, size_t n, uint8_t val);
ZLX_API int zlx_u8a_cmp (uint8_t const * a, uint8_t const * b, size_t n);

ZLX_API void zlx_s8a_set (int8_t * arr, size_t n, int8_t val);
ZLX_API int zlx_s8a_cmp (int8_t const * a, int8_t const * b, size_t n);

#define T uint8_t
#define F(_n) zlx_u8a_##_n
#define CMP(_1, _2) ((int) (_1) - (int) (_2))
#define FDP ZLX_API
#include "array.h"


#define T int8_t
#define F(_n) zlx_s8a_##_n
#define CMP(_1, _2) ((int) (_1) - (int) (_2))
#define FDP ZLX_API
#include "array.h"

#endif /* _ZLX_STD_ARRAY_H */
