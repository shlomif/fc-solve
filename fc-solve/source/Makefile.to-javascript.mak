ifeq ($(SRC_DIR),)
	SRC_DIR := .
endif

ifeq ($(CMAKE_DIR),)
	CMAKE_DIR := .
endif

DATA_DESTDIR = __DESTDIR
RESULT_NODE_JS_EXE = fc-solve.js
RESULT_JS_LIB = libfreecell-solver.js
RESULT_HTML = fc-solve-test.html
PROCESS_PL = $(SRC_DIR)/scripts/process-js-html.pl
EMBED_FILE_MUNGE_PL = $(SRC_DIR)/scripts/emscripten-embed-munge.pl

PATS_C_FILES = $(patsubst %,patsolve-shlomif/patsolve/%,param.c pat.c patsolve.c tree.c)

LIB_C_FILES = scans.c lib.c preset.c instance.c move_funcs_order.c   meta_alloc.c cmd_line.c card.c state.c check_and_add_state.c split_cmd_line.c simpsim.c freecell.c move.c fc_pro_iface.c gen_ms_boards__hll_iface.c hacks_for_hlls.c $(PATS_C_FILES)

CMAKE_C_FILES = move_funcs_maps.c rate_state.c
C_FILES = main.c $(LIB_C_FILES)


SRC_C_FILES = $(patsubst %.c,$(SRC_DIR)/%.c,$(C_FILES))
SRC_CMAKE_C_FILES = $(patsubst %.c,$(CMAKE_DIR)/%.c,$(CMAKE_C_FILES))
LLVM_BITCODE_FILES = $(patsubst %.c,%.bc,$(C_FILES))
LLVM_BITCODE_LIB_FILES = $(patsubst %.c,%.bc,$(LIB_C_FILES))
LLVM_BITCODE_CMAKE_FILES = $(patsubst %.c,%.bc,$(CMAKE_C_FILES))

all: $(RESULT_NODE_JS_EXE) $(RESULT_JS_LIB)

NEEDED_FUNCTIONS = \
    fc_solve__hll_ms_rand__get_singleton \
	fc_solve__hll_ms_rand__init \
	fc_solve__hll_ms_rand__mod_rand \
	free \
	freecell_solver_user_alloc \
	freecell_solver_user_cmd_line_read_cmd_line_preset \
	freecell_solver_user_cmd_line_parse_args_with_file_nesting_count \
	freecell_solver_user_current_state_stringify \
	freecell_solver_user_free \
	freecell_solver_user_get_next_move \
	freecell_solver_user_get_num_freecells \
	freecell_solver_user_get_num_stacks \
	freecell_solver_user_get_invalid_state_error_into_string \
	freecell_solver_user_limit_iterations_long \
	freecell_solver_user_stringify_move_ptr \
	freecell_solver_user_resume_solution \
	freecell_solver_user_solve_board \
	malloc \

NEEDED_FUNCTIONS_STR := $(shell perl -e 'print join(", ", map { chr(0x27) . "_" . $$_ . chr(0x27) } @ARGV)' $(NEEDED_FUNCTIONS))

# OPT_FLAGS = -g
OPT_FLAGS = -O2
# OPT_FLAGS = -O3
# OPT_FLAGS = -O1
# OPT_FLAGS =

CFLAGS = $(OPT_FLAGS) -I . -I $(SRC_DIR) -I $(SRC_DIR)/asprintf-1.0 -I $(SRC_DIR)/patsolve-shlomif/patsolve/ -I $(CMAKE_DIR) -m32 -std=gnu99 -DFC_SOLVE_JAVASCRIPT_QUERYING=1

# ASSERT_FLAGS = -s ASSERTIONS=1
ASSERT_FLAGS =

EMCC_CFLAGS = -s TOTAL_MEMORY="$$((128 * 1024 * 1024))" -s EXPORTED_FUNCTIONS="[$(NEEDED_FUNCTIONS_STR)]" -s MODULARIZE=1 $(CFLAGS) $(ASSERT_FLAGS)

PRESET_DIR = /fc-solve/share/freecell-solver/
PRESET_FILES_TO_EMBED := $(shell find $(DATA_DESTDIR)$(PRESET_DIR) -type f | (LC_ALL=C sort))

PRESET_FILES_LOCAL := $(shell perl $(EMBED_FILE_MUNGE_PL) $(DATA_DESTDIR) $(PRESET_FILES_TO_EMBED) )

EMCC_POST_FLAGS :=  $(patsubst %,--embed-file %,$(PRESET_FILES_LOCAL))

$(LLVM_BITCODE_CMAKE_FILES): %.bc: $(CMAKE_DIR)/%.c
	emcc $(EMCC_CFLAGS) $< -c -o $@

$(LLVM_BITCODE_FILES): %.bc: $(SRC_DIR)/%.c
	mkdir -p "$$(dirname "$@")"
	emcc $(EMCC_CFLAGS) $< -c -o $@

.PHONY: llvm_and_files

llvm_and_files: $(LLVM_BITCODE_FILES) $(LLVM_BITCODE_CMAKE_FILES)

$(RESULT_HTML): llvm_and_files
	emcc $(EMCC_CFLAGS) -o $@  $(LLVM_BITCODE_FILES) $(LLVM_BITCODE_CMAKE_FILES) $(EMCC_POST_FLAGS)

$(RESULT_NODE_JS_EXE): llvm_and_files
	emcc $(EMCC_CFLAGS) -o $@  $(LLVM_BITCODE_FILES) $(LLVM_BITCODE_CMAKE_FILES) $(EMCC_POST_FLAGS)

PRE_JS = $(SRC_DIR)/scripts/pre.js
POST_JS = $(SRC_DIR)/scripts/post.js
$(RESULT_JS_LIB): llvm_and_files
	# emcc $(EMCC_CFLAGS) --pre-js $(PRE_JS) --post-js $(POST_JS) -o $@  $(LLVM_BITCODE_LIB_FILES) $(LLVM_BITCODE_CMAKE_FILES) $(EMCC_POST_FLAGS)
	emcc $(EMCC_CFLAGS) -o $@  $(LLVM_BITCODE_LIB_FILES) $(LLVM_BITCODE_CMAKE_FILES) $(EMCC_POST_FLAGS)
	cat $(SRC_DIR)/scripts/pre.js $@ $(SRC_DIR)/scripts/post.js > temp.$@
	mv -f temp.$@ $@

clean:
	rm -f $(LLVM_BITCODE_FILES) $(RESULT_HTML) $(RESULT_NODE_JS_EXE)

%.show:
	@echo "$* = $($*)"
