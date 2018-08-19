#ifndef __MACH_ASM_H__
#define __MACH_ASM_H__

/*
 * TODO
 * ----
 * - m_cmpswapbit() - bit-level atomic compare and swap
 */

#include <mach/atomic.h>

/* for systems without GNU C, we define these in e.g. <mach/ia32/asm.h> */
#if defined(__GNUC__)
/* stack frames */
#define m_atomread(a)      (*((typeof(a))(a)))
#define m_atomwrite(a, v)  (*((typeof(v) *)(a)) = (v))
#define m_syncread(a, res)                                              \
    do {                                                                \
        m_memwrbar();                                                   \
        (res) = m_atomread(a);                                          \
    } while (0)
#define m_syncwrite(a, val)                                             \
    do {                                                                \
        m_memwrbar();                                                   \
        m_atomwrite(a, val);                                            \
    } while (0)
#endif

#include <mach/atomic.h>
#if defined(_WIN64)
#include <zero/msc/win64.h>
#elif defined(_WIN32)
#include <zero/msc/win32.h>
#elif (defined(__x86_64__) || defined(__amd64__)                      \
       || (defined(__i386__) || defined(__i486__)                     \
           || defined(__i586__) || defined(__i686__)))
#include <mach/x86/asm.h>
#elif defined(__arm__)
#include <mach/arm/asm.h>
#elif defined(__ppc__)
#include <mach/ppc/asm.h>
#endif

#endif /* __MACH_ASM_H__ */

