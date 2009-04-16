all: test

BOARD = 24

fill_pos.cpp: 
	pi-make-microsoft-freecell-board -t $(BOARD) | perl fill_pos.pl > fill_pos.cpp

test: fc_pro_iface.c fill_pos.cpp main.c
	gcc -DSHLOMIF_DEBUG -g -o test -Wall -I /usr/include/freecell-solver fc_pro_iface.c main.c -lfreecell-solver -lm
