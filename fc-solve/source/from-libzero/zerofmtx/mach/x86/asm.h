#ifndef __MACH_X86_ASM_H__
#define __MACH_X86_ASM_H__

//#define frameisusr(tcb) ((tcb)->frame.cs == UTEXTSEL)

#define m_membar()   __asm__ __volatile__ ("mfence\n" : : : "memory")
/* memory read barrier */
#define m_memrdbar() __asm__ __volatile__ ("lfence\n" : : : "memory")
/* memory write barrier */
#define m_memwrbar() __asm__ __volatile__ ("sfence\n" : : : "memory")
/* delay exection */
#define m_waitspin() __asm__ __volatile__ ("pause\n"  : : : "memory")
#define m_endspin()
/* wait for an interrupt */
#define m_waitint()  __asm__ __volatile__ ("hlt\n"  : : : "memory")

#include <stdint.h>
#include <zero/cdefs.h>
#include <mach/ia32/asm.h>
#if defined(__x86_64__) || defined(__amd64__)
#include <mach/x86-64/asm.h>
#endif

/* atomic fetch and add, 16-bit version */
#define m_fetchadd16(p, val)       m_xadd16(p, val)
#define m_fetchaddu16(p, val)      m_xaddu16(p, val)
/* atomic fetch and add, 32-bit version */
#define m_fetchadd32(p, val)       m_xadd32(p, val)
#define m_fetchaddu32(p, val)      m_xaddu32(p, val)
/* atomic compare and swap byte */
#define m_cmpswapb(p, want, val)   (m_cmpxchg8(p, want, val) == want)
#define m_cmpswap32(p, want, val)  (m_cmpxchg32(p, want, val) == want)
#define m_cmpswapu32(p, want, val) (m_cmpxchgu32(p, want, val) == want)

#endif /* __MACH_X86_ASM_H__ */

