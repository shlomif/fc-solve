/*
 * inline.h - the purpose of this file is to define the GCC_INLINE
 * macro.
 *
 * Written by Shlomi Fish, 2002
 *
 * This file is in the public domain (it's uncopyrighted).
 * */

#ifndef __INLINE_H
#define __INLINE_H

#if (defined(__GNUC__) && (!defined(__STRICT_ANSI__))) || defined(HAVE_C_INLINE)
#define GCC_INLINE inline
#else
#define GCC_INLINE
#endif


#endif
