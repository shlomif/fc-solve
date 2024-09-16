set -x
deal="$(( 84 * 10 ** 8 ))"
if false
then
    deal="$(( ( 1 << 32 ) - 1 ))"
    deal="$(( ( 1 << 33 ) - 1 ))"
    deal="$(( 6 * 10 ** 9 ))"
fi
# deal="$(( ( 1 << 33 ) - 1 ))"
# deal="200"
# run_deal="$(( 20004000 ))"
run_deal="$deal"
(
    set -e

    should_rebuild()
    {
        test \( \! -e "$lib" \) -o \( "$REBUILD" = "1" \)
    }
    _compile_c()
    {
        ${CC:-clang} -shared -fPIC -O3 -march=native -flto -o "$lib" ${include_dir_flags[*]} ${WCFLAGS:--Weverything -Wno-unsafe-buffer-usage -Wno-declaration-after-statement} "$@"
    }

    board_gen_dir="../../source/board_gen"
    test -d "$board_gen_dir"
    gen_ocl_py_prog="$board_gen_dir"/find-freecell-deal-index-generate-opencl-code.py
    lib=libfcs_find_deal.so
    include_dir_flags=(-I ~/Download/unpack/to-del/www.dmi.unict.it/bilotta/gpgpu/svolti/aa201920/opencl/ -I "$board_gen_dir" -I "$board_gen_dir/..")
    if false
    then
        # Headers should be installed by using
        # "dnf5 install freecell-solver-devel"
        # or similar.
        include_dir_flags+=(-I "$board_gen_dir/../include")
    fi
    if should_rebuild
    then
        _compile_c "$board_gen_dir/find_deal.c"
    fi
    test -f "$gen_ocl_py_prog"
    lib=libopencl_find_deal_idx.so
    if should_rebuild
    then
        python3 "$gen_ocl_py_prog" --ms <(pi-make-microsoft-freecell-board -t "$deal")
        _compile_c "opencl_find_deal_idx.c" -lOpenCL
    fi
    if ! test "$SKIP_CISH" = 1
    then
        date "+start=%s.%N"
        time python3 "$board_gen_dir"/find-freecell-deal-index-cish.py --ms <(pi-make-microsoft-freecell-board -t "$run_deal")
        date "+finish=%s.%N"
    fi
    date "+start=%s.%N"
    time python3 "$board_gen_dir"/find-freecell-deal-index-using-opencl.py --ms <(pi-make-microsoft-freecell-board -t "$run_deal")
    date "+finish=%s.%N"
)
