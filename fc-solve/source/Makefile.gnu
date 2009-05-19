COMPILER = gcc
# COMPILER = icc


DEBUG = 0
PROFILE = 0
WITH_TRACES = 0
FREECELL_ONLY = 1
WITH_LIBRB = 0

CFLAGS := -Wall
GCC_OR_ICC := 

ifeq ($(COMPILER),gcc)
	CC = gcc
	GCC_COMPAT := 1
else ifeq ($(COMPILER),icc)
	CC = icc
	GCC_COMPAT := 1
else ifdef undefined
	CC = error
endif

ifeq ($(GCC_COMPAT),1)
	CFLAGS += -g
else
	CFLAGS += -Os
endif

ifneq ($(WITH_TRACES),0)
	CFLAGS += -DDEBUG
endif

ifneq ($(FREECELL_ONLY),0)
	CFLAGS += -DFCS_FREECELL_ONLY=1
endif

LFLAGS := $(CFLAGS)
END_LFLAGS := -lm

# Toggle for profiling information.
ifneq ($(PROFILE),0)
	END_OFLAGS := -pg
	END_LFLAGS := -pg -lc_p -lm_p -static-libgcc
endif

ifneq ($(WITH_LIBRB),0)
	END_LFLAGS += -lredblack
endif

DFLAGS = $(CFLAGS) -DDEBUG
END_DLFLAGS = $(END_LFLAGS)

DLFLAGS = $(LFLAGS)

TARGETS = fc-solve libfreecell-solver.so \
		  freecell-solver-range-parallel-solve \
		  freecell-solver-multi-thread-solve

ifeq ($(EXIT),1)

error:
	@echo "Error! WHICH_STATE_ALLOCATION must be defined to a legal value"
	@echo $(LFLAGS)

else

all: $(TARGETS)

board_gen: dummy
	make -C board_gen/

dummy:



#<<<OBJECTS.START
OBJECTS :=                     \
          alloc.o             \
          app_str.o           \
          caas.o              \
          card.o              \
          cl_chop.o           \
          cmd_line.o          \
          fcs_dm.o            \
          fcs_hash.o          \
          fcs_isa.o           \
          freecell.o          \
          intrface.o          \
          lib.o               \
          move.o              \
          pqueue.o            \
          preset.o            \
          rand.o              \
          scans.o             \
          state.o             \

#>>>OBJECTS.END

ifeq ($(FREECELL_ONLY),0)
	OBJECTS += simpsim.o
endif

# MYOBJ.o ==> .deps/MYOBJ.P
DEP_FILES = $(addprefix .deps/,$(addsuffix .pp,$(basename $(OBJECTS))))

-include $(DEP_FILES)

%.o: %.c
	$(CC) -Wp,-MD,.deps/$(*F).pp -c $(CFLAGS) -o $@ $< $(END_OFLAGS)

libfcs.a: $(OBJECTS)
	ar r $@ $(OBJECTS)
	ranlib $@

libfreecell-solver.so: $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)

LIB_LINK_PRE := -Wl,-rpath,. -L.
LIB_LINK_POST := -lfreecell-solver

fc-solve: main.o libfcs.a
	$(CC) $(LFLAGS) -o $@ $(LIB_LINK_PRE) $< $(LIB_LINK_POST) $(END_LFLAGS)

freecell-solver-range-parallel-solve: test_multi_parallel.o libfreecell-solver.so
	gcc -Wall -o $@ $(LIB_LINK_PRE) $< $(LIB_LINK_POST) $(END_LFLAGS)

freecell-solver-multi-thread-solve: threaded_range_solver.o libfreecell-solver.so
	gcc -Wall -o $@ $(LIB_LINK_PRE) $< $(LIB_LINK_POST) -lpthread $(END_LFLAGS)

clean:
	rm -f *.o $(TARGETS) libfcs.a test-lib mtest libfreecell-solver.so*

endif

%.show:
	@echo "$* = $($*)"
