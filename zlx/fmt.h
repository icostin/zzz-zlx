#ifndef _ZLX_FMT_H
#define _ZLX_FMT_H

#include "base.h"
#include "writer.h"

/** @defgroup fmt Formatted printing
 *  @{
 */

ZLX_API char const zlx_digit_char_table[37];

/* zlx_digit_from_char ******************************************************/
/**
 *  Decodes the digit from a single ASCII/Unicode character
 *  @returns digit value
 *  @retval -1 character is not a valid digit
 */
ZLX_INLINE int zlx_digit_from_char (uint32_t ch, uint_fast8_t radix)
{
    unsigned int digit;
    if (ch >= '0' && ch <= '9') digit = ch - '0';
    else
    {
        ch |= 0x20;
        if (ch >= 'a' && ch <= 'z') digit = ch - 'a' + 10;
        else return -1;
    }
    if (digit >= radix) return -1;
    return digit;
}

/* zlx_u64_to_str ***********************************************************/
/**
 *  Converts the given unsigned 64-bit int to ASCII NUL-terminated string.
 *  @param [out]    str     output buffer; caller must ensure is large enough
 *                          for the result
 *  @param [in]     value   value to convert
 *  @param [in]     radix   numeration base; must be in range 2 - 36
 *  @param [in]     width   desired width of the output; this is a guaranteed
 *                          minimum size for the output, not a maximum;
 *                          if the value is large it may cause the output to
 *                          exceed this limit
 *  @param [in]     group   how many digits to group together before using a
 *                          separator char; use 64 or above to disable grouping
 *  @param [in]     sep     separator char
 *  @returns        N       number of chars outputted, not including the
 *                          NUL terminator
 *  @note on DEBUG builds if radix is not in the valid range it will output
 *  the string "BAD_RADIX"
 */
ZLX_API size_t ZLX_CALL zlx_u64_to_str
(
    uint8_t * str,
    uint64_t value,
    uint_fast8_t radix,
    uint32_t width,
    uint_fast8_t group,
    uint_fast8_t sep
);

#define ZLX_NO_SIGN 0
    /**< consider the value unsigned (flag for zlx_i64_to_str#sign_mode) */

#define ZLX_SIGN_NEG 1
    /**< only put the sign if the value is negative
     *  (flag for zlx_i64_to_str#sign_mode) */

#define ZLX_SIGN_ALWAYS 2
    /**< use '-' for negative, space for 0 and + for positive numbers
     *  (flag for zlx_i64_to_str#sign_mode) */

#define ZLX_SIGN_ALIGN 3
    /**< use '-' for negative, space for non-negative numbers
     *  (flag for zlx_i64_to_str#sign_mode) */

/* zlx_i64_to_str ***********************************************************/
/**
 *  Converts the given int to ASCII string.
 *  @param [out]    str     output buffer; caller must ensure is large enough
 *                          for the result
 *  @param [in]     value   value to convert
 *  @param [in]     sign_mode   one of: #ZLX_NO_SIGN, #ZLX_SIGN_NEG,
 *                          ZLX_SIGN_ALWAYS, #ZLX_SIGN_ALIGN
 *  @param [in]     radix   numeration base; must be in range 2 - 36
 *  @param [in]     prefix  NUL-terminated C-string to be put in front of the
 *                          digits (after the sign, if any); can be NULL
 *  @param [in]     width   desired width of the output; this is a guaranteed
 *                          minimum size for the output, not a maximum;
 *                          if the value is large it may cause the output to
 *                          exceed this limit
 *  @param [in]     group   how many digits to group together before using a
 *                          separator char; use 64 or above to disable grouping
 *  @param [in]     sep     separator char
 *  @returns                number of chars outputted, not including the
 *                          NUL terminator
 */
ZLX_API size_t ZLX_CALL zlx_i64_to_str
(
    uint8_t * str,
    int64_t value,
    uint_fast8_t sign_mode,
    uint_fast8_t radix,
    uint8_t const * prefix,
    uint32_t width,
    uint_fast8_t group,
    uint_fast8_t sep
);

#define ZLX_U64_STOP 1 /**< non-digit encountered */
#define ZLX_U64_OVERFLOW 2 /**< number parsed exceeds uint64_t capacity */

/* zlx_u64_from_str *********************************************************/
/**
 *  Converts a string to unsigned int.
 *  @param [in]     str     pointer to data
 *  @param [in]     len     length of input string
 *  @param [in]     radix   numeration base; 0 is magic for autodetect
 *  @param [out]    value   will be filled with the parsed value
 *  @param [out]    used_len filled with number of bytes parsed; can be NULL
 *  @retval 0 parsing successful, full length used
 *  @retval ZLX_U64_STOP non-digit encountered
 *  @retval ZLX_U64_OVERFLOW
 */
ZLX_API uint_fast8_t ZLX_CALL zlx_u64_from_str
(
    uint8_t const * str,
    size_t len,
    uint_fast8_t radix,
    uint64_t * value,
    size_t * used_len
);

#define ZLX_FMT_MALFORMED 1 /**< bad format string error code */
#define ZLX_FMT_WIDTH_ERROR 2 /**< width function returned error */
#define ZLX_FMT_WRITE_ERROR 3 /**< write error */
#define ZLX_FMT_CONV_ERROR 4 /**< conversion error during escaping of some string */
#define ZLX_FMT_NO_CODE 5 /**< feature not implemented */

/* zlx_vfmt *****************************************************************/
/**
 *  Writes formatted UTF-8 text (similar to printf formatting).
 *  @retval 0 success
 *  @retval ZLX_FMT_MALFORMED bad format string
 *  @retval ZLX_FMT_WIDTH_ERROR
 *  @retval ZLX_FMT_WRITE_ERROR
 *  Formatting: "$" [[ALIGN] WIDTH] ["." PREC] ["/" GROUP_LEN] [MOD] TYPE
 *  TYPE:
 *  * 'b': unsigned 8-bit int (byte)
 *  * 'B': signed 8-bit int (char)
 *  * 'w': unsigned 16-bit int
 *  * 'W': signed 16-bit int
 *  * 'd': unsigned 32-bit int
 *  * 'D': signed 32-bit int
 *  * 'q': unsigned 64-bit int
 *  * 'Q': signed 64-bit int
 *  * 'i': unsigned int
 *  * 'I': signed int
 *  * 'l': unsigned long
 *  * 'L': signed long
 *  * 'h': unsigned short
 *  * 'H': signed short
 *  * 'z': size_t
 *  * 'Z': ptrdiff_t
 *  * 'p': uintptr_t
 *  * 'P': intptr_t
 *  * 'c': Unicode codepoint (32-bit int between 0-0x10FFFF minus 0xD800-0xDFFF)
 *  * 's': string
 *  ALIGN:
 *  * '<' align left
 *  * '>' align right
 *  MOD:
 *  * 'e': escapes strings C-style
 *  * 'y': radix 2 (binary) and use '0b' prefix
 *  * 'Y': radix 2 (binary)
 *  * 'o': radix 8 (octal) and use '0o' prefix
 *  * 'O': radix 8 (octal)
 *  * 'n': radix 10 (decimal/normal) and use '0d' prefix
 *  * 'N': radix 10
 *  * 'x': radix 16 (hexadecimal) and use '0x' prefix
 *  * 'X': radix 16 (hexadecimal)
 */
ZLX_API unsigned int ZLX_CALL zlx_vfmt
(
    zlx_write_func_t writer,
    void * writer_context,
    zlx_write_func_t width_func,
    void * width_context,
    char const * fmt,
    va_list va
);

/* zlx_fmt ******************************************************************/
/**
 *  Writes formatted UTF-8 text.
 *  See zlx_vfmt.
 */
ZLX_API unsigned int ZLX_CALL zlx_fmt
(
    zlx_write_func_t writer,
    void * writer_context,
    zlx_write_func_t width_func,
    void * width_context,
    char const * fmt,
    ...
);

/** @} */

#endif

