#include "zlx.h"

ZLX_API char const * const zlx_lib_name = "zlx"
#if ZLX_STATIC
    "-static"
#else
    "-dynamic"
#endif
    "-" ZLX_CONFIG 
    "-" ZLX_TARGET
    "-" ZLX_COMPILER;

