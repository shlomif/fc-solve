CC = gcc


DEBUG = 1

ifneq ($(DEBUG),0)
	OFLAGS := -Wall -g
else
	OFLAGS := -Wall -O3
endif

OLFLAGS := -Wall
END_OLFLAGS := -lm


# OFLAGS += -DCARD_DEBUG_PRES -DFCS_WITH_MHASH -I /usr/local/include

# END_OLFLAGS += -lmhash
# END_OLFLAGS += -lefence

# OFLAGS += -DDMALLOC
# END_OLFLAGS += -ldmalloc

# END_OFLAGS := -pg

# END_OLFLAGS += -pg -lc_p

DFLAGS = $(OFLAGS) -DDEBUG
END_DLFLAGS = $(END_OLFLAGS)

DLFLAGS = $(OLFLAGS)

TARGETS = fc-solve mptest

ifeq ($(EXIT),1)

error:
	@echo "Error! WHICH_STATE_ALLOCATION must be defined to a legal value"
	@echo $(OLFLAGS)

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
          tests.o             \

#>>>OBJECTS.END

# MYOBJ.o ==> .deps/MYOBJ.P
DEP_FILES = $(addprefix .deps/,$(addsuffix .pp,$(basename $(OBJECTS))))

-include $(DEP_FILES)

test_num.h: gen_tests_lists.pl
	perl $<

%.o: %.c
	$(CC) -Wp,-MD,.deps/$(*F).pp -c $(OFLAGS) -o $@ $< $(END_OFLAGS)

libfcs.a: $(OBJECTS)
	ar r $@ $(OBJECTS)
	ranlib $@

fc-solve: main.o libfcs.a
	$(CC) $(OLFLAGS) -o $@ -L. $< -lfcs $(END_OLFLAGS)

mptest: test_multi_parallel.o libfcs.a
	gcc -Wall -o $@ -L. $< -lfcs $(END_OLFLAGS)

clean:
	rm -f *.o $(TARGETS) libfcs.a test-lib mtest mptest

endif

