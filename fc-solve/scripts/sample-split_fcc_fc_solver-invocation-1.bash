#!/bin/bash
export FCS_PATH=/home/shlomif/progs/freecell/git/fc-solve/fc-solve/source/B FCS_SRC_PATH=/home/shlomif/progs/freecell/git/fc-solve/fc-solve/source
offload_path="./split_fcc_offload_dir_path"
out_dir="./split_fcc_out_dir"
idx="384243"
board="${idx}.board"
if ! test -e "$offload_path" ; then
    mkdir "$offload_path"
fi
if ! test -e "$out_dir" ; then
    mkdir "$out_dir"
fi
if ! test -e "$board" ; then
    pi-make-microsoft-freecell-board -t "$idx" > "$board"
fi
init_board="${idx}.init.board"
#if ! test -e "$init_board" ; then
if true; then
    (
        echo 'Foundations: H-A C-0 D-0 S-0 '
        echo "Freecells:        "
        perl -plE 's/^/: /; s/ AH *\z//' < "$board"
    ) > "$init_board"
fi
input="${idx}.input.txtish"
# if ! test -e "$input" ; then
if true ; then
    (
        export I="${init_board}"
        perl -I ../t/t/lib ../../scripts/sample-split_fcc_fc_solver-invocation-1.helper.pl
    ) > "$input"
fi
./split_fcc_fc_solver \
    --offload-dir-path "$offload_path" \
    --output "$out_dir" \
    --board "$board" \
    --fingerprint "$(perl -I ../t/t/lib -E 'use strict; use warnings; use FC_Solve::Base64; print FC_Solve::Base64::base64_encode("\x{02}" . ("\x0" x 12))')" \
    --input "$input"
