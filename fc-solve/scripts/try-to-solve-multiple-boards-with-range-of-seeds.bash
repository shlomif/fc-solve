#!/bin/bash

calc_deals()
{
    deals="$(cat /home/shlomif/Download/unpack/games/freecell/freecell-pro-3fc-deals--split/Int3.txt | perl -lnE 'say if $_ > 3e9 and $_ < 4e9' | perl -lnE 'BEGIN { %h = map {/deal = ([0-9]+)/ ? ($1 => 1) : ()} `cat seeds.log.txt`;} say if ! exists $h{$_}')"
}

calc_deals

for d in $deals ; do
    echo "Generating $d"
    if ! test -e "$d" ; then
        ./board_gen/pi-make-microsoft-freecell-board -t "$d" > "$d"
    fi
done

seed=55
while test -n "$deals" ; do
    echo "Trying seed = $seed"
    export SEED="$seed"
    (for d in $deals ; do echo "$d" ; done) | parallel --group -j4 bash ../scripts/seed-solve-one-board.bash | \
        tee >(grep -E ^Found | sort | tee -a seeds.log.txt)
    let seed++
    calc_deals
done
