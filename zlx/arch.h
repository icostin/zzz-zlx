#ifndef _ZLX_ARCH_H
#define _ZLX_ARCH_H

/** @defgroup arch Architecture
 *  Defines macros that describe the target architecture.
 *  Macros potentially defined:
 *  - #ZLX_BITS
 *  - #ZLX_UNALIGNED_ACCESS
 *  - #ZLX_LITTLE_ENDIAN
 *  - #ZLX_BIG_ENDIAN
 *  -  ZLX_IA32
 *  -  ZLX_AMD64
 *  -  ZLX_ARM32
 *  -  ZLX_THUMB
 *  -  ZLX_ARM64
 *  -  ZLX_ALPHA32
 *  -  ZLX_HPPA
 *  -  ZLX_ITANIUM
 *  -  ZLX_M68K
 *  -  ZLX_MIPS32
 *  -  ZLX_POWERPC32
 *  -  ZLX_POWERPC64
 *  -  ZLX_SPARC32
 *  -  ZLX_SPARC64
 *  @{ */

#undef ZLX_IA32
#undef ZLX_AMD64
#undef ZLX_ARM32
#undef ZLX_THUMB
#undef ZLX_ARM64
#undef ZLX_ALPHA32
#undef ZLX_HPPA
#undef ZLX_ITANIUM
#undef ZLX_M68K
#undef ZLX_MIPS32
#undef ZLX_POWERPC32
#undef ZLX_POWERPC64
#undef ZLX_SPARC32
#undef ZLX_SPARC64


#undef ZLX_UNALIGNED_ACCESS

#if _M_IX86 || _M_I86 || __X86__ || _X86_ || __i386__ || __i386 
#define ZLX_IA32 1
#define ZLX_UNALIGNED_ACCESS 1
#define ZLX_BITS 32
#define ZLX_LITTLE_ENDIAN 1
#undef ZLX_BIG_ENDIAN
#elif _M_X64 || _M_AMD64 || __amd64__ || __x86_64
#define ZLX_AMD64 1
#define ZLX_UNALIGNED_ACCESS 1
#define ZLX_BITS 64
#define ZLX_LITTLE_ENDIAN 1
#undef ZLX_BIG_ENDIAN
#else 
#define ZLX_UNKNOWN_ARCH 1
#endif

#if ZLX_LE != 0
#define ZLX_LITTLE_ENDIAN 1
#elsif ZLX_LE == 0
#define ZLX_BIG_ENDIAN 1
#endif

#if !ZLX_NO_UNALIGNED_ACCESS
#undef ZLX_UNALIGNED_ACCESS
#endif

#ifdef DOCUMENTATION

/*  ZLX_LITTLE_ENDIAN  */
/**
 *  defined non-zero when compiling for a little endian target
 */
#define ZLX_LITTLE_ENDIAN 0

/*  ZLX_BIG_ENDIAN  */
/**
 *  defined non-zero when compiling for a big endian target
 */
#define ZLX_BIG_ENDIAN 0

/*  ZLX_IA32  */
/**
 *  defined non-zero when compiling for an IA32 target machine
 */
#define ZLX_IA32 0
#undef ZLX_IA32

/*  ZLX_AMD64  */
/**
 *  defined non-zero when compiling for an AMD64 target machine
 */
#define ZLX_AMD64 0
#undef ZLX_AMD64

/*  ZLX_ARM32  */
/**
 *  defined non-zero when compiling for an ARM machine
 */
#define ZLX_ARM32

/*  ZLX_THUMB  */
/**
 *  defined non-zero only when compiling for an ARM machine using just the
 *  Thumb instruction set.
 */
#define ZLX_THUMB

/*  ZLX_ARM64  */
/**
 *  defined non-zero only when compiling for AArch64 (ARM 64-bit)
 */
#define ZLX_ARM64

/*  ZLX_ALPHA32  */
/**
 *  defined non-zero only when compiling for DEC Alpha machines
 */
#define ZLX_ALPHA32

/*  ZLX_HPPA  */
/**
 *  defined non-zero only when compiling for HP/PA-RISC machines
 */
#define ZLX_HPPA

/*  ZLX_ITANIUM  */
/**
 *  defined non-zero only when compiling for Intel IA64 Itanium machines
 */
#define ZLX_ITANIUM

/*  ZLX_M68K  */
/**
 *  defined non-zero only when compiling for Motorola 68000 series machines
 */
#define ZLX_M68K

/*  ZLX_MIPS32  */
/**
 *  defined non-zero only when compiling for MIPS 32-bit machines
 */
#define ZLX_MIPS32

/*  ZLX_POWERPC32  */
/**
 *  defined non-zero only when compiling for IBM PowerPC 32-bit machines
 */
#define ZLX_POWERPC32

/*  ZLX_POWERPC64  */
/**
 *  defined non-zero only when compiling for IBM PowerPC 64-bit machines
 */
#define ZLX_POWERPC64

/*  ZLX_SPARC32  */
/**
 *  defined non-zero only when compiling for SPARC 32-bit machines
 */
#define ZLX_SPARC32

/*  ZLX_SPARC64  */
/**
 *  defined non-zero only when compiling for SPARC 64-bit machines
 */
#define ZLX_SPARC64

#undef ZLX_ARM32
#undef ZLX_THUMB
#undef ZLX_ARM64
#undef ZLX_ALPHA32
#undef ZLX_HPPA
#undef ZLX_ITANIUM
#undef ZLX_M68K
#undef ZLX_MIPS32
#undef ZLX_POWERPC32
#undef ZLX_POWERPC64
#undef ZLX_SPARC32
#undef ZLX_SPARC64

/*  ZLX_BITS  */
/**
 *  Defined to the "native" number of bits for the target architecture.
 *  currently this expands only to 32 or 64.
 */
#define ZLX_BITS 0

/*  ZLX_UNALIGNED_ACCESS  */
/**
 *  Defined non-zero when compiling for targets that support transparent
 *  unaligned accesses.
 */
#define ZLX_UNALIGNED_ACCESS 0

#endif

/** @} */

#endif
