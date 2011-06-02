
CC = gcc
OFLAGS = -Wall -O3

TARGETS = make-gnome-freecell-board make-aisleriot-freecell-board \
          make-microsoft-freecell-board pi-make-microsoft-freecell-board

UNBUILT_TARGETS = make-pysol-freecell-board

all: $(TARGETS)

all2: $(TARGETS) $(UNBUILT_TARGETS)

make-gnome-freecell-board: make_gnome_freecell_board.c
	$(CC) $(OFLAGS) -o $@ $<

make-aisleriot-freecell-board: make_aisleriot_freecell_board.c
	$(CC) $(OFLAGS) -o $@ $<

make-microsoft-freecell-board: make_microsoft_freecell_board.c
	$(CC) $(OFLAGS) -o $@ $<

pi-make-microsoft-freecell-board: pi_make_microsoft_freecell_board.c
	$(CC) $(OFLAGS) -o $@ $<

make-pysol-freecell-board: make_pysol_freecell_board.c
	$(CC) $(OFLAGS) -o $@ $<	

clean:
	rm -f $(TARGETS) $(UNBUILT_TARGETS)
