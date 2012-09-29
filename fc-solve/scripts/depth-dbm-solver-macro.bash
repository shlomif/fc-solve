#!/bin/bash
myt()
{
    local deal b d
    deal="$(echo "$1" | perl -lpe 's/\A0*//')"
    shift
    b="$deal.board"
    if ! test -e "$b" ; then
        pi-make-microsoft-freecell-board -t "$deal" > "$b"
    fi

    d="$deal.depth_dbm.dump"
    if ! test -e "$d" ; then
        ./depth_dbm_fc_solver --num-threads 4 --offload-dir-path ~/tmp/depth-dbm/ "$deal".board | tee "$d"
    fi
}
