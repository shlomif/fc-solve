#ifndef __MACH_ARM_PARAM_H__
#define __MACH_ARM_PARAM_H__

#if !defined(__arm64__) && !defined(__aarch64__)
#define WORDSIZE     4
#define CHARSIZE     1
#define SHORTSIZE    2
#define INTSIZE      4
#define LONGSIZE     4
#define LONGSIZELOG2 2
#define LONGLONGSIZE 8
#define PTRSIZE      4
#define PTRBITS      32
#define ADRBITS      32
#define PAGESIZELOG2 12
#else
#error arm64 not supported by <zero/param.h> yet
#endif

#define CLSIZE       32
#define CLSIZELOG2   5
#define PAGESIZE     4096

#endif /* __MACH_ARM_PARAM_H__ */

