#include "zlx/unicode.h"
#include "zlx/stdarray.h"
#include "zlx/clconv.h"
#include "zlx/fmt.h"

ZLX_API char const zlx_digit_char_table[37] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

/* zlx_u64_to_str ***********************************************************/
ZLX_API size_t ZLX_CALL zlx_u64_to_str
(
    uint8_t * str,
    uint64_t value,
    uint_fast8_t radix,
    uint32_t width,
    uint_fast8_t group,
    uint_fast8_t sep
)
{
    unsigned int g, a, b, i;

#if _DEBUG
    if (radix < 2 || radix > 36)
    {
        static char bad_radix[] = "BAD_RADIX";
        zlx_u8a_copy(str, (uint8_t const *) bad_radix, sizeof bad_radix);
        return sizeof bad_radix - 1;
    }

#endif
    //printf("[u64_to_str:%"PRIx64",w=%u]\n", value, width);
    for (i = g = 0; i < width || value; i++)
    {
        uint8_t digit = value % radix;
        value /= radix;
        str[i] = zlx_digit_char_table[digit];
        if (++g == group)
        {
            str[++i] = sep;
            g = 0;
        }
    }
    if (!i) str[i++] = '0';
    str[i] = 0;
    for (a = 0, b = i - 1; a < b; a++, b--)
    {
        uint8_t ch = str[a];
        str[a] = str[b];
        str[b] = ch;
    }
    return i;
}

/* zlx_i64_to_str ***********************************************************/
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
)
{
    uint8_t * start = str;
    size_t w;

    switch (sign_mode)
    {
    case ZLX_NO_SIGN:
        break;
    case ZLX_SIGN_NEG:
        if (value >= 0) break;
        value = -value;
        *str++ = '-';
        break;
    case ZLX_SIGN_ALWAYS:
        if (value < 0)
        {
            *str++ = '-';
            value -= value;
            break;
        }
        *str++ = value ? '+' : ' ';
        break;
    case ZLX_SIGN_ALIGN:
        if (value < 0)
        {
            *str++ = '-';
            value -= value;
            break;
        }
        *str++ = ' ';
        break;
    }
    if (prefix) str = zlx_u8a_zcopy(str, prefix);
    w = str - start;
    if (w > width) width = 0;
    else width -= w;
    return w + zlx_u64_to_str(str, (uint64_t) value, radix, width, group, sep);
}

/* zlx_u64_from_str *********************************************************/
ZLX_API uint_fast8_t ZLX_CALL zlx_u64_from_str
(
    uint8_t const * str,
    size_t len,
    uint_fast8_t radix,
    uint64_t * value,
    size_t * used_len
)
{
    size_t i;
    uint64_t v;
    uint_fast8_t r = 0;

    if (len == 0) return ZLX_U64_STOP;
    i = 0;
    if (radix == 0)
    {
        if (*str != '0' || len == 1) radix = 10;
        else
        {
            switch (str[1])
            {
            case 'b': radix = 2; i = 2; break;
            case 'o': radix = 8; i = 2; break;
            case 'd': radix = 10; i = 2; break;
            case 'x': radix = 16; i = 2; break;
            default:
                radix = 10;
            }
        }
    }

    for (v = 0; i < len; ++i)
    {
        uint8_t digit;
        uint64_t w;
        if (str[i] >= '0' && str[i] <= '9') digit = str[i] - '0';
        else
        {
            digit = str[i] | 0x20;
            if (digit >= 'a' && digit <= 'z') digit -= ('a' - 10);
            else digit = radix;
        }
        if (digit >= radix) { r = ZLX_U64_STOP; break; }
        w = v * radix;
        if (w / radix != v) { r = ZLX_U64_OVERFLOW; break; }
        v = w + digit;
        if (v < w) { r = ZLX_U64_OVERFLOW; break; }
    }
    *value = v;
    if (used_len) *used_len = i;
    return r;
}
/* zlx_vfmt *****************************************************************/
#define CMD_NONE 0
#define CMD_BUF 1
#define CMD_STR 2
#define CMD_CONV 3
#define ALIGN_DEFAULT 0
#define ALIGN_LEFT 1
#define ALIGN_RIGHT 2
#define STR_ESC_NONE 0
#define STR_ESC_C 1
#define STR_ESC_HEX 2
ZLX_API unsigned int ZLX_CALL zlx_vfmt
(
    zlx_write_func_t writer,
    void * writer_context,
    zlx_write_func_t width_func,
    void * width_context,
    char const * fmt,
    va_list va
)
{
    //static uint8_t const nul = 0;
    static uint8_t const empty_spaces[] =  // what are we living for?
        "                                                                ";
    uint8_t buffer[0x400];
    uint8_t const * f = (uint8_t const *) fmt;
    uint8_t const * str = NULL;
    uint8_t const * num_pfx;
    zlx_clconv_func_t conv = NULL;
    void * conv_ctx;
    uint_fast8_t sep;
    uint_fast8_t group_len;
    char zero_fill;
    char align_mode;
    char sign_mode;
    size_t z, req_width, arg_len, prec;
    ptrdiff_t arg_width;
    uint32_t ucp;
    uint8_t radix;
    int64_t i64;
    char cmd;
    char esc_mode;
    size_t ofs, in_len, out_len;
    uint_fast8_t cc;
    zlx_clconv_c_escape_t cectx;

    for (;;)
    {
        uint8_t const * sfmt = f;

        while (*f && *f != '$') f++;
        z = f - sfmt;
        if (z && writer(writer_context, sfmt, z) != (ptrdiff_t) z)
            return ZLX_FMT_WRITE_ERROR;
        if (*f == 0) break;
        zero_fill = 0;
        req_width = 0;
        sign_mode = ZLX_SIGN_NEG;
        align_mode = ALIGN_DEFAULT;
        esc_mode = STR_ESC_NONE;
        radix = 0;
        prec = SIZE_MAX;
        num_pfx = NULL;
        sep = '_';
        group_len = 64;
        arg_width = 0;
        arg_len = 0;
        f++;
        for (cmd = CMD_NONE; cmd == CMD_NONE; ++f)
        {
            switch (*f)
            {
            case '0':
                zero_fill = 1;
                continue;
            case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                for (req_width = 0; *f >= '0' && *f <= '9'; ++f)
                    req_width = req_width * 10 + *f - '0';
                --f;
                break;
            case 'c':
                ucp = va_arg(va, uint32_t);
                if (!zlx_ucp_is_valid(ucp)) return ZLX_FMT_MALFORMED;
                arg_len = zlxi_ucp_to_utf8(ucp, buffer);
                arg_width = width_func(width_context, buffer, arg_len);
                if (arg_width < 0) return ZLX_FMT_WIDTH_ERROR;
                cmd = CMD_BUF;
                if (align_mode == ALIGN_DEFAULT) align_mode = ALIGN_LEFT;
                break;
            case 'b':
                sign_mode = ZLX_NO_SIGN;
                i64 = (uint8_t) va_arg(va, int);
            l_int:
                if (radix == 0) radix = 10;
                arg_len = zlx_i64_to_str(buffer, i64, sign_mode, radix, num_pfx,
                                zero_fill ? req_width : 1, group_len, sep);
                arg_width = width_func(width_context, buffer, arg_len);
                if (arg_width < 0) return ZLX_FMT_WIDTH_ERROR;
                cmd = CMD_BUF;
                if (align_mode == ALIGN_DEFAULT) align_mode = ALIGN_RIGHT;
                break;
            case 'B':
                i64 = (int8_t) va_arg(va, int);
                goto l_int;
            case 'w':
                sign_mode = ZLX_NO_SIGN;
                i64 = (uint16_t) va_arg(va, int);
                goto l_int;
            case 'W':
                i64 = (int16_t) va_arg(va, int);
                goto l_int;
            case 'd':
                sign_mode = ZLX_NO_SIGN;
                i64 = va_arg(va, uint32_t);
                goto l_int;
            case 'D':
                i64 = va_arg(va, int32_t);
                goto l_int;
            case 'q':
                sign_mode = ZLX_NO_SIGN;
                i64 = va_arg(va, uint64_t);
                goto l_int;
            case 'Q':
                i64 = va_arg(va, int64_t);
                goto l_int;
            case 'i':
                sign_mode = ZLX_NO_SIGN;
                i64 = va_arg(va, unsigned int);
                goto l_int;
            case 'I':
                i64 = va_arg(va, signed int);
                goto l_int;
            case 'l':
                sign_mode = ZLX_NO_SIGN;
                i64 = va_arg(va, unsigned long int);
                goto l_int;
            case 'L':
                i64 = va_arg(va, signed long int);
                goto l_int;
            case 'h':
                sign_mode = ZLX_NO_SIGN;
                i64 = (unsigned short int) va_arg(va, int);
                goto l_int;
            case 'H':
                i64 = (signed short int) va_arg(va, int);
                goto l_int;
            case 'z':
                sign_mode = ZLX_NO_SIGN;
                i64 = va_arg(va, size_t);
                goto l_int;
            case 'Z':
                i64 = va_arg(va, ptrdiff_t);
                goto l_int;
            case 'p':
                sign_mode = ZLX_NO_SIGN;
                i64 = va_arg(va, uintptr_t);
                goto l_int;
            case 'P':
                i64 = va_arg(va, intptr_t);
                if (radix == 0) radix = 16;
                goto l_int;
            case 'y':
                num_pfx = (uint8_t const *) "0b";
            case 'Y':
                radix = 2;
                break;
            case 'o':
                num_pfx = (uint8_t const *) "0o";
            case 'O':
                radix = 8;
                break;
            case 'n':
                num_pfx = (uint8_t const *) "0d";
            case 'N':
                radix = 10;
                break;
            case 'x':
                num_pfx = (uint8_t const *) "0x";
            case 'X':
                esc_mode = STR_ESC_HEX;
                radix = 16;
                break;
            case 's':
                str = va_arg(va, uint8_t const *);
                if (prec == SIZE_MAX) arg_len = zlx_u8a_zlen(str);
                else arg_len = prec;
                switch (esc_mode)
                {
                case STR_ESC_NONE:
                    cmd = CMD_STR;
                    arg_width = width_func(width_context, str, arg_len);
                    if (arg_width < 0) return ZLX_FMT_WIDTH_ERROR;
                    break;
                case STR_ESC_HEX:
                    cmd = CMD_CONV;
                    conv = zlx_clconv_bin_to_hex_line;
                    conv_ctx = NULL;
                    break;
                case STR_ESC_C:
                    cmd = CMD_CONV;
                    conv = zlx_clconv_c_escape;
                    conv_ctx = zlx_clconv_c_escape_init(&cectx);
                    break;
                default:
                    return ZLX_FMT_NO_CODE;
                }
                if (cmd == CMD_CONV && req_width)
                {
                    int32_t width;
                    for (arg_width = 0, ofs = 0; ofs < arg_len; ofs += in_len)
                    {
                        cc = conv(str + ofs, arg_len - ofs, &in_len,
                                  buffer, sizeof buffer, &out_len, conv_ctx);
                        if (cc && cc != ZLX_CLCONV_FULL)
                            return ZLX_FMT_CONV_ERROR;
                        width = width_func(width_context, buffer, out_len);
                        if (width < 0) return ZLX_FMT_WIDTH_ERROR;
                        arg_width += width;
                    }
                    cc = conv(NULL, 0, &in_len, buffer, sizeof buffer,
                              &out_len, conv_ctx);
                    width = width_func(width_context, buffer, out_len);
                    if (width < 0) return ZLX_FMT_WIDTH_ERROR;
                    arg_width += width;
                }
                break;
            case '.':
                ++f;
                if (*f == '*')
                {
                    prec = va_arg(va, size_t);
                }
                else
                {
                    for (prec = 0; *f >= '0' && *f <= '9'; ++f)
                        prec = prec * 10 + *f - '0';
                    --f;
                }
                break;
            case '<':
                align_mode = ALIGN_LEFT;
                break;
            case '>':
                align_mode = ALIGN_RIGHT;
                break;
            case '/':
                if (f[1] < '0' || f[1] > '9' ||
                    f[2] == 0) return ZLX_FMT_MALFORMED;
                group_len = f[1] - '0';
                sep = f[2];
                f += 2;
                break;
            case 'e':
                esc_mode = STR_ESC_C;
                break;
            default:
                return ZLX_FMT_MALFORMED;
            }
        }

        if ((size_t) arg_width < req_width && align_mode == ALIGN_RIGHT)
        {
            size_t pad_width = req_width - arg_width;
            size_t clen;
            for (; pad_width; pad_width -= clen)
            {
                clen = pad_width;
                if (clen > sizeof(empty_spaces) - 1)
                    clen = sizeof(empty_spaces) - 1;
                if (writer(writer_context, empty_spaces, clen) 
                    != (ptrdiff_t) clen)
                    return ZLX_FMT_WRITE_ERROR;
            }
        }

        switch (cmd)
        {
        case CMD_BUF:
            z = writer(writer_context, buffer, arg_len);
            if (z != arg_len) return ZLX_FMT_WRITE_ERROR;
            break;
        case CMD_STR:
            z = writer(writer_context, str, arg_len);
            if (z != arg_len) return ZLX_FMT_WRITE_ERROR;
            break;
        case CMD_CONV:
            switch (esc_mode)
            {
            case STR_ESC_C:
                conv_ctx = zlx_clconv_c_escape_init(&cectx);
                break;
            default:
                conv_ctx = NULL;
            }

            for (ofs = 0; ofs < arg_len; ofs += in_len)
            {
                cc = conv(str + ofs, arg_len - ofs, &in_len,
                          buffer, sizeof buffer, &out_len, conv_ctx);
                if (cc && cc != ZLX_CLCONV_FULL) return ZLX_FMT_CONV_ERROR;
                if (writer(writer_context, buffer, out_len) 
                    != (ptrdiff_t) out_len)
                    return ZLX_FMT_WRITE_ERROR;
            }
            cc = conv(NULL, 0, &in_len,
                      buffer, sizeof buffer, &out_len, conv_ctx);
            if (cc) return ZLX_FMT_CONV_ERROR;
            if (writer(writer_context, buffer, out_len) 
                != (ptrdiff_t) out_len)
                return ZLX_FMT_WRITE_ERROR;
        }

        if ((size_t) arg_width < req_width && align_mode == ALIGN_LEFT)
        {
            size_t pad_width = req_width - arg_width;
            size_t clen;
            for (; pad_width; pad_width -= clen)
            {
                clen = pad_width;
                if (clen > sizeof(empty_spaces) - 1)
                    clen = sizeof(empty_spaces) - 1;
                if (writer(writer_context, empty_spaces, clen) 
                    != (ptrdiff_t) clen)
                    return ZLX_FMT_WRITE_ERROR;
            }
        }

    }
    //if (writer(&nul, 1, writer_context) != 1) return ZLX_FMT_WRITE_ERROR;

    return 0;
}
#undef CMD_NONE
#undef CMD_BUF
#undef CMD_STR
#undef CMD_CONV
#undef ALIGN_DEFAULT
#undef ALIGN_LEFT
#undef ALIGN_RIGHT
#undef STR_ESC_NONE
#undef STR_ESC_C
#undef STR_ESC_HEX

/* zlx_fmt ******************************************************************/
ZLX_API unsigned int ZLX_CALL zlx_fmt
(
    zlx_write_func_t writer,
    void * writer_context,
    zlx_write_func_t width_func,
    void * width_context,
    char const * fmt,
    ...
)
{
    va_list va;
    unsigned int rc;

    va_start(va, fmt);
    rc = zlx_vfmt(writer, writer_context, width_func, width_context, fmt, va);
    va_end(va);
    return rc;
}

