bin_PROGRAMS = make-gnome-freecell-board make-aisleriot-freecell-board \
          make-microsoft-freecell-board pi-make-microsoft-freecell-board
          
bin_SCRIPTS = make_pysol_freecell_board.py

man_MANS = fc-solve-board_gen.6 \
    make-gnome-freecell-board.6 \
    make_pysol_freecell_board.py.6 \
    make-aisleriot-freecell-board.6 \
    pi-make-microsoft-freecell-board.6

make_gnome_freecell_board_SOURCES = make_gnome_freecell_board.c
make_aisleriot_freecell_board_SOURCES = make_aisleriot_freecell_board.c
make_microsoft_freecell_board_SOURCES = make_microsoft_freecell_board.c
pi_make_microsoft_freecell_board_SOURCES = pi_make_microsoft_freecell_board.c

EXTRA_DIST += make_pysol_freecell_board.py

fc-solve-board_gen.6: fc-solve-board_gen.pod ../ver.txt
	cat $< | sed 's/FCS_VERSION/@VERSION@/g' > fc-solve.pod
	pod2man --section=6 --center="board_gen" --date="2001/07/07" --release="board_gen @VERSION@" fc-solve.pod > $@
	rm -f fc-solve.pod

EXTRA_DIST += fc-solve-board_gen.pod fc-solve-board_gen.6

EXTRA_DIST += Makefile.unix Makefile.win32 make_pysol_freecell_board.c

EXTRA_DIST += create_man_links.sh

EXTRA_DIST += make-gnome-freecell-board.6 make_pysol_freecell_board.py.6
EXTRA_DIST += make-aisleriot-freecell-board.6 pi-make-microsoft-freecell-board.6



