#ifndef _ZLX_BASE_H
#define _ZLX_BASE_H

#include <limits.h>
#include <stdarg.h>
#include <stddef.h>

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

#define ZLX_INLINE static __inline

#if _WIN32 && !_WIN64
#define ZLX_CALL __fastcall
#else
#define ZLX_CALL
#endif

#ifdef _ISOC99_SOURCE
#define ZLX_RESTRICT restrict
#elif _MSC_VER >= 1600
#define ZLX_RESTRICT __restrict
#elif __GNUC__ >= 3
#define ZLX_RESTRICT __restrict__
#else
#define ZLX_RESTRICT
#endif

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


#endif

