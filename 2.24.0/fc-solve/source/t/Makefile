all: t/card-test-render.exe t/card-test-parse.exe

CFLAGS = -Wall -g

card-test-render.c: card-test-render.c.tt generate-card-tests.pl
	perl generate-card-tests.pl

t/card-test-render.exe: card-test-render.c ../card.c
	gcc $(CFLAGS) -o "$@" $< -ltap

card-test-parse.c: card-test-parse.c.tt generate-card-tests.pl
	perl generate-card-tests.pl

t/card-test-parse.exe: card-test-parse.c ../card.c
	gcc $(CFLAGS) -o "$@" $< -ltap

test: all
	runprove t/*.{exe,t}

clean:
	rm -f t/card-test-parse.exe t/card-test-render.exe
