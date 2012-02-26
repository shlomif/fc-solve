#!/bin/bash

board_num=17760
iters_limit=23000000
board_file="$board_num.board"

if ! test -e "$board_file" ; then
    pi-make-microsoft-freecell-board -t "$board_num" > "$board_file"
fi

let seed=1
while true ; do
    echo "Checking seed $seed"
    if ./fc-solve --freecells-num 2 --method random-dfs -to '[01ABCDEF]' -seed "$seed" -mi "$iters_limit" "$board_file" | grep -P '^This game is solveable' ; then
        echo "Seed '$seed' is successful."
        exit 0
    fi
    let seed++
done
