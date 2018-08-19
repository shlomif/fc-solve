#ifndef __MACH_X86_64_PARAM_H__
#define __MACH_X86_64_PARAM_H__

/* # of I/O ports */
#define NIOPORT        65536

#define WORDSIZE       8
#define CHARSIZE       1
#define SHORTSIZE      2
#define INTSIZE        4
#if (defined(_MSC_VER)                                                  \
     ||defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)        \
     || defined(__CYGWIN__) || defined(__CYGWIN32__)                    \
     || defined(WIN64) || defined(_WIN64)                               \
     || defined(__WIN64) || defined(__WIN64__)                          \
     || defined(__MINGW32__) || defined(__MINGW64__)                    \
     || defined(_M_AMD64))
#define LONGSIZE       4
#define LONGSIZELOG2   2
#else
#define LONGSIZE       8
#define LONGSIZELOG2   3
#endif
#define LONGLONGSIZE   8
#define PTRSIZE        8
#define PTRSIZELOG2    3
#define PTRBITS        64
#define ADRBITS        48 // # of significant bytes in phys and virt addresses
#define ADRHIBITCOPY   1
#undef  ADRHIBITMACH
#define PAGESIZELOG2   12 // 4-kilobyte pages

#define CLSIZE         64
#define CLSIZELOG2     6
#define PAGESIZE       4096

#define LONGDOUBLESIZE 80

#endif /* __MACH_X86_64_PARAM_H__ */

