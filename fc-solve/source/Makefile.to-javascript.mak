ifeq ($(SRC_DIR),)
	SRC_DIR := .
endif

TEST_HTML = my-modified-fc-solve-test.html
RESULT_NODE_JS_EXE = fc-solve.js
RESULT_HTML = fc-solve-test.html
PROCESS_PL = $(SRC_DIR)/scripts/process-js-html.pl

C_FILES = app_str.c scans.c main.c lib.c preset.c instance.c move_funcs_order.c  move_funcs_maps.c meta_alloc.c cmd_line.c card.c state.c check_and_add_state.c fcs_hash.c split_cmd_line.c simpsim.c freecell.c move.c fc_pro_iface.c rate_state.c

SRC_C_FILES = $(patsubst %.c,$(SRC_DIR)/%.c,$(C_FILES))
LLVM_BITCODE_FILES = $(patsubst %.c,%.bc,$(C_FILES))

all: $(TEST_HTML) $(RESULT_NODE_JS_EXE)

CFLAGS = -g -I ./build -I . -m32 -std=gnu99

EMCC_POST_FLAGS = --embed-file 24.board

$(LLVM_BITCODE_FILES): %.bc: $(SRC_DIR)/%.c
	clang $(CFLAGS) -emit-llvm $< -c -o $@

$(RESULT_HTML): $(LLVM_BITCODE_FILES)
	emcc --jcache -s TOTAL_MEMORY="$$((128 * 1024 * 1024))" $(CFLAGS) \
		-o $@  $(LLVM_BITCODE_FILES) $(EMCC_POST_FLAGS)

$(RESULT_NODE_JS_EXE): $(LLVM_BITCODE_FILES)
	emcc --jcache -s TOTAL_MEMORY="$$((128 * 1024 * 1024))" $(CFLAGS) \
		-o $@  $(LLVM_BITCODE_FILES) $(EMCC_POST_FLAGS)

$(TEST_HTML): $(RESULT_HTML) $(PROCESS_PL)
	perl $(PROCESS_PL) < $< > $@

clean:
	rm -f $(LLVM_BITCODE_FILES) $(RESULT_HTML) $(RESULT_NODE_JS_EXE)
