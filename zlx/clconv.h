#ifndef _ZLX_CLCONV_H
#define _ZLX_CLCONV_H

#include "base.h"

#define ZLX_CLCONV_OK 0
/**< entire input used (return code for #zlx_clconv_func_t) */

#define ZLX_CLCONV_MALFORMED 1
/**< malformed input; error code for #zlx_clconv_func_t */

#define ZLX_CLCONV_INCOMPLETE 2
/**< input needs more data for decoding a valid item */

#define ZLX_CLCONV_FULL 3
/**< output full (error code for #zlx_clconv_func_t) */

/* zlx_clconv_func_t ********************************************************/
/**
 *  Chunked linear converter function.
 *  Implementations must consume as much as possible from input, either
 *  until the entire input is used, either until the output buffer cannot
 *  hold an extra item.
 *  Param @a ctx can be used to store the state if the conversion requires
 *  keeping a state.
 *  After feeding the entire desired input to the converter, users should
 *  call this function again with @a in set to NULL and @a in_len set to 0
 *  to tell the converter to append any terminating sequence.
 */
typedef uint_fast8_t (ZLX_CALL * zlx_clconv_func_t)
    (
        uint8_t const * in,
        size_t in_len,
        size_t * in_used_len,
        uint8_t * out,
        size_t out_len,
        size_t * out_used_len,
        void * ctx
    );

/* zlx_clconv_hex_to_bin ****************************************************/
/**
 *  Hex to bin converter.
 *  @param [in] in input data
 *  @param [in] in_len length of input data
 *  @param [out] in_used_len will be filled in with length of input data
 *              processed
 *  @param [in] out output buffer
 *  @param [in] out_len size of output buffer
 *  @param [out] out_used_len   will be filled in with length of output produced
 *  @param [in] ctx this can be NULL for strict conversion or it should
 *      point to a NUL-terminated byte string containing bytes to skip
 */
ZLX_API uint_fast8_t ZLX_CALL zlx_clconv_hex_to_bin
(
    uint8_t const * in,
    size_t in_len,
    size_t * in_used_len,
    uint8_t * out,
    size_t out_len,
    size_t * out_used_len,
    void * ctx
);

/* zlx_clconv_bin_to_hex_line ***********************************************/
/**
 *  Bin to hex (all output on one line without any whitespace).
 *  @param [in] in
 *      input data
 *  @param [in] in_len
 *      length of input data
 *  @param [out] in_used_len
 *      will be filled in with length of input data processed
 *  @param [in] out
 *      output buffer
 *  @param [in] out_len
 *      size of output buffer
 *  @param [out] out_used_len
 *      will be filled in with length of output produced
 *  @param ctx
 *      this should be NULL
 */
ZLX_API uint_fast8_t ZLX_CALL zlx_clconv_bin_to_hex_line
(
    uint8_t const * in,
    size_t in_len,
    size_t * in_used_len,
    uint8_t * out,
    size_t out_len,
    size_t * out_used_len,
    void * ctx
);

/* zlx_clconv_c_escape_force_hex ********************************************/
/**
 *  C-Escapes a byte array forcing 2 hex digit '\\x' escapes even when
 *  generating bad string literals.
 *  @param [in] in
 *      input data
 *  @param [in] in_len
 *      length of input data
 *  @param [out] in_used_len
 *      will be filled in with length of input data processed
 *  @param [in] out
 *      output buffer
 *  @param [in] out_len
 *      size of output buffer
 *  @param [out] out_used_len
 *      will be filled in with length of output produced
 *  @param ctx
 *      NULL
 *  @warning This does not produce strings conforming to the C standard;
 *      AB34 will be converted to "\\xAB4" which according to C is a 1 char
 *      string (or invalid literal when char has under 12 bits).
 *      This is the same way python is understanding literals (\\x is always
 *      followed by exactly to hex digits)
 */
ZLX_API uint_fast8_t ZLX_CALL zlx_clconv_c_escape_force_hex
(
    uint8_t const * in,
    size_t in_len,
    size_t * in_used_len,
    uint8_t * out,
    size_t out_len,
    size_t * out_used_len,
    void * ctx
);

/* zlx_clconv_c_escape_t ****************************************************/
/**
 *  Context structure for zlx_clconv_c_escape().
 */
typedef struct zlx_clconv_c_escape_s zlx_clconv_c_escape_t;
struct zlx_clconv_c_escape_s
{
    int crt_byte; /**< internal field. */
};

/* zlx_clconv_c_escape_init *************************************************/
/**
 *  Inits the c-escape converter context.
 */
ZLX_API void * ZLX_CALL zlx_clconv_c_escape_init
(
    zlx_clconv_c_escape_t * ctx
);

/* zlx_clconv_c_escape ******************************************************/
/**
 *  C-style escape.
 */
ZLX_API uint_fast8_t ZLX_CALL zlx_clconv_c_escape
(
    uint8_t const * in,
    size_t in_len,
    size_t * in_used_len,
    uint8_t * out,
    size_t out_len,
    size_t * out_used_len,
    void * ctx
);


#endif /* _ZLX_CLCONV_H */
