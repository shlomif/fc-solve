#ifndef __MACH_ARM_ASM_H__
#define __MACH_ARM_ASM_H__

#include <mach/types.h>

/* API declarations */
#define m_membar()   __asm__ __volatile__ ("" : : : "memory")
#if defined(__ARM_ARCH_7__)
/* TODO: are these ARMv7? */
#define m_waitspin() __asm__ __volatile__ ("wfe\n")
#define m_endspin()  __asm__ __volatile__ ("sev\n")
#define m_waitint()  __asm__ __volatile__ ("wfi\n")
#elif defined(__ARM_ARCH_4__) || defined (__ARM_ARCH_4T__)
#define m_waitint()  __asm__ __volatile__ ("nop\n")
#else
#define m_waitint()  __asm__ __volatile__ ("mov r0, #0\n"                            "mcr p15, #0, r0, c7, c0, #4\n")
#endif

static __inline__ m_atomic_t
m_cmpswap_armv6(volatile m_atomic_t *p,
                m_atomic_t want,
                m_atomic_t val)
{
    m_atomic_t res = 0;

    __asm__ __volatile__ ("ldr r1, %1\n"                // r1 = want;
                          "mov %0, #0\n"                // res = 0;
                          "0: ldrex r2, [%2]\n"         // r2 = *p;
                          "cmp r2, r1\n"                // is *p == want?
                          "bne 1f\n"                    // if not, wait
                          "strexne r2, r1, [%3]\n"      // r2
                          "cmpne r2, %3\n"
                          "beq 0b\n"
                          "dmb\n"
                          "mov %0, %3\n"
                          "bx lr\n"
                          "1: wfi\n"
                          "b 0b\n"
                          : "=r" (res)
                          : "r" (want), "r" (p), "r" (val));

    return res;
}

static __inline__ m_atomic_t
m_cmpswap(volatile m_atomic_t *p,
          m_atomic_t want,
          m_atomic_t val)
{
    m_atomic_t res = 0;
    m_atomic_t tmp;

    __asm__ __volatile__ ("0:\n"
                          "ldr %1, [%2]\n"
                          "mov %0, #0\n"
                          "cmp %1, %4\n"
                          "bne 1f\n"
                          "swp %0, %0, [%2]\n"
                          "cmp %1, %0\n"
                          "swpne %1, %0, [%2]\n"
                          "bne 0b\n"
                          "mov %0, %3\n"
                          "1:\n"
                          : "=&r" (res), "=&r" (tmp)
                          : "r" (p), "r" (val), "r" (want)
                          : "cc", "memory");

    return res;
}

#endif /* __MACH_ARM_ASM_H__ */

