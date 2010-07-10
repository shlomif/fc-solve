#include <stdio.h>

#include "cl_chop.h"

struct myfile_struct
{
    char * filename;
    int offset;
    args_man_t * args_man;
};

struct myfile_struct files[2] = { { "cool-jives.sh" , 4, NULL }, { "fools-gold.sh", 4, NULL } };

int main()
{
    args_man_t * args;
    FILE * f;
    char buffer[10000];
    int a;

    args = freecell_solver_args_man_alloc();

    f = fopen("/usr/share/freecell-solver/presets/cool-jives.sh", "r");
    memset(buffer, '\0', sizeof(buffer));
    fread(buffer, 1, sizeof(buffer), f);
    fclose(f);

    freecell_solver_args_man_chop(args, buffer);

    freecell_solver_args_man_chop(args, "-ni");

    f = fopen("/usr/share/freecell-solver/presets/fools-gold.sh", "r");
    memset(buffer, '\0', sizeof(buffer));
    fread(buffer, 1, sizeof(buffer), f);
    fclose(f);

    freecell_solver_args_man_chop(args, buffer);

    for(a=0;a<args->argc;a++)
    {
        printf("%s\n", args->argv[a]);
    }

    freecell_solver_args_man_free(args);
    
    return 0;
}
