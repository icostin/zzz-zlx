#include "zlx/stdarray.h"
#include "zlx/fmt.h"
#include "zlx/clconv.h"

static uint32_t cet[8] =
{
    0xFFFFFFFF, 0x00000084, 0x10000000, 0x00000000,
    0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
};

static char ec[] = "abtnvfr";

/* byte_c_escape_len ********************************************************/
static unsigned int byte_c_escape_len
(
    uint8_t v
)
{
    return (cet[v >> 5] & (1 << (v & 31))) == 0 ? 1
        : ((v >= 7 && v <= 13) || v >= 32 ? 2: 4);
}

/* zlx_clconv_bin_to_hex_line ***********************************************/
ZLX_API uint_fast8_t ZLX_CALL zlx_clconv_bin_to_hex_line
(
    uint8_t const * in,
    size_t in_len,
    size_t * in_used_len,
    uint8_t * out,
    size_t out_len,
    size_t * out_used_len,
    void * ctx
)
{
    size_t i, o, ol;
    (void) ctx;
    ol = out_len & -2;
    for (i = o = 0; i < in_len && o < ol; i++)
    {
        out[o++] = zlx_digit_char_table[in[i] >> 4];
        out[o++] = zlx_digit_char_table[in[i] & 15];
    }
    *in_used_len = i;
    *out_used_len = o;
    return (i == in_len) ? ZLX_CLCONV_OK : ZLX_CLCONV_FULL;
}

/* zlx_clconv_hex_to_bin ****************************************************/
ZLX_API uint_fast8_t ZLX_CALL zlx_clconv_hex_to_bin
(
    uint8_t const * in,
    size_t in_len,
    size_t * in_used_len,
    uint8_t * out,
    size_t out_len,
    size_t * out_used_len,
    void * ctx
)
{
    uint8_t const * skip_list = ctx;
    size_t i, o;
    uint_fast8_t r;

    for (r = 0, i = o = 0; i < in_len; i++)
    {
        int lo, hi = zlx_digit_from_char(in[i], 16);
        if (hi < 0)
        {
            if (skip_list && zlx_u8a_scan(skip_list, in[i])) continue;
            r = ZLX_CLCONV_MALFORMED;
            break;
        }
        if (++i == in_len)
        {
            r = ZLX_CLCONV_INCOMPLETE;
            --i;
            break;
        }
        lo = zlx_digit_from_char(in[i], 16);
        if (lo < 0)
        {
            r = ZLX_CLCONV_MALFORMED;
            break;
        }
        if (o == out_len)
        {
            r = ZLX_CLCONV_FULL;
            --i;
            break;
        }
        out[o++] = (hi << 4) | lo;
    }
    *in_used_len = i;
    *out_used_len = o;
    return r;
}

/* zlx_clconv_c_escape_force_hex ********************************************/
ZLX_API uint_fast8_t ZLX_CALL zlx_clconv_c_escape_force_hex
(
    uint8_t const * in,
    size_t in_len,
    size_t * in_used_len,
    uint8_t * out,
    size_t out_len,
    size_t * out_used_len,
    void * ctx
)
{
    size_t i, o;
    uint_fast8_t r;
    (void) ctx;
    for (r = ZLX_CLCONV_OK, i = o = 0; i < in_len; ++i)
    {
        uint8_t v = in[i];
        unsigned int cl = byte_c_escape_len(v);
        if (o + cl > out_len) { r = ZLX_CLCONV_FULL; break; }
        if (cl == 1) out[o++] = v;
        else
        {
            out[o++] = '\\';

            if (cl == 4)
            {
                out[o++] = 'x';
                out[o++] = zlx_digit_char_table[v >> 4];
                out[o++] = zlx_digit_char_table[v & 15];
            }
            else if (v >= 7 && v <= 13) out[o++] = ec[v - 7];
            else out[o++] = v;
        }
    }
    *in_used_len = i;
    *out_used_len = o;
    return r;
}

/* zlx_clconv_c_escape_init *************************************************/
ZLX_API void * ZLX_CALL zlx_clconv_c_escape_init
(
    zlx_clconv_c_escape_t * ctx
)
{
    ctx->crt_byte = -1;
    return ctx;
}

/* zlx_clconv_c_escape ******************************************************/
ZLX_API uint_fast8_t ZLX_CALL zlx_clconv_c_escape
(
    uint8_t const * in,
    size_t in_len,
    size_t * in_used_len,
    uint8_t * out,
    size_t out_len,
    size_t * out_used_len,
    void * ctx
)
{
    zlx_clconv_c_escape_t * cectx = ctx;
    int v = cectx->crt_byte;
    size_t i, o;
    uint_fast8_t r;

    if (in)
    {
        if (in_len == 0) return 0;
        i = o = 0;
        if (v < 0) v = in[i++];
        for (r = 0;; v = in[i++])
        {
            if (cet[v >> 5] & (1 << (v & 31)))
            {
                // escaping needed
                if (v < 7 || (v >= 14 && v < 32) || v >= 0x7F)
                {
                    // should escape with hex or octal form
                    if (o + 4 > out_len) { r = ZLX_CLCONV_FULL; break; }
                    if (i == in_len) { r = ZLX_CLCONV_OK; break; }
                    out[o++] = '\\';
                    if (zlx_digit_from_char(in[i], 16) < 0)
                    {
                        out[o++] = 'x';
                        out[o++] = zlx_digit_char_table[v >> 4];
                        out[o++] = zlx_digit_char_table[v & 15];
                    }
                    else
                    {
                        out[o++] = zlx_digit_char_table[v >> 6];
                        out[o++] = zlx_digit_char_table[(v >> 3) & 7];
                        out[o++] = zlx_digit_char_table[v & 7];
                    }
                }
                else
                {
                    // short escape
                    if (o + 2 > out_len) { r = ZLX_CLCONV_FULL; break; }
                    out[o++] = '\\';
                    out[o++] = v < 32 ? ec[v - 7] : v;
                    if (i == in_len) { v = -1; r = ZLX_CLCONV_FULL; break; }
                }
            }
            else
            {
                // non-escaped char
                if (o == out_len) { r = ZLX_CLCONV_FULL; break; }
                out[o++] = v;
                if (i == in_len) { v = -1; r = ZLX_CLCONV_FULL; break; }
            }
        }
        cectx->crt_byte = v;
        *in_used_len = i;
        *out_used_len = o;
        return r;
    }
    else
    {
        if (v == -1)
        {
            *out_used_len = 0;
            return 0;
        }
        if (out_len < 4) return ZLX_CLCONV_FULL;
        out[0] = '\\';
        out[1] = 'x';
        out[2] = zlx_digit_char_table[v >> 4];
        out[3] = zlx_digit_char_table[v & 15];
        *out_used_len = 4;
        return 0;
    }
}


