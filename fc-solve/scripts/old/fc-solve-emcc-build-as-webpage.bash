#!/bin/bash

if ! test -d build ; then
    mkdir build
fi

cd build
../Tatzer -l x64_pysol
make boards
OBJS="../app_str.c ../scans.c ../main.c ../lib.c ../preset.c ../instance.c ../move_funcs_order.c  ../move_funcs_maps.c ../meta_alloc.c ../cmd_line.c ../card.c ../state.c ../check_and_add_state.c ../fcs_hash.c ../split_cmd_line.c ../simpsim.c ../freecell.c ../move.c ../fc_pro_iface.c ../rate_state.c"

# emcc -m32 -std=gnu99 -g -o fc-solve.js -I. $OBJS --embed-file 24.board
emcc -s TOTAL_MEMORY=$((128 * 1024 * 1024)) -O2 -m32 -std=gnu99 -o fc-solve-test.html -I. $OBJS --embed-file 24.board --preload-file 24.board
# gcc -std=gnu99 -g -o fc-solve.exe -I. $OBJS

if false ; then
    echo "Running node.js fc-solve"
    echo "[[[[["
    node fc-solve.js -s -i -p -t 24.board
    echo "]]]]]"

    echo "Running the gcc-built fc-solve"
    echo "[[[[["
    ./fc-solve.exe -s -i -p -t 24.board
    echo "]]]]]"
fi
