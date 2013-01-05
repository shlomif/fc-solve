TEST_HTML = my-modified-fc-solve-test.html
RESULT_HTML = fc-solve-test.html
PROCESS_PL = scripts/process-js-html.pl

C_FILES = app_str.c scans.c main.c lib.c preset.c instance.c move_funcs_order.c  move_funcs_maps.c meta_alloc.c cmd_line.c card.c state.c check_and_add_state.c fcs_hash.c split_cmd_line.c simpsim.c freecell.c move.c fc_pro_iface.c rate_state.c

LLVM_BITCODE_FILES = $(patsubst %.c,build/%.bc,$(C_FILES))

all: $(TEST_HTML)

CFLAGS = -g -I ./build

$(LLVM_BITCODE_FILES): build/%.bc: %.c
	clang $(CFLAGS) -emit-llvm $< -c -o $@

$(RESULT_HTML): $(LLVM_BITCODE_FILES)
	emcc --jcache -s TOTAL_MEMORY="$$((128 * 1024 * 1024))" -m32 -std=gnu99 $(CFLAGS) \
		-o $@ -I $(LLVM_BITCODE_FILES) --embed-file build/24.board

$(TEST_HTML): $(RESULT_HTML) $(PROCESS_PL)
	perl $(PROCESS_PL) < $< > $@
