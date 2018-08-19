#ifndef __MACH_PARAM_H__
#define __MACH_PARAM_H__

/*
 * machine-specific param.h headers should declare the following:
 * CHARSIZE     - sizeof(char) in bytes
 * SHORTSIZE    - sizeof(short) in bytes
 * INTSIZE      - sizeof(int) in bytes
 * LONGSIZE     - sizeof(long) in bytes
 * LONGLONGSIZE - sizeof(long long) in bytes
 * PTRSIZE      - sizeof pointer in bytes
 * PTRBITS      - number of bits in pointers
 * ADRBITS      - number of [low] significant bits in memory addresses
 * CLSIZE       - cacheline size
 * PAGESIZE     - virtual memory page size
 */
#if defined(__x86_64__) || defined(__amd64__)
#include <mach/x86-64/param.h>
#elif (defined(__i386__) || defined(__i486__)                           \
       || defined(__i586__) || defined(__i686__))
#include <mach/ia32/param.h>
#elif defined(__arm__)
#include <mach/arm/param.h>
#elif defined(__ppc__)
#include <mach/ppc/param.h>
#endif

#endif /* __MACH_PARAM_H__ */

