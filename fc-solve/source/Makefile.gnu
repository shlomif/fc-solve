CC = gcc

DEBUG = 1
PROFILE = 0
WITH_TRACES = 1
FREECELL_ONLY = 0

ifneq ($(DEBUG),0)
	CFLAGS := -Wall -g
else
	CFLAGS := -Wall -O3
endif

ifneq ($(WITH_TRACES),0)
	CFLAGS += -DDEBUG
endif

ifneq ($(FREECELL_ONLY),0)
	CFLAGS += -DFCS_FREECELL_ONLY=1
endif

LFLAGS := -Wall
END_LFLAGS := -lm

# Toggle for profiling information.
ifneq ($(PROFILE),0)
	END_OFLAGS := -pg
	END_LFLAGS := -pg -lc_p -lm_p -static-libgcc
endif

DFLAGS = $(CFLAGS) -DDEBUG
END_DLFLAGS = $(END_LFLAGS)

DLFLAGS = $(LFLAGS)

TARGETS = fc-solve mptest

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
OBJECTS =                     \
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
          lookup2.o           \
          move.o              \
          pqueue.o            \
          preset.o            \
          rand.o              \
          scans.o             \
          simpsim.o           \
          state.o             \

#>>>OBJECTS.END

# MYOBJ.o ==> .deps/MYOBJ.P
DEP_FILES = $(addprefix .deps/,$(addsuffix .pp,$(basename $(OBJECTS))))

-include $(DEP_FILES)

%.o: %.c
	$(CC) -Wp,-MD,.deps/$(*F).pp -c $(CFLAGS) -o $@ $< $(END_OFLAGS)

libfcs.a: $(OBJECTS)
	ar r $@ $(OBJECTS)
	ranlib $@

fc-solve: main.o libfcs.a
	$(CC) $(LFLAGS) -o $@ -L. $< -lfcs $(END_LFLAGS)

mptest: test_multi_parallel.o libfcs.a
	gcc -Wall -o $@ -L. $< -lfcs $(END_LFLAGS)

clean:
	rm -f *.o $(TARGETS) libfcs.a test-lib mtest mptest

endif

