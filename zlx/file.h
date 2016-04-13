#ifndef _ZLX_FILE_H
#define _ZLX_FILE_H

#include <stdarg.h>
#include "base.h"

typedef enum zlx_file_status_enum zlx_file_status_t;
typedef struct zlx_file_s zlx_file_t;
typedef struct zlx_file_class_s zlx_file_class_t;
typedef struct zlx_file_writer_ctx_s zlx_file_writer_ctx_t;

#define ZLXF_SET 0
#define ZLXF_CUR 1
#define ZLXF_END 2

#define ZLXF_CRT ZLXF_CUR // for pudic Romanians

#define ZLXF_READ (1 << 0)
#define ZLXF_WRITE (1 << 1)
#define ZLXF_SEEK (1 << 2)
#define ZLXF_NONBLOCK (1 << 3)

enum zlx_file_status_enum
{
    ZLXF_OK = 0,
    ZLXF_WOULD_BLOCK,
    ZLXF_BAD_BUFFER,
    ZLXF_BAD_OPERATION,
    ZLXF_BAD_FILE_DESC,
    ZLXF_INTERRUPTED, // signal or APC interrupted the operation
    ZLXF_IO_ERROR,
    ZLXF_FAILED,
    ZLXF_NO_SPACE,
    ZLXF_QUOTA_EXHAUSTED,
    ZLXF_SIZE_LIMIT,
    ZLXF_OVERFLOW,
    ZLXF_FMT_MALFORMED,
    ZLXF_FMT_WIDTH_ERROR,
    ZLXF_FMT_WRITE_ERROR,
    ZLXF_FMT_CONV_ERROR,
    ZLXF_NO_CODE
};

typedef 
    ptrdiff_t (ZLX_CALL * zlx_file_write_func_t)
        (
            zlx_file_t * ZLX_RESTRICT f,
            uint8_t const * ZLX_RESTRICT data,
            size_t size
        );

struct zlx_file_class_s
{
    ptrdiff_t (ZLX_CALL * read)
        (
            zlx_file_t * ZLX_RESTRICT f,
            uint8_t * ZLX_RESTRICT data,
            size_t size
        );
    ptrdiff_t (ZLX_CALL * write)
        (
            zlx_file_t * ZLX_RESTRICT f,
            uint8_t const * ZLX_RESTRICT data,
            size_t size
        );

    int64_t (ZLX_CALL * seek64)
        (
            zlx_file_t * ZLX_RESTRICT f,
            int64_t offset,
            int anchor
        );
    zlx_file_status_t (ZLX_CALL * truncate)
        (
            zlx_file_t * ZLX_RESTRICT f
        );

    zlx_file_status_t (ZLX_CALL * close)
        (
            zlx_file_t * ZLX_RESTRICT f,
            unsigned int flags // ZLXF_READ | ZLXF_WRITE
        );
    char const * ZLX_RESTRICT name; // a static name for the file class
};

struct zlx_file_s
{
    zlx_file_class_t const * fcls;
    uint32_t flags;
};

/* zlx_raw_read *************************************************************/
ZLX_INLINE ptrdiff_t zlx_raw_read
(
    zlx_file_t * ZLX_RESTRICT zf,
    void * ZLX_RESTRICT data,
    size_t size
)
{
    return zf->fcls->read(zf, data, size);
}

/* zlx_raw_write ************************************************************/
ZLX_INLINE ptrdiff_t zlx_raw_write
(
    zlx_file_t * ZLX_RESTRICT zf,
    void const * ZLX_RESTRICT data,
    size_t size
)
{
    return zf->fcls->write(zf, data, size);
}

/* zlx_read *****************************************************************/
/**
 *  Reads from a file ignoring interruptions.
 *  Interruptions are signals on unix-like systems and APCs on Windows-like
 *  Windows.
 *  @retval -ZLXF_BAD_OPERATION
 *      read not allowed for this file
 *  @retval -ZLXF_OVERFLOW
 *      @a size was too big causing integer overflow
 */
ZLX_API ptrdiff_t ZLX_CALL zlx_read
(
    zlx_file_t * ZLX_RESTRICT zf,
    void * ZLX_RESTRICT data,
    size_t size
);

/* zlx_write ****************************************************************/
/**
 *  Writes to a file ignoring interruptions.
 *  Interruptions are typically seen when working with OS's file system
 *  functions. They are signals on unix-like system and APCs on Windows-like
 *  Windows.
 */
ZLX_API ptrdiff_t ZLX_CALL zlx_write
(
    zlx_file_t * ZLX_RESTRICT zf,
    void const * ZLX_RESTRICT data,
    size_t size
);

struct zlx_file_writer_ctx_s
{
    zlx_file_t * file;
    zlx_file_status_t status;
    size_t written;
};

/* zlx_file_writer **********************************************************/
/**
 *  @param obj [in, out]
 *      an initialized instance of #zlx_file_writer_ctx_t
 *  @param data [in]
 *      input data
 *  @param size [in]
 *      size in bytes of input data
 */
ZLX_API ptrdiff_t ZLX_CALL zlx_file_writer
(
    void * obj,
    uint8_t const * ZLX_RESTRICT data,
    size_t size
);

/* zlx_seek64 ***************************************************************/
ZLX_INLINE int64_t zlx_seek64
(
    zlx_file_t * ZLX_RESTRICT zf,
    int64_t ofs,
    unsigned int anchor
)
{
    return zf->fcls->seek64(zf, ofs, anchor);
}

/* zlx_close ****************************************************************/
ZLX_INLINE zlx_file_status_t zlx_close
(
    zlx_file_t * ZLX_RESTRICT zf
)
{
    return zf->fcls->close(zf, ZLXF_READ | ZLXF_WRITE);
}

/* zlx_fvprint **************************************************************/
ZLX_API ptrdiff_t ZLX_CALL zlx_fvprint
(
    zlx_file_t * ZLX_RESTRICT zf,
    char const * fmt,
    va_list va
);

/* zlx_fprint ***************************************************************/
ZLX_API ptrdiff_t ZLX_CALL zlx_fprint
(
    zlx_file_t * ZLX_RESTRICT zf,
    char const * fmt,
    ...
);

ZLX_API zlx_file_t zlx_null_file;

#endif /* _ZLX_FILE_H */

