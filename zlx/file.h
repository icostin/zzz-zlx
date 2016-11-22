#ifndef _ZLX_FILE_H
#define _ZLX_FILE_H

#include <stdarg.h>
#include "base.h"

/** @defgroup file File I/O
 *  Common interface for file I/O operations.
 *  @{ */

/*  zlx_file_status_t  */
/**
 *  Integral type describing the status of file operations.
 */
typedef enum zlx_file_status_enum zlx_file_status_t;

/*  zlx_file_t  */
/**
 *  Stub type that is the base of file types.
 */
typedef struct zlx_file_s zlx_file_t;

/*  zlx_file_class_t  */
/**
 *  File class containing the functions to perform operations on a file.
 */
typedef struct zlx_file_class_s zlx_file_class_t;

/*  zlx_file_writer_ctx_t  */
/**
 *  Context structure to allow using files where a writer (#zlx_writer_t) is 
 *  required.
 */
typedef struct zlx_file_writer_ctx_s zlx_file_writer_ctx_t;

/** Anchor for seek to specify an offset from the beginning of the file */
#define ZLXF_SET 0

/** Anchor for seek to specify an offset from the current location in the file */
#define ZLXF_CUR 1

/** Anchor for seek to specify an offset from the end of the file */
#define ZLXF_END 2

/** Alias for #ZLXF_CUR that doesn't sound rude in Romanian */
#define ZLXF_CRT ZLXF_CUR // for pudic Romanians

/*  ZLXF_READ  */
/**
 *  File flag showing that reads are allowed on the given file instance.
 *  This flag is used with zlx_file_t#flags and zlx_file_class_t#close.
 */
#define ZLXF_READ (1 << 0)

/*  ZLXF_WRITE  */
/**
 *  File flag showing that writes are allowed on the given file instance.
 *  This flag is used with zlx_file_t#flags and zlx_file_class_t#close.
 */
#define ZLXF_WRITE (1 << 1)

/*  ZLXF_SEEK  */
/**
 *  File flag signifying that seeking is allowed on the given file instance.
 *  This flag is used with zlx_file_t#flags.
 */
#define ZLXF_SEEK (1 << 2)

/*  ZLXF_NONBLOCK  */
/**
 *  File flag signifying that reads and writes are non-blocking on the given
 *  file instance.
 *  This flag is used with zlx_file_t#flags.
 */
#define ZLXF_NONBLOCK (1 << 3)

enum zlx_file_status_enum
{
    /** Ok */
    ZLXF_OK = 0,

    /** Generic failure message */
    ZLXF_FAILED,

    /** For non-blocking files the read or write operation would block. */
    ZLXF_WOULD_BLOCK,

    /** Inaccessible buffer */
    ZLXF_BAD_BUFFER,

    /** Operation requested not available */
    ZLXF_BAD_OPERATION,

    /** Bad file descriptor */
    ZLXF_BAD_FILE_DESC,

    /** A signal or APC interrupted the operation */
    ZLXF_INTERRUPTED,

    /** I/O error */
    ZLXF_IO_ERROR,

    /** No space to finish the write operation */
    ZLXF_NO_SPACE,

    /** User quota exhausted during the write operation */
    ZLXF_QUOTA_EXHAUSTED,

    /** Operation size limit for given file object exceeded */
    ZLXF_SIZE_LIMIT,

    /** File position or size of operation overflow */
    ZLXF_OVERFLOW,

    /** Malformed print format string, see #ZLX_FMT_MALFORMED */
    ZLXF_FMT_MALFORMED,

    /** Print format width error, see #ZLX_FMT_WIDTH_ERROR */
    ZLXF_FMT_WIDTH_ERROR,

    /** Print format write error, see #ZLX_FMT_WRITE_ERROR */
    ZLXF_FMT_WRITE_ERROR,

    /** Print formatting error while converting, see #ZLX_FMT_CONV_ERROR */
    ZLXF_FMT_CONV_ERROR,

    /** No code */
    ZLXF_NO_CODE
};

struct zlx_file_class_s
{
    /** Reads from the file. 
     *  @param f [in, out]
     *      file stream to read from
     *  @param data [out]
     *      buffer to fill with data
     *  @param size [in]
     *      amount of data requested
     *  @returns 
     *      number of bytes read, which can be less than @a size, or a
     *      negative number on error matching a negated #zlx_file_status_t value
     **/
    ptrdiff_t (ZLX_CALL * read)
        (
            zlx_file_t * ZLX_RESTRICT f,
            uint8_t * ZLX_RESTRICT data,
            size_t size
        );

    /** Writes to the file. 
     *  @param f [in, out]
     *      file stream to write to
     *  @param data [in]
     *      buffer to write
     *  @param size [in]
     *      size of buffer
     *  @returns number of bytes successfully sent for writing, which can be
     *      less than @a size, or a negative number on error, matching a
     *      negated #zlx_file_status_t value.
     **/
    ptrdiff_t (ZLX_CALL * write)
        (
            zlx_file_t * ZLX_RESTRICT f,
            uint8_t const * ZLX_RESTRICT data,
            size_t size
        );

    /** Seeks in the file. */
    int64_t (ZLX_CALL * seek64)
        (
            zlx_file_t * ZLX_RESTRICT f,
            int64_t offset,
            int anchor
        );

    /** Truncates file at current file position. */
    zlx_file_status_t (ZLX_CALL * truncate)
        (
            zlx_file_t * ZLX_RESTRICT f
        );

    /** Closes the read, write or both sides of the stream */
    zlx_file_status_t (ZLX_CALL * close)
        (
            zlx_file_t * ZLX_RESTRICT f,
            unsigned int flags // ZLXF_READ | ZLXF_WRITE
        );

    /** A static name describing the file class */
    char const * ZLX_RESTRICT name;
};

struct zlx_file_s
{
    /** File class */
    zlx_file_class_t const * fcls;

    /** Flags describing operations supported by this file object.
     *  This is a bit-mask holding a combination of #ZLXF_READ, #ZLXF_WRITE, 
     *  #ZLXF_SEEK, #ZLXF_NONBLOCK. */
    uint32_t flags;
};

/* zlx_raw_read *************************************************************/
/**
 *  Reads data from a file.
 *  This simply invokes the read function from the file class of the given
 *  file object. See zlx_file_class_t#read.
 *
 *  @note 
 *      if the file class translates for instance file accesses on the local
 *      host file system then the read may get interrupted by the asynchronous
 *      event delivery mechanism of the OS (signals for Unix-like OSes and
 *      APCs for Windows-like Windowses :-P), returning ZLXF_INTERRUPTED.
 */
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
/**
 *  Writes data to a file.
 *  This simply invokes the write function from the file class of the given
 *  file object. See zlx_file_class_t#read.
 *
 *  @note 
 *      if the file class translates for instance file accesses on the local
 *      host file system then the write may get interrupted by the asynchronous
 *      event delivery mechanism of the OS (signals for Unix-like OSes and
 *      APCs for Windows-like Windowses :-P), returning ZLXF_INTERRUPTED.
 */
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
 *  Implementation of the writer interface that wraps file objects.
 *  @param obj [in, out]
 *      an initialized instance of #zlx_file_writer_ctx_t
 *  @param data [in]
 *      input data
 *  @param size [in]
 *      size in bytes of input data
 *  @returns size successfully written or a negative int for error
 *  @note 
 *      the caller of a writer function treats any return value that doesn't
 *      match @a size as an error and stops writing.
 *
 */
ZLX_API ptrdiff_t ZLX_CALL zlx_file_writer
(
    void * obj,
    uint8_t const * ZLX_RESTRICT data,
    size_t size
);

/* zlx_seek64 ***************************************************************/
/**
 *  Seeks into the file.
 *  @param zf [in, out]
 *      file to seek into
 *  @param ofs [in]
 *      64-bit signed int specifying the distance from the anchor.
 *  @param anchor [in]
 *      one of #ZLXF_SET, #ZLXF_CUR or #ZLXF_END specifying the position of
 *      the anchor (begin, current position, end of the file)
 *  @returns the absolute resultant non-negative file position, or if negative
 *      a negated #zlx_file_status_t value.
 */
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
/**
 *  Closes both the read and write ends of the stream.
 *  @param zf [in, out]
 *      file to close
 *  @returns the status of the operation.
 *  @warning 
 *      this does not free the memory associated with the file object; since
 *      functions that create such file objects are custom, the functions
 *      for destroying them are custom as well.
 */
ZLX_INLINE zlx_file_status_t zlx_close
(
    zlx_file_t * ZLX_RESTRICT zf
)
{
    return zf->fcls->close(zf, ZLXF_READ | ZLXF_WRITE);
}

/* zlx_fvprint **************************************************************/
/**
 *  Prints a formatted string to the given file.
 *  @param zf [in, out]
 *      file to write to
 *  @param fmt [in]
 *      format string; see zlx_vfmt()
 *  @param va [in]
 *      var arg with arguments to be formatted
 *  @returns 
 *      number of bytes written on success or one of -#ZLXF_FMT_MALFORMED,
 *      -#ZLXF_FMT_WIDTH_ERROR, -#ZLXF_FMT_WRITE_ERROR, -#ZLXF_FMT_CONV_ERROR.
 *
 */
ZLX_API ptrdiff_t ZLX_CALL zlx_fvprint
(
    zlx_file_t * ZLX_RESTRICT zf,
    char const * fmt,
    va_list va
);

/* zlx_fprint ***************************************************************/
/**
 *  Prints a formatted string to the given file.
 *  Prints a formatted string to the given file.
 *  @param zf [in, out]
 *      file to write to
 *  @param fmt [in]
 *      format string; see zlx_vfmt()
 *  @returns 
 *      number of bytes written on success or one of -#ZLXF_FMT_MALFORMED,
 *      -#ZLXF_FMT_WIDTH_ERROR, -#ZLXF_FMT_WRITE_ERROR, -#ZLXF_FMT_CONV_ERROR.
 */
ZLX_API ptrdiff_t ZLX_CALL zlx_fprint
(
    zlx_file_t * ZLX_RESTRICT zf,
    char const * fmt,
    ...
);

/*  zlx_null_file  */
/**
 *  A file object behaving like a typical "/dev/null" (or "NUL") file.
 */
ZLX_API zlx_file_t zlx_null_file;

#endif /* _ZLX_FILE_H */

