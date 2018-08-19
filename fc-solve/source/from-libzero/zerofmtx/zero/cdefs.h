#ifndef __ZERO_CDEFS_H__
#define __ZERO_CDEFS_H__

/* size for 'empty' array (placeholder) (VLAs aka variable-length arrays) */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#   define VLA
#else
#   define VLA 0
#endif

/* __func__ for non-C99 compilers; MSVC should do this, probably =) */
#if !defined(__STDC_VERSION__) || (__STDC_VERSION__ < 199901L) \
    && (defined(__GNUC__) || defined(_MSC_VER))
#define __func__ __FUNCTION__
#endif

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L) && 0
#include <stdalign.h>
#define ALIGNED(x)  alignas(x)
#if !defined(__STDC_NO_THREADS__)
#include <threads.h>
#define THREADLOCAL _Thread_local
#endif
#endif
#include <zero/pack.h>

#if defined(__GNUC__) || defined(__clang__)

/* immediate argument test */
#define IMMEDIATE(x) __builtin_constant_p(x)
/* align variables, aggregates, and tables to boundary of a */
#define ALIGNED(a)   __attribute__ ((__aligned__(a)))
#define ALIGNOF(x)   __alignof__ (x)
/* ALWAYS inline the function */
#define INLINE       __inline__ __attribute__ ((__always_inline__))
/* do NOT inline the function */
#define NOINLINE     __attribute__ ((__noinline__))
#define RESTRICT     __restrict
#define WEAK         __attribute__ ((__weak__))
#define WEAKALIAS(n) __attribute__ ((weak, alias(#n)))
#define UNUSED       __attribute__ ((unused))
/*
 * AMD64 passes first six arguments in rdi, rsi, rdx, rcx, r8, and r9; the rest
 * are pushed to stack in reverse order
 *
 * IA-32 can pass up to 3 register arguments in eax, edx, and ecx
 */
#define REGPARM(n)   __attribute__ ((__regparm__(n)))
#if defined(__x86_64__) || defined(__amd64__) || defined(__arm__)
#define FASTCALL    /* at least 3 arguments passed in registers by default */
#elif defined(__i386__) || defined(__i486__) || defined(__i586__) \
     || defined(__i686__)
#define FASTCALL     REGPARM(3)
#endif

/* pass all arguments on stack for assembly-linkage */
#define ASMLINK      REGPARM(0)

/* declare function with no return (e.g., longjmp()) */
#define NORETURN     __attribute__ ((__noreturn__))
#define INTERRUPT    __attribute__ ((__interrupt__))
//#define INTERRUPTJMP INTERRUPT __attribute__ ((__no_caller_saved_registers__))
#define PURE         __attribute__ ((__pure__))
#define CONST        __attribute__ ((__const__))

#define likely(x)    (__builtin_expect(!!(x), 1))
#define unlikely(x)  (__builtin_expect(!!(x), 0))
#define isconst(x)   (__builtin_constant_p(x))

#elif defined(_MSC_VER) /* !defined(__GNUC__) && !defined(__clang__) */

#define ASMLINK      __stdcall

/* Microsoft */
#define ALIGNED(a)   __declspec(align((a)))
#define ALIGNOF(x)   __alignof(x)
#define __inline__   inline
#define INLINE       __forceinline
#define NOINLINE     __declspec(noinline)
#define NOALIAS      __declspec(noalias)

#define ASMLINK      __cdecl

#define NORETURN     __declspec(noreturn)

#define THREADLOCAL  __declspec(thread)

#endif /* __GNUC__ || __clang__ || _MSC_VER */

#if !defined(UNUSED)
#define UNUSED
#endif
#if !defined(THREADLOCAL)
/* declare thread-local data */
#define THREADLOCAL  __thread
#endif

#endif /* __ZERO_CDEFS_H__ */

