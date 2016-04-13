#include "zlx/file.h"
#include "zlx/fmt.h"
#include "zlx/unicode.h"

static ptrdiff_t ZLX_CALL null_file_read
(
    zlx_file_t * f,
    uint8_t * data,
    size_t size
);

static ptrdiff_t ZLX_CALL null_file_write
(
    zlx_file_t * f,
    uint8_t const * data,
    size_t size
);

static int64_t ZLX_CALL null_file_seek64
(
    zlx_file_t * f,
    int64_t offset,
    int anchor
);

static zlx_file_status_t ZLX_CALL null_file_truncate
(
    zlx_file_t * f
);

static zlx_file_status_t ZLX_CALL null_file_close
(
    zlx_file_t * f,
    unsigned int flags // ZLXF_READ | ZLXF_WRITE
);

static zlx_file_class_t null_file_class =
{
    null_file_read,
    null_file_write,
    null_file_seek64,
    null_file_truncate,
    null_file_close,
    "zlx/null"
};

/* zlx_null_file ************************************************************/
ZLX_API zlx_file_t zlx_null_file =
{
    &null_file_class,
    ZLXF_READ | ZLXF_WRITE | ZLXF_SEEK
};

/* zlx_read *****************************************************************/
ZLX_API ptrdiff_t ZLX_CALL zlx_read
(
    zlx_file_t * ZLX_RESTRICT zf,
    void * ZLX_RESTRICT data,
    size_t size
)
{
    ptrdiff_t r;

    if (!(zf->flags & ZLXF_READ)) return -ZLXF_BAD_OPERATION;
    if ((ptrdiff_t) size <= 0) return size ? -ZLXF_OVERFLOW : 0;
    do
    {
        r = zlx_raw_read(zf, data, size);
    } 
    while (r == -ZLXF_INTERRUPTED);
    return r;
}

/* zlx_write ****************************************************************/
ZLX_API ptrdiff_t ZLX_CALL zlx_write
(
    zlx_file_t * ZLX_RESTRICT zf,
    void const * ZLX_RESTRICT data,
    size_t size
)
{
    ptrdiff_t w;
    if (!(zf->flags & ZLXF_WRITE)) return -ZLXF_BAD_OPERATION;
    if ((ptrdiff_t) size <= 0) return size ? -ZLXF_OVERFLOW : 0;
    do
    {
        w = zlx_raw_write(zf, data, size);
    }
    while (w == -ZLXF_INTERRUPTED);
    return w;
}

/* zlx_write_full ***********************************************************/
ZLX_API zlx_file_status_t ZLX_CALL zlx_write_full
(
    zlx_file_t * ZLX_RESTRICT zf,
    void const * ZLX_RESTRICT data,
    size_t size,
    size_t * written
)
{
    size_t z;
    ptrdiff_t r;
    zlx_file_status_t fs = ZLXF_OK;
    for (z = 0; z < size; z += r)
    {
        r = zlx_write(zf, data, size);
        if (r < 0) 
        {
            fs = (zlx_file_status_t) -r;
            break;
        }
    }
    if (written) *written = z;
    return fs;
}

/* null_file_read ***********************************************************/
static ptrdiff_t ZLX_CALL null_file_read
(
    zlx_file_t * f,
    uint8_t * data,
    size_t size
)
{
    (void) f; (void) data; (void) size;
    return 0;
}


/* null_file_write **********************************************************/
static ptrdiff_t ZLX_CALL null_file_write
(
    zlx_file_t * f,
    uint8_t const * data,
    size_t size
)
{
    (void) f; (void) data; (void) size;
    return size;
}


/* null_file_seek64 *********************************************************/
static int64_t ZLX_CALL null_file_seek64
(
    zlx_file_t * f,
    int64_t offset,
    int anchor
)
{
    (void) f; (void) offset; (void) anchor;
    return 0;
}


/* null_file_truncate *******************************************************/
static zlx_file_status_t ZLX_CALL null_file_truncate
(
    zlx_file_t * f
)
{
    (void) f;
    return ZLXF_OK;
}


/* null_file_close **********************************************************/
static zlx_file_status_t ZLX_CALL null_file_close
(
    zlx_file_t * f,
    unsigned int flags // ZLXF_READ | ZLXF_WRITE
)
{
    (void) f; (void) flags;
    return ZLXF_OK;
}

/* zlx_file_writer **********************************************************/
ZLX_API ptrdiff_t ZLX_CALL zlx_file_writer
(
    void * obj,
    uint8_t const * ZLX_RESTRICT data,
    size_t size
)
{
    zlx_file_writer_ctx_t * ZLX_RESTRICT fwc = obj;
    zlx_file_t * ZLX_RESTRICT zf = fwc->file;
    zlx_file_status_t fs;
    size_t wsize;

    fs = zlx_write_full(zf, data, size, &wsize);
    fwc->written += wsize;
    if (fs) 
    {
        fwc->status = fs;
        return -fs;
    }
    return size;
}

/* zlx_fvprint **************************************************************/
ZLX_API ptrdiff_t ZLX_CALL zlx_fvprint
(
    zlx_file_t * ZLX_RESTRICT zf,
    char const * fmt,
    va_list va
)
{
    zlx_file_writer_ctx_t fwc;
    unsigned int fmt_status;
    fwc.file = zf;
    fwc.status = ZLXF_OK;
    fwc.written = 0;
    
    fmt_status = zlx_vfmt(zlx_file_writer, &fwc, zlx_utf8_term_width, NULL, 
                          fmt, va);
    if (fmt_status) return -ZLXF_FMT_MALFORMED + 1 - fmt_status;
    return (ptrdiff_t) fwc.written;
}

/* zlx_fprint ***************************************************************/
ZLX_API ptrdiff_t ZLX_CALL zlx_fprint
(
    zlx_file_t * ZLX_RESTRICT zf,
    char const * fmt,
    ...
)
{
    va_list va;
    ptrdiff_t r;
    va_start(va, fmt);
    r = zlx_fvprint(zf, fmt, va);
    va_end(va);
    return r;
}

