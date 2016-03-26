#include "zlx/unicode.h"

/* zlx_utf16le_to_utf8_len **************************************************/
ZLX_API ptrdiff_t ZLX_CALL zlx_utf16le_to_utf8_len
(
    uint8_t const * data,
    size_t len,
    unsigned int flags
)
{
    uint32_t ucp;
    ptrdiff_t o;
    uint8_t const * end = data + len;
    if ((len & 1) != 0) return -1;
    for (o = 0; data < end && o >= 0; )
    {
        ucp = *data++;
        ucp |= (*data++) << 8;
        if ((ucp & 0xF800) == 0xD800)
        {
            /* surrogate code point */
            if ((flags & ZLX_UTF16_DEC_NO_PAIRING)) o += 3;
            else if (ucp < 0xDC00 && data < end && (data[1] & 0xFC) == 0xDC)
            {
                o += (flags & ZLX_UTF8_ENC_BREAK_SUPPLEM) ? 6 : 4;
                data += 2;
            }
            else
            {
                /* hi-value surrogate in lead position - invalid UTF-16 */
                if ((flags & ZLX_UTF16_DEC_UNPAIRED_SURROGATES) == 0) return -2;
                o += 3;
            }
        }
        else
        {
            /* non-surrogate code point */
            o += ucp >= 0x800 || (!ucp && (flags & ZLX_UTF8_ENC_TWO_BYTE_NUL)) ? 2 : 1;
        }
    }
    return (o >= 0) ? o : -3;
}

/* zlx_utf16le_to_utf8 ******************************************************/
ZLX_API uint8_t * ZLX_CALL zlx_utf16le_to_utf8
(
    uint8_t const * data,
    size_t len,
    unsigned int flags,
    uint8_t * out
)
{
    uint32_t ucp;
    uint8_t const * end = data + len;
    for (; data < end; )
    {
        ucp = *data++;
        ucp |= (uint32_t) (*data++) << 8;
        if ((ucp & 0xFC00) == 0xD800
            && !(flags & ZLX_UTF16_DEC_NO_PAIRING)
            && data < end
            && (data[1] & 0xFC) == 0xDC)
        {
            ucp = (ucp & 0x3FF) << 10;
            ucp |= *data++;
            ucp |= (*data++ & 3) << 8;
            ucp += 0x10000;
        }
        else if (ucp == 0 && (flags & ZLX_UTF8_ENC_TWO_BYTE_NUL))
        {
            *out++ = 0xC0;
            *out++ = 0x80;
            continue;
        }
        out += zlxi_ucp_to_utf8(ucp, out);
    }
    return out;
}

/* zlx_utf32le_to_ucp *******************************************************/
ZLX_API ptrdiff_t ZLX_CALL zlx_utf32le_to_ucp
(
    uint8_t const * in,
    uint8_t const * end,
    unsigned int flags,
    uint32_t * ZLX_RESTRICT out
)
{
    uint32_t ucp;
    if (end - in < 4) return ZLX_UTF_ERR_TRUNC;
    ucp = ZLX_UREAD_U32LE(in);
    if ((ucp & 0xF800) == 0xD800 && !(flags & ZLX_UTF32_DEC_SURROGATES))
        return ZLX_UTF_ERR_SURROGATE;
    *out = ucp;
    return 4;
}

/* zlx_utf16le_to_ucp *******************************************************/
ZLX_API ptrdiff_t ZLX_CALL zlx_utf16le_to_ucp
(
    uint8_t const * in,
    uint8_t const * end,
    unsigned int flags,
    uint32_t * ZLX_RESTRICT out
)
{
    uint32_t ucp, tcp;
    if (end - in < 2) return ZLX_UTF_ERR_TRUNC;
    ucp = ZLX_UREAD_U16LE(in);
    if ((ucp & 0xF800) == 0xD800)
    {
        /* surrogate */
        if (ucp < 0xDC00)
        {
            /* lead surrogate */
            if (!(flags & ZLX_UTF16_DEC_NO_PAIRING))
            {
                if (end - in < 4) return ZLX_UTF_ERR_TRUNC;
                tcp = ZLX_UREAD_U16LE(in + 2);
                if ((tcp & 0xFC00) == 0xDC00)
                {
                    *out = 0x10000 + ((ucp & 0x3FF) << 10) + (tcp & 0x3FF);
                    return 4;
                }
                else if (!(flags & ZLX_UTF16_DEC_UNPAIRED_SURROGATES))
                    return ZLX_UTF_ERR_SURROGATE;
            }
        }
        else
        {
            /* tail surrogate */
            if (!(flags & ZLX_UTF16_DEC_UNPAIRED_SURROGATES))
                return ZLX_UTF_ERR_SURROGATE;
        }
    }
    *out = ucp;
    return 2;
}

/* zlx_utf8_to_ucp **********************************************************/
ZLX_API ptrdiff_t ZLX_CALL zlx_utf8_to_ucp
(
    uint8_t const * in,
    uint8_t const * end,
    unsigned int flags,
    uint32_t * ZLX_RESTRICT out
)
{
    size_t l;
    uint32_t ucp;

    if (in >= end) return ZLX_UTF_ERR_TRUNC;
    if (!zlx_utf8_lead_valid(*in)) return ZLX_UTF8_ERR_LEAD;
    l = zlx_utf8_lead_to_len(*in);
    if ((size_t) (end - in) < l) return ZLX_UTF_ERR_TRUNC;
    switch (l)
    {
    case 1:
        if (!*in && (flags & ZLX_UTF8_DEC_NO_NUL_BYTE)) 
            return ZLX_UTF8_ERR_NUL_BYTE;
        ucp = *in;
        break;
    case 2:
        if ((in[1] & 0xC0) != 0x80) return ZLX_UTF8_ERR_CONT1;
        ucp = ((in[0] & 0x1F) << 6) | (in[1] & 0x3F);
        if (ucp < 0x80 && !(flags & ZLX_UTF8_DEC_OVERLY_LONG) && 
            (ucp || !(flags & ZLX_UTF8_DEC_TWO_BYTE_NUL)))
            return ZLX_UTF8_ERR_OVERLY_LONG;
    case 3:
        if ((in[1] & 0xC0) != 0x80) return ZLX_UTF8_ERR_CONT1;
        if ((in[2] & 0xC0) != 0x80) return ZLX_UTF8_ERR_CONT2;
        ucp = ((in[0] & 0x0F) << 12) | ((in[1] & 0x3F) << 6) | (in[2] & 0x3F);
        if (ucp < 0x80 && !(flags & ZLX_UTF8_DEC_OVERLY_LONG))
            return ZLX_UTF8_ERR_OVERLY_LONG;
    l_surrogate_check:
        if ((ucp & 0xF800) == 0xD800)
        {
            /* surrogate codepoint */
            if ((flags & ZLX_UTF8_DEC_SURROGATE_PAIRS) && ucp < 0xDC00)
            {
                ptrdiff_t tl;
                uint32_t tcp;
                /* decode the next utf8 and validate that it is a tail 
                 * surrogate and combine it with our lead surrogate */
                tl = zlx_utf8_to_ucp(in + l, end, 
                                     ZLX_UTF8_DEC_SURROGATE_PAIRS, &tcp);
                if (tl >= 0 && (tcp & 0xFC00) == 0xDC00)
                {
                    /* got a valid surrogate pair and we're supposed to combine
                     * them */
                    ucp = (((ucp & 0x3FF) << 10) | (tcp & 0x3FF)) + 0x10000;
                    l += tl;
                    break;
                }
            }
            /* unpaired surrogate */
            if (!(flags & ZLX_UTF8_DEC_SURROGATES))
                return ZLX_UTF_ERR_SURROGATE;
        }
        break;
    case 4:
        if ((in[1] & 0xC0) != 0x80) return ZLX_UTF8_ERR_CONT1;
        if ((in[2] & 0xC0) != 0x80) return ZLX_UTF8_ERR_CONT2;
        if ((in[3] & 0xC0) != 0x80) return ZLX_UTF8_ERR_CONT3;
        ucp = ((in[0] & 0x07) << 18) | ((in[1] & 0x3F) << 12) 
            | ((in[2] & 0x3F) << 6) | (in[3] & 0x3F);
        if (ucp < 0x10000)
        {
            if (!(flags & ZLX_UTF8_DEC_OVERLY_LONG))
                return ZLX_UTF8_ERR_OVERLY_LONG;
        }
        if (ucp >= 0x110000) return ZLX_UTF8_ERR_CP_TOO_BIG;
        goto l_surrogate_check;
    }
    *out = ucp;
    return l;
}

/* zlx_ucp_to_utf8_size *****************************************************/
ZLX_API size_t ZLX_CALL zlx_ucp_to_utf8_size
(
    uint32_t ucp,
    unsigned int flags
)
{
    if (ucp < 0x80)
        return !ucp && (flags & ZLX_UTF8_ENC_TWO_BYTE_NUL) ? 2 : 1;
    if (ucp >= 0x10000)
    {
        return ((flags & ZLX_UTF8_ENC_BREAK_SUPPLEM)) ? 6 : 4;
    }
    return ucp < 0x800 ? 2 : 3;
}

/* zlx_ucp_to_utf16_size ****************************************************/
ZLX_API size_t ZLX_CALL zlx_ucp_to_utf16_size // in bytes
(
    uint32_t ucp,
    unsigned int flags
)
{
    (void) flags;
    return ucp < 0x10000 ? 2 : 4;
}

/* zlx_ucp_to_utf32_size ****************************************************/
ZLX_API size_t ZLX_CALL zlx_ucp_to_utf32_size
(
    uint32_t ucp,
    unsigned int flags
)
{
    return (ucp >= 0x10000) && (flags & ZLX_UTF32_ENC_BREAK_SUPPLEM) ? 8 : 4;
}

/* zlx_ucp_to_utf8 **********************************************************/
ZLX_API size_t ZLX_CALL zlx_ucp_to_utf8
(
    uint32_t ucp,
    unsigned int flags,
    uint8_t * ZLX_RESTRICT out
)
{
    if (!ucp && (flags & ZLX_UTF8_ENC_TWO_BYTE_NUL))
    {
        out[0] = 0xC0;
        out[1] = 0x80;
        return 2;
    }
    if (ucp >= 0x10000 && (flags & ZLX_UTF8_ENC_BREAK_SUPPLEM))
    {
        size_t l = zlxi_ucp_to_utf8(0xD800 | (ucp >> 10), out);
        l += zlxi_ucp_to_utf8(0xDC00 | (ucp & 0x3FF), out + l);
        return l;
    }
    return zlxi_ucp_to_utf8(ucp, out);
}

/* zlx_ucp_to_utf16le *******************************************************/
ZLX_API size_t ZLX_CALL zlx_ucp_to_utf16le
(
    uint32_t ucp,
    unsigned int flags,
    uint8_t * ZLX_RESTRICT out
)
{
    (void) flags;
    if (ucp < 0x10000)
    {
        ZLX_UWRITE_U16LE(out, (uint16_t) ucp);
        return 2;
    }
    ZLX_UWRITE_U16LE(out, (uint16_t) (0xD800 | (ucp >> 10)));
    ZLX_UWRITE_U16LE(out, (uint16_t) (0xDC00 | (ucp & 0x3FF)));
    return 4;
}

/* zlx_ucp_to_utf32le *******************************************************/
ZLX_API size_t ZLX_CALL zlx_ucp_to_utf32le
(
    uint32_t ucp,
    unsigned int flags,
    uint8_t * ZLX_RESTRICT out
)
{
    if ((ucp >= 0x10000) && (flags & ZLX_UTF32_ENC_BREAK_SUPPLEM))
    {
        ZLX_UWRITE_U32LE(out, (0xD800 | (ucp >> 10)));
        ZLX_UWRITE_U32LE(out + 4, (0xDC00 | (ucp & 0x3FF)));
        return 8;
    }
    ZLX_UWRITE_U32LE(out, ucp);
    return 4;
}

/* zlx_uconv ****************************************************************/
ZLX_API ptrdiff_t ZLX_CALL zlx_uconv
(
    uint8_t const * ZLX_RESTRICT in,
    size_t in_size,
    unsigned int flags,
    uint8_t * ZLX_RESTRICT out,
    size_t out_size,
    size_t * in_pos
)
{
    uint8_t const * i = in;
    uint8_t const * end = in + in_size;
    uint8_t * o = out;
    uint8_t * oend = out + out_size;
    zlx_decode_ucp_func_t decode;
    zlx_encoded_ucp_size_func_t measure;
    zlx_encode_ucp_func_t encode;
    uint32_t ucp;

    switch ((flags & 7))
    {
    case ZLX_UTF32LE_DEC: decode = zlx_utf32le_to_ucp; break;
    case ZLX_UTF16LE_DEC: decode = zlx_utf16le_to_ucp; break;
    case ZLX_UTF8_DEC: decode = zlx_utf8_to_ucp; break;
    default: return ZLX_UTF_ERR_NO_CONV;
    }
    switch ((flags & (7 << 3)))
    {
    case ZLX_UTF32LE_ENC:
        measure = zlx_ucp_to_utf32_size;
        encode = zlx_ucp_to_utf32le;
        break;
    case ZLX_UTF16LE_ENC:
        measure = zlx_ucp_to_utf16_size;
        encode = zlx_ucp_to_utf16le;
        break;
    case ZLX_UTF8_ENC:
        measure = zlx_ucp_to_utf8_size;
        encode = zlx_ucp_to_utf8;
        break;
    default: return ZLX_UTF_ERR_NO_CONV;
    }
    while (i < end)
    {
        ptrdiff_t cl;
        size_t ol;
        cl = decode(i, end, flags, &ucp);
        if (cl < 0) 
        {
            if (in_pos) *in_pos = i - in;
            return cl;
        }
        i += cl;
        ol = measure(ucp, flags);
        if (o + ol <= oend) encode(ucp, flags, o);
        o += ol;
    }

    if (in_pos) *in_pos = i - in;
    return o - out;
}

extern uint8_t zlx_ucp_width_ofs_a[];
extern uint8_t zlx_ucp_width_val_a[];

/* zlx_ucp_term_width *******************************************************/
ZLX_API int ZLX_CALL zlx_ucp_term_width (uint32_t ucp)
{
    return ((int) (zlx_ucp_width_val_a[
        ((unsigned int) zlx_ucp_width_ofs_a[ucp >> 8] << 6)
        + ((ucp >> 2) & 0x3F)] >> (2 * (ucp & 3))) & 3) - 1;

}

/* zlx_utf8_term_width ******************************************************/
ZLX_API ptrdiff_t ZLX_CALL zlx_utf8_term_width
(
    void * ZLX_RESTRICT obj,
    uint8_t const * ZLX_RESTRICT data,
    size_t size
)
{
    ptrdiff_t w = 0;
    uint8_t const * end = data + size;
    (void) obj;
    while (data != end)
    {
        ptrdiff_t l;
        uint32_t ucp;
        int cw;
        l = zlx_utf8_to_ucp(data, end, 0, &ucp);
        if (l < 0) return -1;
        data += l;
        cw = zlx_ucp_term_width(ucp);
        if (cw < 0) return -2;
        w += cw;
        if (w < 0) return -3;
    }
    return w;
}


