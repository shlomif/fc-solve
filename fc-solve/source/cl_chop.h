
#ifndef __CMD_LINE_CHOP_H
#define __CMD_LINE_CHOP_H

struct args_man_struct
{
    int argc;
    char * * argv;
    int max_num_argv;
};

typedef struct args_man_struct args_man_t;

extern args_man_t * freecell_solver_args_man_alloc(void);
extern void freecell_solver_args_man_free(args_man_t * manager);
extern int freecell_solver_args_man_chop(args_man_t * manager, char * string);

#endif /* #ifndef __CMD_LINE_CHOP_H */

