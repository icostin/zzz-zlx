#include "zlx/file.h"

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


