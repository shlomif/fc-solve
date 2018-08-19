#ifndef __MACH_IA32_ASM_H__
#define __MACH_IA32_ASM_H__

#include <stdint.h>
#include <zero/cdefs.h>

#define __EIPFRAMEOFS                4

#if !defined(__x86_64__) && !defined(__amd64__)

#define m_atominc(p)                 m_atominc32(p)
#define m_atomdec(p)                 m_atomdec32(p)
#define m_atomswap(p, val)           m_xchg32(p, val)
#define m_fetchadd(p, val)           m_xadd32(p, val)
#define m_fetchswap(p, want, val)    m_cmpxchg32(p, want, val)
#define m_fetchswapu(p, want, val)   m_cmpxchgu32(p, want, val)
#define m_fetchswapptr(p, want, val) m_cmpxchg32ptr(p, want, val)
#define m_cmpswap(p, want, val)      (m_cmpxchg32(p, want, val) == want)
#define m_cmpswapu(p, want, val)     (m_cmpxchgu32(p, want, val) == want)
#define m_cmpswapptr(p, want, val)   (m_cmpxchg32ptr(p, want, val) == want)
#define m_cmpswapdbl(p, want, val)   (m_cmpxchg64(p, val))
#define m_setbit(p, ndx)             m_setbit32(p, ndx)
#define m_clrbit(p, ndx)             m_clrbit32(p, ndx)
#define m_flipbit(p, ndx)            m_flipbit32(p, ndx)
#define m_cmpsetbit(p, ndx)          m_cmpsetbit32(p, ndx)
#define m_cmpclrbit(p, ndx)          m_cmpclrbit32(p, ndx)
#define m_scanlo1bit(l)              m_bsf32(l)
#define m_scanhi1bit(l)              m_bsr32(l)

static INLINE void *
m_getretadr(void)
{
    void *ptr;

    __asm__ __volatile__ ("movl %c1(%%ebp), %0\n"
                          : "=r" (ptr)
                          : "i" (__EIPFRAMEOFS));
    return ptr;
}

static INLINE void *
m_getfrmadr(void)
{
    void *ptr;

    __asm__ __volatile__ ("movl %%ebp, %0\n" : "=r" (ptr));

    return ptr;
}

static INLINE void *
m_getfrmadr1(void **fp)
{
    void *ptr;

    if (fp) {
        ptr = *fp;
    } else {
        ptr = m_getfrmadr();
    }

    return ptr;
}

static INLINE void *
m_getretfrm(void)
{
    void *ptr;

    __asm__ __volatile__ ("movl *%%ebp, %0\n" : "=r" (ptr));

    return ptr;
}

static INLINE void *
m_loadretadr(void *frm)
{
    void *ptr;

    __asm__ __volatile__ ("movl %c1(%2), %0\n"
                          : "=r" (ptr)
                          : "i" (__EIPFRAMEOFS), "r" (frm));

    return ptr;
}

#endif /* !64-bit */

/* atomic increment operation */
static __inline__ void
m_atominc32(volatile m_atomic32_t *p)
{
    __asm__ __volatile__ ("lock incl %0\n"
                          : "+m" (*(p))
                          :
                          : "memory");

    return;
}

/* atomic decrement operation */
static __inline__ void
m_atomdec32(volatile m_atomic32_t *p)
{
    __asm__ __volatile__ ("lock decl %0\n"
                          : "+m" (*(p))
                          :
                          : "memory");

    return;
}

/* atomic exchange operation */
static __inline__ int32_t
m_xchg32(volatile m_atomic32_t *p,
         int32_t val)
{
    int32_t res;

    __asm__ __volatile__ ("lock xchgl %0, %2\n"
                          : "+m" (*p), "=a" (res)
                          : "r" (val)
                          : "cc", "memory");

    return res;
}

/*
 * atomic add
 * - let *p = *p + val
 * - return original *p
 */
static __inline__ m_atomic16_t
m_add16(volatile m_atomic16_t *p,
         m_atomic16_t val)
{
    __asm__ __volatile__ ("lock addw %1, %w0\n"
                          : "+m" (*(p)), "=a" (val)
                          :
                          : "cc", "memory");

    return val;
}

/*
 * atomic add
 * - let *p = *p + val
 * - return original *p
 */
static __inline__ m_atomicu16_t
m_addu16(volatile m_atomicu16_t *p,
          m_atomicu16_t val)
{
    __asm__ __volatile__ ("lock addw %1, %w0\n"
                          : "+m" (*(p)), "=a" (val)
                          :
                          : "cc", "memory");

    return val;
}

/*
 * atomic add
 * - let *p = *p + val
 * - return original *p
 */
static __inline__ int32_t
m_add32(volatile m_atomic32_t *p,
         int32_t val)
{
    __asm__ __volatile__ ("lock addl %1, %0\n"
                          : "+m" (*(p)), "=a" (val)
                          :
                          : "cc", "memory");

    return val;
}

/*
 * atomic add
 * - let *p = *p + val
 * - return original *p
 */
static __inline__ m_atomicu32_t
m_addu32(volatile m_atomicu32_t *p,
          m_atomicu32_t val)
{
    __asm__ __volatile__ ("lock addl %1, %0\n"
                          : "+m" (*(p)), "=a" (val)
                          :
                          : "cc", "memory");

    return val;
}

/*
 * atomic fetch and add
 * - let *p = *p + val
 * - return original *p
 */
static __inline__ m_atomic16_t
m_xadd16(volatile m_atomic16_t *p,
         m_atomic16_t val)
{
    __asm__ __volatile__ ("lock xaddw %1, %w0\n"
                          : "+m" (*(p)), "=a" (val)
                          :
                          : "cc", "memory");

    return val;
}

/*
 * atomic fetch and add
 * - let *p = *p + val
 * - return original *p
 */
static __inline__ m_atomicu16_t
m_xaddu16(volatile m_atomicu16_t *p,
          m_atomicu16_t val)
{
    __asm__ __volatile__ ("lock xaddw %1, %w0\n"
                          : "+m" (*(p)), "=a" (val)
                          :
                          : "cc", "memory");

    return val;
}

/*
 * atomic fetch and add
 * - let *p = *p + val
 * - return original *p
 */
static __inline__ int32_t
m_xadd32(volatile m_atomic32_t *p,
         int32_t val)
{
    __asm__ __volatile__ ("lock xaddl %1, %0\n"
                          : "+m" (*(p)), "=a" (val)
                          :
                          : "cc", "memory");

    return val;
}

/*
 * atomic fetch and add
 * - let *p = *p + val
 * - return original *p
 */
static __inline__ m_atomicu32_t
m_xaddu32(volatile m_atomicu32_t *p,
          m_atomicu32_t val)
{
    __asm__ __volatile__ ("lock xaddl %1, %0\n"
                          : "+m" (*(p)), "=a" (val)
                          :
                          : "cc", "memory");

    return val;
}

/*
 * atomic compare and exchange longword
 * - if *p == want, let *p = val
 * - return nonzero on success, zero on failure
 */
static __inline__ int32_t
m_cmpxchg32(volatile m_atomic32_t *p,
            int32_t want,
            int32_t val)
{
    int32_t res;

    __asm__ __volatile__("lock cmpxchgl %1, %2\n"
                         : "=a" (res)
                         : "q" (val), "m" (*(p)), "0" (want)
                         : "memory");

    return res;
}

/*
 * atomic compare and exchange unsigned longword
 * - if *p == want, let *p = val
 * - return nonzero on success, zero on failure
 */
static __inline__ m_atomicu32_t
m_cmpxchgu32(volatile m_atomicu32_t *p,
             m_atomicu32_t want,
             m_atomicu32_t val)
{
    m_atomicu32_t res;

    __asm__ __volatile__ ("lock cmpxchgl %1, %2\n"
                          : "=a" (res)
                          : "q" (val), "m" (*(p)), "0" (want)
                          : "memory");

    return res;
}

/* atomic set bit operation */
static INLINE void
m_setbit32(volatile m_atomic32_t *p, int32_t ndx)
{
    __asm__ __volatile__ ("lock btsl %1, %0\n"
                          : "=m" (*(p))
                          : "Ir" (ndx)
                          : "memory");

    return;
}

/* atomic reset/clear bit operation */
static INLINE void
m_clrbit32(volatile m_atomic32_t *p, int32_t ndx)
{
    int32_t mask = ~(INT32_C(1) << ndx);

    __asm__ __volatile__ ("lock andl %1, %0\n"
                          : "=m" (*((uint8_t *)(p) + (ndx >> 3)))
                          : "Ir" (mask));

    return;
}

/* atomic flip/toggle bit operation */
static INLINE void
m_flipbit32(volatile m_atomic32_t *p, int32_t ndx)
{
    int32_t bit = INT32_C(1) << ndx;

    __asm__ __volatile__ ("lock orl %1, %0\n"
                          : "=m" (*((uint8_t *)(p) + (ndx >> 3)))
                          : "Ir" (bit));

    return;
}

/* atomic set and test bit operation; returns the old value */
static __inline__ int32_t
m_cmpsetbit32(volatile m_atomic32_t *p, int32_t ndx)
{
    int32_t val = 0;

    if (IMMEDIATE(ndx)) {
        __asm__ __volatile__ ("lock btsl %2, %0\n"
                              "jnc 1f\n"
                              "incl %1\n"
                              "1:\n"
                              : "+m" (*(p)), "=r" (val)
                              : "i" (ndx)
                              : "cc", "memory");
    } else {
        __asm__ __volatile__ ("lock btsl %2, %0\n"
                              "jnc 1f\n"
                              "incl %1\n"
                              "1:\n"
                              : "+m" (*(p)), "=r" (val)
                              : "r" (ndx)
                              : "cc", "memory");
    }

    return val;
}

/* atomic clear bit operation */
static __inline__ int32_t
m_cmpclrbit32(volatile m_atomic32_t *p, int32_t ndx)
{
    int32_t val = 0;

    if (IMMEDIATE(ndx)) {
        __asm__ __volatile__ ("lock btrl %2, %0\n"
                              "jnc 1f\n"
                              "incl %1\n"
                              "1:\n"
                              : "+m" (*(p)), "=r" (val)
                              : "i" (ndx)
                              : "cc", "memory");
    } else {
        __asm__ __volatile__ ("lock btrl %2, %0\n"
                              "jnc 1f\n"
                              "incl %1\n"
                              "1:\n"
                              : "+m" (*(p)), "=r" (val)
                              : "r" (ndx)
                              : "cc", "memory");
    }

    return val;
}

#if defined(__GNUC__)
#define m_atomor(p, val)  __sync_or_and_fetch((p), (val))
#define m_atomand(p, val) __sync_and_and_fetch((p), (val))
#endif

/*
 * atomic compare and exchange byte
 * - if *p == want, let *p = val
 * - return original *p
 */
static __inline__ int8_t
m_cmpxchg8(volatile m_atomic64_t *p,
           int8_t want,
           int8_t val)
{
    long res;

    __asm__ __volatile__ ("lock cmpxchgb %b1, %2\n"
                          : "=a" (res)
                          : "q" (val), "m" (*(p)), "0" (want)
                          : "memory");

    return res;
}

static __inline__ int32_t
m_bsf32(m_atomicu32_t val)
{
    int32_t ret = ~0;

    __asm__ __volatile__ ("bsfl %1, %0\n" : "=r" (ret) : "rm" (val));

    return ret;
}

static __inline__ int32_t
m_bsr32(m_atomicu32_t val)
{
    int32_t ret = ~0;

    __asm__ __volatile__ ("bsrl %1, %0\n" : "=r" (ret) : "rm" (val));

    return ret;
}

/*
 * atomic compare and exchange pointer
 * - if *p == want, let *p = val
 * return nonzero on success, zero on failure
 */
static __inline__ void *
m_cmpxchg32ptr(m_atomic32_t **p,
               m_atomic32_t *want,
               m_atomicptr_t *val)
{
    void *res;

    __asm__ __volatile__("lock cmpxchgl %1, %2\n"
                         : "=a" (res)
                         : "q" (val), "m" (*(p)), "0" (want)
                         : "memory");

    return res;
}

#if !defined(__x86_64__) && !defined(__amd64__)

#if defined(__GNUC__) && 0

/*
 * atomic 64-bit compare and swap
 * - if *p == want, let *p = val
 * - return original nonzero on success, zero on failure
 */
static __inline__ long
m_cmpxchg64(int64_t *p64,
            int64_t *want,
            int64_t *val)
{
    return __sync_bool_compare_and_swap(p64, want, val);
}

#elif defined(_MSC_VER)

static __inline__ long
m_cmpxchg64(int64_t *p64,
            int64_t *want,
            int64_t *val)
{
    long long ask = *want;
    long long say = *val;
    long long res;

    res = InterlockedCompareExchange64(p64, say, ask);

    return res;
}

#else

/*
 * atomic 64-bit compare and swap
 * - if *p == want, let *p = val + set ZF
 *   - else let EDX:EAX = *p + clear ZF
 * - return 0 on failur, 1 on success

 */
static __inline__ long
m_cmpxchg64(int64_t *ptr,
            int64_t want,
            int64_t val)
{
    long     res = 0;
    register int32_t ebx __asm__ ("ebx") = want & 0xffffffff;

    __asm__ __volatile (
        "movl %%edi, %%ebx\n" // load EBX
        "lock cmpxchg8b (%%esi)\n"
        "setz %%al\n" // EAX may change
        "movzx %%al, %1\n" // res = ZF
        : "+S" (ptr), "=a" (res)
        : "0" (ptr),
          "d" ((uint32_t)(want >> 32)),
          "a" ((uint32_t)(want & 0xffffffff)),
          "c" ((uint32_t)(val >> 32)),
          "D" ((uint32_t)(val & 0xffffffff))
        : "flags", "memory", "eax", "edx");

    return res;
}

#endif

#endif /* !64-bit */

#endif /* __MACH_IA32_ASM_H__ */

