set -x
deal="$(( 2 * 10 ** 9 ))"
run_deal="$(( 20004000 ))"
run_deal="$deal"
(
    set -e
    python3 ../../source/board_gen/find-freecell-deal-index-julia-opencl.py --ms <(pi-make-microsoft-freecell-board -t "$deal")
    ${CC:-clang} -shared -fPIC -O3 -march=native -flto -o lib"opencl_find_deal_idx.so" -I ~/Download/unpack/to-del/www.dmi.unict.it/bilotta/gpgpu/svolti/aa201920/opencl/ -Wall -Wextra "../../source/board_gen/opencl_find_deal_idx.c" -lOpenCL
    time python3 ../../source/board_gen/find-freecell-deal-index-using-opencl.py --ms <(pi-make-microsoft-freecell-board -t "$run_deal")
)
