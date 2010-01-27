#!/bin/sh

start="$1"
end="$2"

seq "$start" "$end" |  \
(while read A ; do
    echo "$A:"
    ./board_gen/make_pysol_freecell_board.py "$A" simple_simon |
        ./fc-solve --game simple_simon -ni -to abcdefghi -p -t -mi 1500000 | 
        tail -3
done) | tee -a total_dump.txt

