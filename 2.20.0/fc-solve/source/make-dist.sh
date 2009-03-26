#!/bin/sh

PROG="freecell-solver"
VER=`cat ver.txt`

if [ ! -e ../$PROG-$VER ] ; then
	mkdir ../$PROG-$VER
fi

if [ ! -e ../$PROG-$VER/board_gen ] ; then
    mkdir ../$PROG-$VER/board_gen
fi

if [ ! -e ../$PROG-$VER/devel_docs ] ; then
    mkdir ../$PROG-$VER/devel_docs
fi

cp Makefile Makefile.lite *.c *.h ChangeLog  \
	README TODO INSTALL USAGE CREDITS gen_makefile.pl \
    Makefile.win32 gen_presets.pl \
    config.h.freecell config.h.dkds fc-solve.pod fc-solve.pre.pod \
    change_ver.sh \
	../$PROG-$VER/

cp  ./board_gen/*.c ./board_gen/*.py \
    ./board_gen/Makefile ./board_gen/COPYING ./board_gen/README \
    ./board_gen/Makefile.win32 ./board_gen/fc-solve-board_gen.pod \
    ./board_gen/fc-solve-board_gen.pre.pod \
    ./board_gen/*.6 ./board_gen/create_man_links.sh
        ../$PROG-$VER/board_gen/

cp ./devel_docs/*Arch*.txt \
        ../$PROG-$VER/devel_docs/

(cd .. && tar -czvf Arcs/$PROG-$VER.tar.gz $PROG-$VER/)

rm -fr ../$PROG-$VER
