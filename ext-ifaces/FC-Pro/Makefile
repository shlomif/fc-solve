all: test

test: fc_pro_iface.c fill_pos.cpp main.c
	gcc -DSHLOMIF_DEBUG -g -o test -Wall -I /usr/include/freecell-solver fc_pro_iface.c main.c -L. -lfcs -lm
