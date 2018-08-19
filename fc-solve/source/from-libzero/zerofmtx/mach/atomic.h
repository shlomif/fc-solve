#ifndef __MACH_ATOMIC_H__
#define __MACH_ATOMIC_H__

#include <stdint.h>
#include <mach/param.h>

/* atomic types */
#if (PTRSIZE == 8)
typedef int64_t    m_atomic_t;
#elif (PTRSIZE == 4)
typedef int32_t    m_atomic_t;
#endif
typedef int8_t     m_atomic8_t;
typedef uint8_t    m_atomicu8_t;
typedef int16_t    m_atomic16_t;
typedef uint16_t   m_atomicu16_t;
typedef int32_t    m_atomic32_t;
typedef uint32_t   m_atomicu32_t;
typedef int64_t    m_atomic64_t;
typedef uint64_t   m_atomicu64_t;
typedef void      *m_atomicptr_t;
typedef int8_t    *m_atomicptr8_t;
typedef uintptr_t  m_atomicadr_t;

#endif /* __MACH_ATOMIC_H__ */

