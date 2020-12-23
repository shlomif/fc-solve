set -x
deal="$(( 4 * 10 ** 9 ))"
deal="$(( ( 1 << 32 ) - 1 ))"
deal="$(( ( 1 << 33 ) - 1 ))"
deal="$(( 6 * 10 ** 9 ))"
run_deal="$(( 20004000 ))"
run_deal="$deal"
(
    set -e
    board_gen_dir="../../source/board_gen"
    test -d "$board_gen_dir"
    gen_ocl_py_prog="$board_gen_dir"/find-freecell-deal-index-julia-opencl.py
    test -f "$gen_ocl_py_prog"
    python3 "$gen_ocl_py_prog" --ms <(pi-make-microsoft-freecell-board -t "$deal")
    ${CC:-clang} -shared -fPIC -O3 -march=native -flto -o lib"opencl_find_deal_idx.so" -I ~/Download/unpack/to-del/www.dmi.unict.it/bilotta/gpgpu/svolti/aa201920/opencl/ -I "$board_gen_dir" -Wall -Wextra "opencl_find_deal_idx.c" -lOpenCL
    time python3 "$board_gen_dir"/find-freecell-deal-index-using-opencl.py --ms <(pi-make-microsoft-freecell-board -t "$run_deal")
)
