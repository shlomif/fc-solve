#!/bin/bash

deals="$(cat /home/shlomif/Download/unpack/games/freecell/freecell-pro-3fc-deals--split/Int3.txt | perl -lnE 'say if $_ > 3e9 and $_ < 4e9' | perl -lnE 'BEGIN { %h = map {/deal = ([0-9]+)/ ? ($1 => 1) : ()} `cat seeds.log.txt`;} say if ! exists $h{$_}')"

for d in $deals ; do
    ./board_gen/pi-make-microsoft-freecell-board -t "$d" > "$d"
done

seed=2
while test -n "$deals" ; do
    echo "Trying seed = $seed"
    new=''
    for d in $deals ; do
        echo "Trying seed = $seed deal=$d"
        if ./fc-solve --method random-dfs -sp r:tf -to '[0123456789ABCDE]' -seed "$seed" -mi 250000 --freecells-num 3 "$d" | grep -E '^This game is solv' ; then
            echo "Found seed = $seed for deal = $d" | tee -a seeds.log.txt
        else
            new+=" $d"
        fi
    done
    let seed++
    deals="$new"
done
