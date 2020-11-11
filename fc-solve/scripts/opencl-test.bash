deal="$(( 2 * 10 ** 7))"
(
    ( python3 ../../source/board_gen/find-freecell-deal-index-julia-opencl.py --ms <(pi-make-microsoft-freecell-board -t "$deal" ) && \
    gcc -shared -fPIC -o lib"opencl_find_deal_idx.so" -I ~/Download/unpack/to-del/www.dmi.unict.it/bilotta/gpgpu/svolti/aa201920/opencl/ -Wall -Wextra "opencl_find_deal_idx.c" -lOpenCL && \
(
    export LD_LIBRARY_PATH="$PWD" ; python3 ../../source/board_gen/find-freecell-deal-index-using-opencl.py --ms <(pi-make-microsoft-freecell-board -t "$deal" )
    )
)
)
