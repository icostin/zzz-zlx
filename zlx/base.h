#ifndef _ZLX_BASE_H
#define _ZLX_BASE_H

/** @defgroup base Zalmoxis base declarations
 *  Basic macros and types are made available through this component.
 *
 *  @{ */

#include <limits.h>
#include <stdarg.h>
#include <stddef.h>

#include "arch.h"

#if _MSC_VER
typedef unsigned char uint8_t;
typedef   signed char  int8_t;
typedef unsigned short int uint16_t;
typedef   signed short int  int16_t;
typedef unsigned int uint32_t;
typedef   signed int  int32_t;
typedef unsigned __int64 uint64_t;
typedef   signed __int64  int64_t;
#else
#include <stdint.h>
#endif

/*  ZLX_INLINE  */
/**
 *  compiler-specific declaration for "inline" declaration attribute
 */
#define ZLX_INLINE static __inline

#if _WIN32 && !_WIN64
#define ZLX_CALL __fastcall
#else
/*  ZLX_CALL  */
/**
 *  calling convention used by this library
 */
#define ZLX_CALL
#endif

#if __STDC_VERSION__ < 199901L
# if _MSC_VER >= 1600
#  define restrict __restrict
# elif __GNUC__ >= 3
#  define restrict __restrict__
# else
#  define restrict
# endif
#endif

//#ifndef restrict
//#   ifdef _ISOC99_SOURCE
//#       define restrict restrict
//#   elif _MSC_VER >= 1600
//#       define restrict __restrict
//#   elif __GNUC__ >= 3
//#       define restrict __restrict__
//#   else
///*  restrict  */
///**
// *  expands to compiler-specific "restrict" keyword as defined in the C99 
// *  standard
// */
//#       define restrict
//#   endif
//#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#   define ZLX_LIB_EXPORT __declspec(dllexport)
#   define ZLX_LIB_IMPORT __declspec(dllimport)
#   define ZLX_LOCAL
#elif __GNUC__ >= 4
#   define ZLX_LIB_IMPORT __attribute__ ((visibility ("default")))
#   define ZLX_LIB_EXPORT __attribute__ ((visibility ("default")))
#   define ZLX_LOCAL __attribute__ ((visibility ("hidden")))
#else //if defined(__GNUC__)
#   define ZLX_LIB_IMPORT
#   define ZLX_LIB_EXPORT
#   define ZLX_LOCAL
#endif

#if ZLX_STATIC
#define ZLX_API
#elif ZLX_DYNAMIC
#define ZLX_API ZLX_LIB_EXPORT
#else
/*  ZLX_API  */
/**
 *  Declaration for exported variables and functions.
 *  When building the library this expands to whatever declaration specifier
 *  to signify exporting, otherwise expands to the counter-specifier signifying
 *  importing.
 */
#define ZLX_API ZLX_LIB_IMPORT
#endif

#define ZLX_1_OF_2(_1, _2) _1
#define ZLX_2_OF_2(_1, _2) _2
#define ZLX_1_OF_L2(_l) ZLX_1_OF_2(_l)
#define ZLX_2_OF_L2(_l) ZLX_2_OF_2(_l)

#define ZLX_1_OF_3(_1, _2, _3) _1
#define ZLX_2_OF_3(_1, _2, _3) _2
#define ZLX_3_OF_3(_1, _2, _3) _3
#define ZLX_1_OF_L3(_l) ZLX_1_OF_3(_l)
#define ZLX_2_OF_L3(_l) ZLX_2_OF_3(_l)
#define ZLX_3_OF_L3(_l) ZLX_3_OF_3(_l)

#define ZLX_1_OF_4(_1, _2, _3, _4) _1
#define ZLX_2_OF_4(_1, _2, _3, _4) _2
#define ZLX_3_OF_4(_1, _2, _3, _4) _3
#define ZLX_4_OF_4(_1, _2, _3, _4) _4
#define ZLX_1_OF_L4(_l) ZLX_1_OF_4(_l)
#define ZLX_2_OF_L4(_l) ZLX_2_OF_4(_l)
#define ZLX_3_OF_L4(_l) ZLX_3_OF_4(_l)
#define ZLX_4_OF_L4(_l) ZLX_4_OF_4(_l)

#define ZLX_TP2A(_1, _2) _1 ## _2
#define ZLX_TP2B(_1, _2) ZLX_TP2A(_1, _2)
#define ZLX_TP2C(_1, _2) ZLX_TP2B(_1, _2)
#define ZLX_TP2D(_1, _2) ZLX_TP2C(_1, _2)
#define ZLX_TP2(_1, _2) ZLX_TP2D(_1, _2)


/*  ZLX_FIELD_OFS  */
/**
 *  macro returning the offset to a given field.
 */
#define ZLX_FIELD_OFS(_type, _field) \
    ((uintptr_t) &(((_type *) 0)->_field))

/*  ZLX_FIELD_END_OFS  */
/**
 *  macro returning the offset past a given field.
 */
#define ZLX_FIELD_END_OFS(_type, _field) \
    (ZLX_FIELD_OFS(_type, _field) + sizeof((((_type *) 0)->_field)))

/*  ZLX_STRUCT_FROM_FIELD  */
/**
 *  macro that takes a pointer to a structure field and gives the pointer to
 *  the structure containing it.
 */
#define ZLX_STRUCT_FROM_FIELD(_type, _field, _field_ptr) \
    ((_type *) ((uintptr_t) (_field_ptr) - ZLX_FIELD_OFS(_type, _field)))

/*  ZLX_ITEM_COUNT  */
/**
 *  macro that returns the number of items in an array whose size is declared
 *  at compile time.
 */
#define ZLX_ITEM_COUNT(_a) (sizeof(_a) / sizeof(_a[0]))

#if _DEBUG
#define ZLX_ON_DEBUG(_expr) _expr
#else
/*  ZLX_ON_DEBUG  */
/**
 *  macro that evaluates the given expression only on debug builds.
 */
#define ZLX_ON_DEBUG(_expr)
#endif



/* zlx_u8_log2_ceil *********************************************************/
/**
 *  Computes the smallest power of 2 that is greater or equal to the given 
 *  8-bit unsigned int.
 */
ZLX_INLINE uint8_t zlx_u8_log2_ceil (uint8_t x)
{
    return x <= 0x10
        ? (x <= 4 
           ? (((2 << 8) | (2 << 6) | (1 << 4)) >> (x + x)) & 3
           : 3 + (x > 8))
        : (x <= 0x40 ? 5 + (x > 0x20) : 7 + (x > 0x80));
}

/* zlx_u16_log2_ceil ********************************************************/
/**
 *  Computes the smallest power of 2 that is greater or equal to the given 
 *  16-bit unsigned int.
 */
ZLX_INLINE uint8_t zlx_u16_log2_ceil (uint16_t x)
{
    return x == (uint8_t) x 
        ? zlx_u8_log2_ceil((uint8_t) x)
        : 8 + zlx_u8_log2_ceil((uint8_t) (x >> 8));
}

/* zlx_u32_log2_ceil ********************************************************/
/**
 *  Computes the smallest power of 2 that is greater or equal to the given 
 *  32-bit unsigned int.
 */
ZLX_INLINE uint8_t zlx_u32_log2_ceil (uint32_t x)
{
    return x == (uint16_t) x 
        ? zlx_u16_log2_ceil((uint16_t) x)
        : 16 + zlx_u16_log2_ceil((uint16_t) (x >> 16));
}

/* zlx_u64_log2_ceil ********************************************************/
/**
 *  Computes the smallest power of 2 that is greater or equal to the given 
 *  64-bit unsigned int.
 */
ZLX_INLINE uint8_t zlx_u64_log2_ceil (uint64_t x)
{
    return x == (uint32_t) x 
        ? zlx_u32_log2_ceil((uint32_t) x) 
        : 32 + zlx_u32_log2_ceil((uint32_t) (x >> 32));
}

#if ZLX_BITS == 64
#define zlx_size_log2_ceil zlx_u64_log2_ceil
#define zlx_uptr_log2_ceil zlx_u64_log2_ceil
#else
/*  zlx_size_log2_ceil  */
/**
 *  Computes the smallest power of 2 that is greater or equal to the given 
 *  size_t unsigned int.
 */
#define zlx_size_log2_ceil zlx_u32_log2_ceil

/* zlx_uptr_log2_ceil */
/**
 *  Computes the smallest power of 2 that is greater or equal to the given 
 *  pointer-sized unsigned int.
 */
#define zlx_uptr_log2_ceil zlx_u32_log2_ceil
#endif

/* zlx_seqbswap16 ***********************************************************/
/**
 *  Sequential 16-bit byte-swap.
 *  This function computes the byte-swap value by shifting each byte from the
 *  input value to its final bit location.
 */
ZLX_INLINE uint16_t zlx_seqbswap16 (uint16_t v)
{
    return ((uint16_t) ((uint8_t) v) << 8) | (v >> 8);
}

/* zlx_seqbswap32 ***********************************************************/
/**
 *  Sequential 32-bit byte-swap.
 *  This function computes the byte-swap value by shifting each byte from the
 *  input value to its final bit location.
 */
ZLX_INLINE uint32_t zlx_seqbswap32 (uint32_t v)
{
    return ((uint32_t) ((uint8_t) v) << 24) | ((v & 0xFF00) << 8)
        | ((v >> 8) & 0xFF00) | (v >> 24);
}

/* zlx_seqbswap64 ***********************************************************/
/**
 *  Sequential 32-bit byte-swap.
 *  This function computes the byte-swap value by shifting each byte from the
 *  input value to its final bit location.
 */
ZLX_INLINE uint64_t zlx_seqbswap64 (uint64_t v)
{
    return ((uint64_t) ((uint8_t) v) << 56) | ((v & 0xFF00) << 40)
        | ((v & 0xFF0000) << 24) | ((v & 0xFF000000) << 8)
        | ((v >> 8) & 0xFF000000) | ((v >> 24) & 0xFF0000)
        | ((v >> 40) & 0xFF00) | (v >> 56);
}

#if __GNUC__
#define ZLX_BSWAP16(_v) (__builtin_bswap16(_v))
#define ZLX_BSWAP32(_v) (__builtin_bswap32(_v))
#define ZLX_BSWAP64(_v) (__builtin_bswap64(_v))
#else
/*  ZLX_BSWAP16  */
/**
 *  16-bit byte-swap.
 *  This macro expands to the fastest available function for byte-swapping.
 */
#define ZLX_BSWAP16(_v) (zlx_seqbswap16((_v)))

/*  ZLX_BSWAP32  */
/**
 *  32-bit byte-swap.
 *  This macro expands to the fastest available function for byte-swapping.
 */
#define ZLX_BSWAP32(_v) (zlx_seqbswap32((_v)))

/*  ZLX_BSWAP64  */
/**
 *  64-bit byte-swap.
 *  This macro expands to the fastest available function for byte-swapping.
 */
#define ZLX_BSWAP64(_v) (zlx_seqbswap64((_v)))
#endif

/* macros for reading bytes */
#define ZLX_READ_U8(_p) (*(uint8_t const *) (_p))
#define ZLX_AREAD_U8(_p) (ZLX_READ_U8(_p))
#define ZLX_UREAD_U8(_p) (ZLX_READ_U8(_p))
#define ZLX_AREAD_U8LE(_p) (ZLX_READ_U8(_p))
#define ZLX_UREAD_U8LE(_p) (ZLX_READ_U8(_p))
#define ZLX_AREAD_U8BE(_p) (ZLX_READ_U8(_p))
#define ZLX_UREAD_U8BE(_p) (ZLX_READ_U8(_p))
#define ZLX_AREAD_U8NE(_p) (ZLX_READ_U8(_p))
#define ZLX_UREAD_U8NE(_p) (ZLX_READ_U8(_p))
#define ZLX_AREAD_U8RE(_p) (ZLX_READ_U8(_p))
#define ZLX_UREAD_U8RE(_p) (ZLX_READ_U8(_p))

/* macros for writing bytes */
#define ZLX_WRITE_U8(_p, _v) (*(uint8_t *) (_p) = (_v))
#define ZLX_AWRITE_U8(_p) (ZLX_WRITE_U8(_p))
#define ZLX_UWRITE_U8(_p) (ZLX_WRITE_U8(_p))
#define ZLX_AWRITE_U8LE(_p) (ZLX_WRITE_U8(_p))
#define ZLX_UWRITE_U8LE(_p) (ZLX_WRITE_U8(_p))
#define ZLX_AWRITE_U8BE(_p) (ZLX_WRITE_U8(_p))
#define ZLX_UWRITE_U8BE(_p) (ZLX_WRITE_U8(_p))
#define ZLX_AWRITE_U8NE(_p) (ZLX_WRITE_U8(_p))
#define ZLX_UWRITE_U8NE(_p) (ZLX_WRITE_U8(_p))
#define ZLX_AWRITE_U8RE(_p) (ZLX_WRITE_U8(_p))
#define ZLX_UWRITE_U8RE(_p) (ZLX_WRITE_U8(_p))

/* macros for reading 16-bit ints */
#define ZLX_AREAD_U16NE(_p) (*(uint16_t const *) (_p))
#define ZLX_AREAD_U16RE(_p) (ZLX_BSWAP16(ZLX_AREAD_U16NE(_p)))
#if ZLX_LE
#define ZLX_AREAD_U16LE(_p) (ZLX_AREAD_U16NE(_p))
#define ZLX_AREAD_U16BE(_p) (ZLX_AREAD_U16RE(_p))
#else
#define ZLX_AREAD_U16LE(_p) (ZLX_AREAD_U16RE(_p))
#define ZLX_AREAD_U16BE(_p) (ZLX_AREAD_U16NE(_p))
#endif

ZLX_INLINE uint16_t zlx_seqread_u16le (void const * p)
{
    uint8_t const * b = p;
    return b[0] | ((uint16_t) b[1] << 8);
}

ZLX_INLINE uint16_t zlx_seqread_u16be (void const * p)
{
    uint8_t const * b = p;
    return b[1] | ((uint16_t) b[0] << 8);
}

#if ZLX_UNALIGNED_ACCESS
#define ZLX_UREAD_U16NE(_p) (ZLX_AREAD_U16NE(_p))
#define ZLX_UREAD_U16RE(_p) (ZLX_AREAD_U16RE(_p))
#define ZLX_UREAD_U16LE(_p) (ZLX_AREAD_U16LE(_p))
#define ZLX_UREAD_U16BE(_p) (ZLX_AREAD_U16BE(_p))
#else
#define ZLX_UREAD_U16LE(_p) (zlx_seqread_u16le((_p)))
#define ZLX_UREAD_U16BE(_p) (zlx_seqread_u16be((_p)))
#if ZLX_LE
#define ZLX_UREAD_U16NE(_p) (ZLX_UREAD_U16LE(_p))
#define ZLX_UREAD_U16RE(_p) (ZLX_UREAD_U16BE(_p))
#else /* big endian */
#define ZLX_UREAD_U16NE(_p) (ZLX_UREAD_U16BE(_p))
#define ZLX_UREAD_U16RE(_p) (ZLX_UREAD_U16LE(_p))
#endif
#endif

/* macros for writing 16-bit ints */
#define ZLX_AWRITE_U16NE(_p, _v) (*(uint16_t *) (_p) = (_v))
#define ZLX_AWRITE_U16RE(_p, _v) (*(uint16_t *) (_p) = ZLX_BSWAP16(_v))
#if ZLX_LE
#define ZLX_AWRITE_U16LE(_p, _v) (ZLX_AWRITE_U16NE((_p), (_v)))
#define ZLX_AWRITE_U16BE(_p, _v) (ZLX_AWRITE_U16RE((_p), (_v)))
#else
#define ZLX_AWRITE_U16LE(_p, _v) (ZLX_AWRITE_U16RE((_p), (_v)))
#define ZLX_AWRITE_U16BE(_p, _v) (ZLX_AWRITE_U16NE((_p), (_v)))
#endif

ZLX_INLINE void zlx_seqwrite_u16le (void * p, uint16_t v)
{
    uint8_t * b = p;
    b[0] = (uint8_t) v;
    b[1] = (uint8_t) (v >> 8);
}

ZLX_INLINE void zlx_seqwrite_u16be (void * p, uint16_t v)
{
    uint8_t * b = p;
    b[0] = (uint8_t) (v >> 8);
    b[1] = (uint8_t) v;
}

#if ZLX_UNALIGNED_ACCESS
#define ZLX_UWRITE_U16NE(_p, _v) (ZLX_AWRITE_U16NE((_p), (_v)))
#define ZLX_UWRITE_U16RE(_p, _v) (ZLX_AWRITE_U16RE((_p), (_v)))
#define ZLX_UWRITE_U16LE(_p, _v) (ZLX_AWRITE_U16LE((_p), (_v)))
#define ZLX_UWRITE_U16BE(_p, _v) (ZLX_AWRITE_U16BE((_p), (_v)))
#else
#define ZLX_UWRITE_U16LE(_p, _v) (zlx_seqwrite_u16le((_p), (_v)))
#define ZLX_UWRITE_U16BE(_p, _v) (zlx_seqwrite_u16be((_p), (_v)))
#if ZLX_LE
#define ZLX_UWRITE_U16NE(_p, _v) (ZLX_UWRITE_U16LE(((_p), (_v))))
#define ZLX_UWRITE_U16RE(_p, _v) (ZLX_UWRITE_U16BE(((_p), (_v))))
#else /* big endian */
#define ZLX_UWRITE_U16NE(_p, _v) (ZLX_UWRITE_U16BE(((_p), (_v))))
#define ZLX_UWRITE_U16RE(_p, _v) (ZLX_UWRITE_U16LE(((_p), (_v))))
#endif
#endif

/* macros for reading 32-bit ints */
#define ZLX_AREAD_U32NE(_p) (*(uint32_t const *) (_p))
#define ZLX_AREAD_U32RE(_p) (ZLX_BSWAP32(ZLX_AREAD_U32NE(_p)))
#if ZLX_LE
#define ZLX_AREAD_U32LE(_p) (ZLX_AREAD_U32NE(_p))
#define ZLX_AREAD_U32BE(_p) (ZLX_AREAD_U32RE(_p))
#else
#define ZLX_AREAD_U32LE(_p) (ZLX_AREAD_U32RE(_p))
#define ZLX_AREAD_U32BE(_p) (ZLX_AREAD_U32NE(_p))
#endif

ZLX_INLINE uint32_t zlx_seqread_u32le (void const * p)
{
    uint8_t const * b = p;
    return b[0] | ((uint32_t) b[1] << 8) |
        ((uint32_t) b[2] << 16) | ((uint32_t) b[3] << 24);
}

ZLX_INLINE uint32_t zlx_seqread_u32be (void const * p)
{
    uint8_t const * b = p;
    return b[3] | ((uint32_t) b[2] << 8) |
        ((uint32_t) b[1] << 16) | ((uint32_t) b[0] << 24);
}

#if ZLX_UNALIGNED_ACCESS
#define ZLX_UREAD_U32NE(_p) (ZLX_AREAD_U32NE(_p))
#define ZLX_UREAD_U32RE(_p) (ZLX_AREAD_U32RE(_p))
#define ZLX_UREAD_U32LE(_p) (ZLX_AREAD_U32LE(_p))
#define ZLX_UREAD_U32BE(_p) (ZLX_AREAD_U32BE(_p))
#else
#define ZLX_UREAD_U32LE(_p) (zlx_seqread_u32le((_p)))
#define ZLX_UREAD_U32BE(_p) (zlx_seqread_u32be((_p)))
#if ZLX_LE
#define ZLX_UREAD_U32NE(_p) (ZLX_UREAD_U32LE(_p))
#define ZLX_UREAD_U32RE(_p) (ZLX_UREAD_U32BE(_p))
#else /* big endian */
#define ZLX_UREAD_U32NE(_p) (ZLX_UREAD_U32BE(_p))
#define ZLX_UREAD_U32RE(_p) (ZLX_UREAD_U32LE(_p))
#endif
#endif

/* macros for writing 32-bit ints */
#define ZLX_AWRITE_U32NE(_p, _v) (*(uint32_t *) (_p) = (_v))
#define ZLX_AWRITE_U32RE(_p, _v) (*(uint32_t *) (_p) = ZLX_BSWAP32(_v))
#if ZLX_LE
#define ZLX_AWRITE_U32LE(_p, _v) (ZLX_AWRITE_U32NE((_p), (_v)))
#define ZLX_AWRITE_U32BE(_p, _v) (ZLX_AWRITE_U32RE((_p), (_v)))
#else
#define ZLX_AWRITE_U32LE(_p, _v) (ZLX_AWRITE_U32RE((_p), (_v)))
#define ZLX_AWRITE_U32BE(_p, _v) (ZLX_AWRITE_U32NE((_p), (_v)))
#endif

ZLX_INLINE void zlx_seqwrite_u32le (void * p, uint32_t v)
{
    uint8_t * b = p;
    b[0] = (uint8_t) v;
    b[1] = (uint8_t) (v >> 8);
    b[2] = (uint8_t) (v >> 16);
    b[3] = (uint8_t) (v >> 24);
}

ZLX_INLINE void zlx_seqwrite_u32be (void * p, uint32_t v)
{
    uint8_t * b = p;
    b[0] = (uint8_t) (v >> 24);
    b[1] = (uint8_t) (v >> 16);
    b[2] = (uint8_t) (v >> 8);
    b[3] = (uint8_t) v;
}

#if ZLX_UNALIGNED_ACCESS
#define ZLX_UWRITE_U32NE(_p, _v) (ZLX_AWRITE_U32NE((_p), (_v)))
#define ZLX_UWRITE_U32RE(_p, _v) (ZLX_AWRITE_U32RE((_p), (_v)))
#define ZLX_UWRITE_U32LE(_p, _v) (ZLX_AWRITE_U32LE((_p), (_v)))
#define ZLX_UWRITE_U32BE(_p, _v) (ZLX_AWRITE_U32BE((_p), (_v)))
#else
#define ZLX_UWRITE_U32LE(_p, _v) (zlx_seqwrite_u32le((_p), (_v)))
#define ZLX_UWRITE_U32BE(_p, _v) (zlx_seqwrite_u32be((_p), (_v)))
#if ZLX_LE
#define ZLX_UWRITE_U32NE(_p, _v) (ZLX_UWRITE_U32LE(((_p), (_v))))
#define ZLX_UWRITE_U32RE(_p, _v) (ZLX_UWRITE_U32BE(((_p), (_v))))
#else /* big endian */
#define ZLX_UWRITE_U32NE(_p, _v) (ZLX_UWRITE_U32BE(((_p), (_v))))
#define ZLX_UWRITE_U32RE(_p, _v) (ZLX_UWRITE_U32LE(((_p), (_v))))
#endif
#endif

/* macros for reading 64-bit ints */
#define ZLX_AREAD_U64NE(_p) (*(uint64_t const *) (_p))
#define ZLX_AREAD_U64RE(_p) (ZLX_BSWAP64(ZLX_AREAD_U64NE(_p)))
#if ZLX_LE
#define ZLX_AREAD_U64LE(_p) (ZLX_AREAD_U64NE(_p))
#define ZLX_AREAD_U64BE(_p) (ZLX_AREAD_U64RE(_p))
#else
#define ZLX_AREAD_U64LE(_p) (ZLX_AREAD_U64RE(_p))
#define ZLX_AREAD_U64BE(_p) (ZLX_AREAD_U64NE(_p))
#endif

ZLX_INLINE uint64_t zlx_seqread_u64le (void const * p)
{
    uint8_t const * b = p;
    return b[0] | ((uint64_t) b[1] << 8) |
        ((uint64_t) b[2] << 16) | ((uint64_t) b[3] << 24) |
        ((uint64_t) b[4] << 32) | ((uint64_t) b[5] << 40) |
        ((uint64_t) b[6] << 48) | ((uint64_t) b[7] << 56);
}

ZLX_INLINE uint64_t zlx_seqread_u64be (void const * p)
{
    uint8_t const * b = p;
    return b[7] | ((uint64_t) b[6] << 8) |
        ((uint64_t) b[5] << 16) | ((uint64_t) b[4] << 24) |
        ((uint64_t) b[3] << 32) | ((uint64_t) b[2] << 40) |
        ((uint64_t) b[1] << 48) | ((uint64_t) b[0] << 56);
}

#if ZLX_UNALIGNED_ACCESS
#define ZLX_UREAD_U64NE(_p) (ZLX_AREAD_U64NE(_p))
#define ZLX_UREAD_U64RE(_p) (ZLX_AREAD_U64RE(_p))
#define ZLX_UREAD_U64LE(_p) (ZLX_AREAD_U64LE(_p))
#define ZLX_UREAD_U64BE(_p) (ZLX_AREAD_U64BE(_p))
#else
#define ZLX_UREAD_U64LE(_p) (zlx_seqread_u64le((_p)))
#define ZLX_UREAD_U64BE(_p) (zlx_seqread_u64be((_p)))
#if ZLX_LE
#define ZLX_UREAD_U64NE(_p) (ZLX_UREAD_U64LE(_p))
#define ZLX_UREAD_U64RE(_p) (ZLX_UREAD_U64BE(_p))
#else /* big endian */
#define ZLX_UREAD_U64NE(_p) (ZLX_UREAD_U64BE(_p))
#define ZLX_UREAD_U64RE(_p) (ZLX_UREAD_U64LE(_p))
#endif
#endif

/* macros for writing 64-bit ints */
#define ZLX_AWRITE_U64NE(_p, _v) (*(uint64_t *) (_p) = (_v))
#define ZLX_AWRITE_U64RE(_p, _v) (*(uint64_t *) (_p) = ZLX_BSWAP64(_v))
#if ZLX_LE
#define ZLX_AWRITE_U64LE(_p, _v) (ZLX_AWRITE_U64NE((_p), (_v)))
#define ZLX_AWRITE_U64BE(_p, _v) (ZLX_AWRITE_U64RE((_p), (_v)))
#else
#define ZLX_AWRITE_U64LE(_p, _v) (ZLX_AWRITE_U64RE((_p), (_v)))
#define ZLX_AWRITE_U64BE(_p, _v) (ZLX_AWRITE_U64NE((_p), (_v)))
#endif

ZLX_INLINE void zlx_seqwrite_u64le (void * p, uint64_t v)
{
    uint8_t * b = p;
    b[0] = (uint8_t) v;
    b[1] = (uint8_t) (v >> 8);
    b[2] = (uint8_t) (v >> 16);
    b[3] = (uint8_t) (v >> 24);
    b[4] = (uint8_t) (v >> 32);
    b[5] = (uint8_t) (v >> 40);
    b[6] = (uint8_t) (v >> 48);
    b[7] = (uint8_t) (v >> 56);
}

ZLX_INLINE void zlx_seqwrite_u64be (void * p, uint64_t v)
{
    uint8_t * b = p;
    b[0] = (uint8_t) (v >> 56);
    b[1] = (uint8_t) (v >> 48);
    b[2] = (uint8_t) (v >> 40);
    b[3] = (uint8_t) (v >> 32);
    b[4] = (uint8_t) (v >> 24);
    b[5] = (uint8_t) (v >> 16);
    b[6] = (uint8_t) (v >> 8);
    b[7] = (uint8_t) v;
}

#if ZLX_UNALIGNED_ACCESS
#define ZLX_UWRITE_U64NE(_p, _v) (ZLX_AWRITE_U64NE((_p), (_v)))
#define ZLX_UWRITE_U64RE(_p, _v) (ZLX_AWRITE_U64RE((_p), (_v)))
#define ZLX_UWRITE_U64LE(_p, _v) (ZLX_AWRITE_U64LE((_p), (_v)))
#define ZLX_UWRITE_U64BE(_p, _v) (ZLX_AWRITE_U64BE((_p), (_v)))
#else
#define ZLX_UWRITE_U64LE(_p, _v) (zlx_seqwrite_u64le((_p), (_v)))
#define ZLX_UWRITE_U64BE(_p, _v) (zlx_seqwrite_u64be((_p), (_v)))
#if ZLX_LE
#define ZLX_UWRITE_U64NE(_p, _v) (ZLX_UWRITE_U64LE(((_p), (_v))))
#define ZLX_UWRITE_U64RE(_p, _v) (ZLX_UWRITE_U64BE(((_p), (_v))))
#else /* big endian */
#define ZLX_UWRITE_U64NE(_p, _v) (ZLX_UWRITE_U64BE(((_p), (_v))))
#define ZLX_UWRITE_U64RE(_p, _v) (ZLX_UWRITE_U64LE(((_p), (_v))))
#endif
#endif

/** @} */

#endif

