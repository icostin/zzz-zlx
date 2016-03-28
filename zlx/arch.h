#ifndef _ZLX_ARCH_H
#define _ZLX_ARCH_H

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

#undef ZLX_LE
#undef ZLX_BE

#undef ZLX_UNALIGNED_ACCESS

#if _M_IX86 || _M_I86 || __X86__ || _X86_ || __i386__ || __i386 
#define ZLX_IA32 1
#define ZLX_UNALIGNED_ACCESS 1
#define ZLX_BITS 32
#elif _M_X64 || _M_AMD64 || __amd64__ || __x86_64
#define ZLX_AMD64 1
#define ZLX_UNALIGNED_ACCESS 1
#define ZLX_BITS 64
#endif

#if ZLX_LE
#define ZLX_LITTLE_ENDIAN 1
#else
#define ZLX_BIG_ENDIAN 1
#endif

#if !ZLX_NO_UNALIGNED_ACCESS
#undef ZLX_UNALIGNED_ACCESS
#endif

#endif