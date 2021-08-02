ifeq ($(ASMJS),1)
	WASM := 0
	LIBSUF:=-asm
	WASM_FLAGS := -s LEGACY_VM_SUPPORT=1
else
	WASM := 1
	LIBSUF:=
	WASM_FLAGS :=
endif

ifeq ($(SRC_DIR),)
	SRC_DIR := .
endif

ifeq ($(CMAKE_DIR),)
	CMAKE_DIR := .
endif

DATA_DESTDIR ?= __DESTDIR
RESULT_NODE_JS_EXE = fc-solve.js
RESULT_JS_LIB = libfreecell-solver$(LIBSUF).js
PROCESS_PL = $(SRC_DIR)/scripts/process-js-html.pl
EMBED_FILE_MUNGE_PL = $(SRC_DIR)/../scripts/emscripten-embed-munge.pl

PATS_C_FILES = $(patsubst %,patsolve/patsolve/%,pat.c patsolve.c tree.c)

LIB_C_FILES = \
board_gen/find_deal.c \
card.c \
check_and_add_state.c \
cmd_line.c \
fc_pro_iface.c \
freecell.c \
gen_ms_boards__hll_iface.c \
hacks_for_hlls.c \
instance.c \
lib.c \
meta_alloc.c \
move.c \
move_funcs_order.c \
preset.c \
scans.c \
simpsim.c \
split_cmd_line.c \
state.c \
$(PATS_C_FILES)

CMAKE_C_FILES = \
fcs_is_ss_true_parent.c \
is_king.c \
is_parent.c \
move_funcs_maps.c \
param.c \
pos_by_rank__freecell.c \
rate_state.c \


C_FILES = $(LIB_C_FILES)


SRC_C_FILES = $(patsubst %.c,$(SRC_DIR)/%.c,$(C_FILES))
SRC_CMAKE_C_FILES = $(patsubst %.c,$(CMAKE_DIR)/%.c,$(CMAKE_C_FILES))
LLVM_BITCODE_FILES = $(patsubst %.c,%.bc,$(C_FILES))
LLVM_BITCODE_LIB_FILES = $(patsubst %.c,%.bc,$(LIB_C_FILES))
LLVM_BITCODE_CMAKE_FILES = $(patsubst %.c,%.bc,$(CMAKE_C_FILES))

all: $(RESULT_JS_LIB)

NEEDED_FUNCTIONS = \
	fc_solve__hll_ms_rand__get_singleton \
	fc_solve__hll_ms_rand__init \
	fc_solve__hll_ms_rand__mod_rand \
	fc_solve_user__find_deal__alloc \
	fc_solve_user__find_deal__fill \
	fc_solve_user__find_deal__free \
	fc_solve_user__find_deal__run \
	free \
	freecell_solver_user_alloc \
	freecell_solver_user_args_man_argc \
	freecell_solver_user_args_man_argv \
	freecell_solver_user_args_man_chop \
	freecell_solver_user_args_man_free \
	freecell_solver_user_cmd_line_parse_args_with_file_nesting_count \
	freecell_solver_user_cmd_line_read_cmd_line_preset \
	freecell_solver_user_current_state_stringify \
	freecell_solver_user_free \
	freecell_solver_user_get_invalid_state_error_into_string \
	freecell_solver_user_get_next_move \
	freecell_solver_user_get_num_freecells \
	freecell_solver_user_get_num_stacks \
	freecell_solver_user_get_unrecognized_cmd_line_flag \
	freecell_solver_user_get_unrecognized_cmd_line_flag_status \
	freecell_solver_user_limit_iterations_long \
	freecell_solver_user_resume_solution \
	freecell_solver_user_solve_board \
	freecell_solver_user_stringify_move_ptr \
	malloc \


NEEDED_FUNCTIONS_STR := $(shell perl -e 'print join(", ", map { chr(0x27) . "_" . $$_ . chr(0x27) } @ARGV)' $(NEEDED_FUNCTIONS))

# OPT_FLAGS = -g
# OPT_FLAGS = -O2
OPT_FLAGS = -O3
# OPT_FLAGS = -O1
# OPT_FLAGS =
# OPT_FLAGS = -g

RINUTILS_BASE_DIR ?= $(CMAKE_DIR)
RINUTILS_DIR ?= $(RINUTILS_BASE_DIR)/rinutils-include
RINUTILS_INCLUDE_DIR := $(RINUTILS_DIR)/include
RINUTILS_PIVOT = $(RINUTILS_INCLUDE_DIR)/rinutils/rinutils.h

CFLAGS = $(OPT_FLAGS) -I $(DATA_DESTDIR)/fc-solve/include -I ./include -I $(RINUTILS_INCLUDE_DIR) -I . -I $(SRC_DIR)/include -I $(SRC_DIR) -I $(SRC_DIR)/asprintf-1.0 -I $(SRC_DIR)/patsolve/patsolve/include -I $(SRC_DIR)/patsolve/patsolve/ -I $(SRC_DIR)/xxHash-wrapper -I $(SRC_DIR)/xxHash-wrapper/xxHash-0.7.2 -I $(CMAKE_DIR) -I $(CMAKE_DIR)/include -m32 -std=gnu18 -DFC_SOLVE_JAVASCRIPT_QUERYING=1

# ASSERT_FLAGS = -s ASSERTIONS=1
ASSERT_FLAGS =

EMCC_CFLAGS = -s WASM=$(WASM) -s TOTAL_MEMORY="$$((128 * 1024 * 1024))" -s EXPORTED_FUNCTIONS="[$(NEEDED_FUNCTIONS_STR)]" -s EXPORTED_RUNTIME_METHODS="['allocate', 'cwrap', 'getValue', 'intArrayFromString', 'setValue', 'ALLOC_STACK', 'FS', 'UTF8ToString']" $(WASM_FLAGS) -s MODULARIZE=1 $(CFLAGS) $(ASSERT_FLAGS)

PRESET_DIR = /fc-solve/share/freecell-solver/
PRESET_FILES_TO_EMBED := $(shell find $(DATA_DESTDIR)$(PRESET_DIR) -type f | (LC_ALL=C sort))

PRESET_FILES_LOCAL := $(shell perl $(EMBED_FILE_MUNGE_PL) $(DATA_DESTDIR) $(PRESET_FILES_TO_EMBED) )

EMCC_POST_FLAGS := $(patsubst %,--embed-file %,$(PRESET_FILES_LOCAL))

$(LLVM_BITCODE_CMAKE_FILES): %.bc: $(CMAKE_DIR)/%.c
	emcc $(EMCC_CFLAGS) $< -c -o $@

$(LLVM_BITCODE_FILES): %.bc: $(SRC_DIR)/%.c
	mkdir -p "$$(dirname "$@")"
	emcc $(EMCC_CFLAGS) $< -c -o $@

LLVM_AND_FILES_TARGETS = $(LLVM_BITCODE_FILES) $(LLVM_BITCODE_CMAKE_FILES)

$(LLVM_AND_FILES_TARGETS): $(RINUTILS_PIVOT)

$(RINUTILS_PIVOT):
	rin="$$(perl -MPath::Tiny -e 'print path(shift)->absolute' "$(RINUTILS_DIR)")"; unset CFLAGS ; (git clone https://github.com/shlomif/rinutils && cd rinutils && mkdir b && cd b && cmake -DWITH_TEST_SUITE=OFF -DCMAKE_INSTALL_PREFIX="$$rin" .. && make && make install && cd ../.. && rm -fr rinutils)

$(RESULT_NODE_JS_EXE): $(LLVM_AND_FILES_TARGETS)
	emcc $(EMCC_CFLAGS) -o $@ $(LLVM_BITCODE_FILES) $(LLVM_BITCODE_CMAKE_FILES) $(EMCC_POST_FLAGS)

$(RESULT_JS_LIB): $(LLVM_AND_FILES_TARGETS)
	emcc $(EMCC_CFLAGS) -o $@ $(LLVM_BITCODE_LIB_FILES) $(LLVM_BITCODE_CMAKE_FILES) $(EMCC_POST_FLAGS)

clean:
	rm -f $(LLVM_BITCODE_FILES) $(RESULT_NODE_JS_EXE)

%.show:
	@echo "$* = $($*)"
