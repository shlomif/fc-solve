DEBUG = 1
PROFILE = 2
WITH_TRACES = 0
FREECELL_ONLY = 1
DISABLE_PATSOLVE = 1
DISABLE_SIMPLE_SIMON := 0
OPT_FOR_SIZE = 0
OPT_AND_DEBUG = 0
NATIVE_ARCH = 1

# Can be any of clang, gcc, icc, lcc, pcc, tcc or tendra
COMPILER = gcc

ifeq ($(SRC_DIR),)
	SRC_DIR := .
endif

ifeq ($(FREECELL_ONLY),1)
	DISABLE_SIMPLE_SIMON := 1
endif

CFLAGS := -I. -I$(SRC_DIR)/libavl/ -I$(SRC_DIR) -I$(SRC_DIR)/patsolve-shlomif/patsolve
GCC_COMPAT :=
INIT_CFLAGS := -Wp,-MD,.deps/$(*F).pp

ARCH := $(shell uname -i)

ifeq ($(ARCH),i386)
	MACHINE_OPT := -O2
else
	MACHINE_OPT := -O3
endif

LTO_FLAGS := -flto -ffat-lto-objects
AR := ar
RANLIB := ranlib
END_LFLAGS := -lm -Wl,-rpath,::::::::::::::::::::::::::

STD_FLAG := -std=gnu11

ifeq ($(COMPILER),gcc)
	CC = gcc
	GCC_COMPAT := 1
	CFLAGS += $(STD_FLAG)
else ifeq ($(COMPILER),clang)
	CC = clang
	GCC_COMPAT := 1
	GOLD = -fuse-ld=gold
	END_LFLAGS += $(GOLD)
	# CFLAGS += -Werror=implicit-function-declaration $(STD_FLAG) -fuse-ld=gold
	CFLAGS += -Werror=implicit-function-declaration $(STD_FLAG) -fPIC $(GOLD)
	# LTO_FLAGS := -flto
	# LTO_FLAGS :=
	AR := llvm-ar
	RANLIB := llvm-ranlib
else ifeq ($(COMPILER),icc)
	CC = icc
	GCC_COMPAT := 1
else ifeq ($(COMPILER),sun)
	CFLAGS :=
	CC = cc
	GCC_COMPAT := 0
	INIT_CFLAGS :=
	CREATE_SHARED = $(CC) $(CFLAGS) -shared
	ifeq ($(OPT_FOR_SIZE),1)
		CFLAGS +=
	else
		CFLAGS += -fast
	endif
else ifeq ($(COMPILER),lcc)
	CC = lcc
	GCC_COMPAT := 1
else ifeq ($(COMPILER),pcc)
	CC = pcc
	GCC_COMPAT := 1
	CFLAGS += -I /usr/include/linux -I /usr/lib/gcc/i586-manbo-linux-gnu/4.3.2/include/
else ifeq ($(COMPILER),tcc)
	CC = tcc
	GCC_COMPAT := 1
else ifeq ($(COMPILER),tendra)
	CC = tcc
	CFLAGS := -Ysystem
	ifeq ($(DEBUG),1)
		CFLAGS += -g
	endif
	CREATE_SHARED = ld -shared
	END_SHARED :=
else
	CC = error
endif

ifeq ($(GCC_COMPAT),1)
	CREATE_SHARED = $(CC) -fPIC $(CFLAGS) -shared -fwhole-program
	END_SHARED = $(END_LFLAGS)
	ifeq ($(NATIVE_ARCH),1)
		MARCH_FLAG := -march=native
	else
		MARCH_FLAG := -mtune=generic
	endif
	ifeq ($(DEBUG),1)
		CFLAGS += -g
	else ifeq ($(OPT_FOR_SIZE),1)
		CFLAGS += -Os -fvisibility=hidden
	else ifeq ($(OPT_AND_DEBUG),1)
		CFLAGS += -g -O2 $(MARCH_FLAG) $(LTO_FLAGS)
	else
		CFLAGS += $(MACHINE_OPT) $(MARCH_FLAG) -DNDEBUG -fomit-frame-pointer $(LTO_FLAGS) -fvisibility=hidden
	endif
	CFLAGS += -fPIC
endif

# The malloc library should appear as early as possible in the link stage
# per the instructions in the E-mail from Hoard malloc's Emery Berger.
TCMALLOC_LINK = -ltcmalloc
# TCMALLOC_LINK = -lzmalloc
# TCMALLOC_LINK = -lhoard

# END_SHARED += $(TCMALLOC_LINK)

ifneq ($(WITH_TRACES),0)
	CFLAGS += -DDEBUG
endif

ifneq ($(FREECELL_ONLY),0)
	CFLAGS += -DFCS_FREECELL_ONLY=1
endif

ifneq ($(DISABLE_SIMPLE_SIMON),0)
	# CFLAGS += -DFCS_DISABLE_SIMPLE_SIMON=1
endif

ifneq ($(DISABLE_PATSOLVE),0)
	CFLAGS += -DFCS_DISABLE_PATSOLVE=1
endif

EXTRA_CFLAGS =
CFLAGS += $(EXTRA_CFLAGS)

LFLAGS := -O3 -DNDEBUG -fvisibility=hidden -march=native -fomit-frame-pointer -flto -ffat-lto-objects -fwhole-program $(EXTRA_CFLAGS)

# Toggle for profiling information.
ifneq ($(PROFILE),0)
	ifeq ($(PROFILE),2)
		END_LFLAGS := -lm -lprofiler
	else
		END_OFLAGS := -pg
		# END_LFLAGS := -pg -lc_p -lm_p -static-libgcc
		END_LFLAGS := -pg -lm
	endif
endif

FCS_SHARED_LIB = libfreecell-solver.so.0
TARGETS = fc-solve $(FCS_SHARED_LIB) \
          freecell-solver-range-parallel-solve \
          freecell-solver-multi-thread-solve \
          freecell-solver-fork-solve \
          freecell-solver-fc-pro-range-solve

all: $(TARGETS)

board_gen: dummy
	make -C board_gen/

dummy:



#<<<OBJECTS.START
GEN_C_OBJECTS := \
	move_funcs_maps.o   \
	rate_state.o        \

SOURCE_OBJECTS :=             \
          card.o              \
          check_and_add_state.o \
          cmd_line.o          \
          freecell.o          \
          instance.o          \
          lib.o               \
          meta_alloc.o        \
          move.o              \
          move_funcs_order.o  \
          scans.o             \
          split_cmd_line.o    \
          state.o             \

MAIN_OBJECT := main.o
T_MAIN_OBJECT := serial_range_solver.o
THR_MAIN_OBJECT := threaded_range_solver.o
FORK_MAIN_OBJECT := forking_range_solver.o
#>>>OBJECTS.END

OBJECTS := $(GEN_C_OBJECTS) $(SOURCE_OBJECTS)

PAT_OBJECTS = \
		  param.o \
		  patsolve.o \
		  pat.o \
		  tree.o \


ifeq ($(DISABLE_SIMPLE_SIMON),0)
	OBJECTS += simpsim.o
endif

ifeq ($(FREECELL_ONLY),0)
	OBJECTS += preset.o
endif

# MYOBJ.o ==> .deps/MYOBJ.P
DEP_FILES = $(addprefix .deps/,$(addsuffix .pp,$(basename $(OBJECTS))))

-include $(DEP_FILES)

FC_PRO_OBJS = fc_pro_range_solver.o fc_pro_iface.o fc_pro_iface_aux.o

$(SOURCE_OBJECTS) $(MAIN_OBJECT) $(T_MAIN_OBJECT) $(THR_MAIN_OBJECT) $(FORK_MAIN_OBJECT) $(FC_PRO_OBJS): %.o: $(SRC_DIR)/%.c
	$(CC) $(INIT_CFLAGS) $(CFLAGS) -o $@ -c $< $(END_OFLAGS)

$(GEN_C_OBJECTS): %.o: %.c
	$(CC) $(INIT_CFLAGS) $(CFLAGS) -o $@ -c $< $(END_OFLAGS)

$(PAT_OBJECTS): %.o: $(SRC_DIR)/patsolve-shlomif/patsolve/%.c
	$(CC) $(INIT_CFLAGS) -c $(CFLAGS) -o $@ $< $(END_OFLAGS)

STATIC_LIB_BASE = fcs
STATIC_LIB = lib$(STATIC_LIB_BASE).a

FCS_OBJECTS = $(OBJECTS)

ifeq ($(DISABLE_PATSOLVE),0)
	FCS_OBJECTS += $(PAT_OBJECTS)
endif

$(STATIC_LIB): $(FCS_OBJECTS)
	$(AR) qc $@ $^
	$(RANLIB) $@

$(FCS_SHARED_LIB): $(FCS_OBJECTS)
	$(CREATE_SHARED) $(LIB_LINK_PRE) $(TCMALLOC_LINK) -o $@ $^ $(END_SHARED)
	if ! test -e libfreecell-solver.so ; then \
		ln -s $@ libfreecell-solver.so ; \
	fi

ifeq ($(COMPILER),tcc)
    LIB_LINK_PRE :=
else
    # LIB_LINK_PRE := -Wl,-rpath,.
endif

# LIB_LINK_PRE += -L.
# LIB_LINK_POST := -lfreecell-solver
LIB_LINK_POST := -rdynamic libfcs.a
# LIB_LINK_POST := -l$(STATIC_LIB_BASE)

fc-solve: $(MAIN_OBJECT) fc_pro_iface.o $(STATIC_LIB)
	$(CC) $(LFLAGS) -o $@ $(LIB_LINK_PRE) $< fc_pro_iface.o $(LIB_LINK_POST) $(END_LFLAGS)

freecell-solver-range-parallel-solve: $(T_MAIN_OBJECT) $(STATIC_LIB)
	$(CC) $(LFLAGS) -o $@ $(LIB_LINK_PRE) $< $(LIB_LINK_POST) $(END_LFLAGS)

freecell-solver-multi-thread-solve: $(THR_MAIN_OBJECT) $(STATIC_LIB)
	$(CC) $(LFLAGS) -o $@ $(LIB_LINK_PRE) $< $(LIB_LINK_POST) -lpthread $(TCMALLOC_LINK) $(END_LFLAGS)

freecell-solver-fork-solve: $(FORK_MAIN_OBJECT) $(STATIC_LIB)
	$(CC) $(TCMALLOC_LINK) $(LFLAGS) -o $@ $(LIB_LINK_PRE) $< $(LIB_LINK_POST) -lpthread $(END_LFLAGS)


freecell-solver-fc-pro-range-solve: $(FC_PRO_OBJS) $(STATIC_LIB)
	$(CC) $(LFLAGS) -o $@ $(LIB_LINK_PRE) $(FC_PRO_OBJS) $(LIB_LINK_POST) $(END_LFLAGS)

clean:
	rm -f *.o libavl/*.o $(TARGETS) libfcs.a test-lib mtest libfreecell-solver.so*

%.show:
	@echo "$* = $($*)"
