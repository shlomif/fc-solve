#include <stdio.h>
#include <stdlib.h>

#include "Fcsolvex.h"
extern char * moves_string;

extern int Free2Solver(Position * orig, int NoFcs, int limit, int cmd_line_argc, char * * cmd_line_argv,
	 int (*signal_step)(int step_limit));

static int my_signal_step(int step_limit)
{
    printf("num_iters=%i\n", step_limit);
    return 0;
}
int main(int argc, char * argv[])
{
    Position mypos;
    int ret;

#include "fill_pos.cpp"

    ret = Free2Solver(&mypos, 4, atoi(argv[1]), argc-2, argv+2, my_signal_step);

    printf("%i\n", ret);

    printf("%s\n", moves_string);

    return 0;
}

