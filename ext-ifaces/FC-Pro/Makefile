all: test

test: fc_pro_iface.c fill_pos.cpp
	gcc -o test -Wall -I /usr/include/freecell-solver fc_pro_iface.c -lfreecell-solver -lm
