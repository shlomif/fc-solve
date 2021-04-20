set -x
deal="$(( 84 * 10 ** 8 ))"
if false
then
    deal="$(( ( 1 << 32 ) - 1 ))"
    deal="$(( ( 1 << 33 ) - 1 ))"
    deal="$(( 6 * 10 ** 9 ))"
fi
deal="$(( ( 1 << 33 ) - 1 ))"
# deal="200"
run_deal="$(( 20004000 ))"
run_deal="$deal"
(
    set -e
    board_gen_dir="../../source/board_gen"
    test -d "$board_gen_dir"
    gen_ocl_py_prog="$board_gen_dir"/find-freecell-deal-index-julia-opencl.py
    test -f "$gen_ocl_py_prog"
    lib=libopencl_find_deal_idx.so
    if ( ! test -e "$lib" ) || ( test "$REBUILD" == "1" )
    then
        python3 "$gen_ocl_py_prog" --ms <(pi-make-microsoft-freecell-board -t "$deal")
        ${CC:-clang} -shared -fPIC -O3 -march=native -flto -o "$lib" -I ~/Download/unpack/to-del/www.dmi.unict.it/bilotta/gpgpu/svolti/aa201920/opencl/ -I "$board_gen_dir" ${WCFLAGS:--Weverything} "opencl_find_deal_idx.c" -lOpenCL
    fi
    time python3 "$board_gen_dir"/find-freecell-deal-index-using-opencl.py --ms <(pi-make-microsoft-freecell-board -t "$run_deal")
)
