#ifndef __LOOKUP2_H
#define __LOOKUP2_H

typedef  unsigned long  int  ub4;   /* unsigned 4-byte quantities */
typedef  unsigned       char ub1;

ub4 freecell_solver_lookup2_hash_function(
    register ub4 *k,        /* the key */
    register ub4  length,  /* the length of the key, in ub4s */
    register ub4  initval  /* the previous hash, or an arbitrary value */
    );


#endif /* __LOOKUP2_H */
