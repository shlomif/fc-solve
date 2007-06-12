all: t/card-test-render.exe

card-test-render.c: card-test-render.c.tt generate-card-tests.pl
	perl generate-card-tests.pl

t/card-test-render.exe: card-test-render.c
	gcc -o "$@" $< -ltap

test:
	runprove t/*
