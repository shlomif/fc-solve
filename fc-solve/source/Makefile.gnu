CC = gcc

DEBUG = 1

ifneq ($(DEBUG),0)
	OFLAGS := -Wall -g
else
	OFLAGS := -Wall -O3
endif

OLFLAGS := -Wall
END_OLFLAGS := -lm

USE_INDIRECT_STATE_ALLOCATION := 1
USE_AVL_AVL_STATE_ALLOCATION := 2
USE_AVL_REDBLACK_STATE_ALLOCATION := 3
USE_LIBREDBLACK_STATE_ALLOCATION := 4
USE_GLIB_TREE_STATE_ALLOCATION := 5
USE_GLIB_HASH_STATE_ALLOCATION := 6
USE_DB_FILE_STATE_ALLOCATION := 7
USE_INTERNAL_HASH_STATE_ALLOCATION := 8

#
# Uncomment one of these lines
# You need to have libredblack or the avl library installed if you want
# to compile Freecell Solver with a balanced tree support.
#

#WHICH_STATE_ALLOCATION := $(USE_AVL_AVL_STATE_ALLOCATION)
#WHICH_STATE_ALLOCATION := $(USE_AVL_REDBLACK_STATE_ALLOCATION)
#WHICH_STATE_ALLOCATION := $(USE_LIBREDBLACK_STATE_ALLOCATION)
#WHICH_STATE_ALLOCATION := $(USE_GLIB_TREE_STATE_ALLOCATION)
#WHICH_STATE_ALLOCATION := $(USE_GLIB_HASH_STATE_ALLOCATION)
#WHICH_STATE_ALLOCATION := $(USE_DB_FILE_STATE_ALLOCATION)
WHICH_STATE_ALLOCATION := $(USE_INTERNAL_HASH_STATE_ALLOCATION)

# I choose indirect state allocation by default because it is the fastest
# implementation that is not dependant on external libraries.
ifndef WHICH_STATE_ALLOCATION
WHICH_STATE_ALLOCATION := $(USE_INDIRECT_STATE_ALLOCATION)
endif


USE_NONE_STACK_ALLOC := 0
USE_INTERNAL_HASH_STACK_ALLOC := 1
USE_LIBAVL_AVL_STACK_ALLOC := 2
USE_LIBAVL_REDBLACK_STACK_ALLOC := 3
USE_LIBREDBLACK_STACK_ALLOC := 4
USE_GLIB_TREE_STACK_ALLOC := 5
USE_GLIB_HASH_STACK_ALLOC := 6


#
# If you are using Indirect Stack States, you have to uncomment one of 
# lines for the following implementations.
#
# USE_INTERNAL_HASH_STACK_ALLOC will always work no matter what you have 
# installed.
#



#WHICH_STACK_ALLOCATION := $(USE_NONE_STACK_ALLOC)
#WHICH_STACK_ALLOCATION := $(USE_GLIB_TREE_STACK_ALLOC)
#WHICH_STACK_ALLOCATION := $(USE_LIBAVL_AVL_STACK_ALLOC)
#WHICH_STACK_ALLOCATION := $(USE_LIBAVL_REDBLACK_STACK_ALLOC)
#WHICH_STACK_ALLOCATION := $(USE_GLIB_HASH_STACK_ALLOC)
#WHICH_STACK_ALLOCATION := $(USE_LIBREDBLACK_STACK_ALLOC)
WHICH_STACK_ALLOCATION := $(USE_INTERNAL_HASH_STACK_ALLOC)


LINK_WITH_GLIB := 0
LINK_WITH_LIBAVL := 0
LINK_WITH_LIBREDBLACK := 0

ifeq ($(WHICH_STATE_ALLOCATION),$(USE_INDIRECT_STATE_ALLOCATION))
	OFLAGS := $(OFLAGS) -DFCS_STATE_STORAGE=FCS_STATE_STORAGE_INDIRECT
else
ifeq ($(WHICH_STATE_ALLOCATION),$(USE_AVL_AVL_STATE_ALLOCATION))
	OFLAGS := $(OFLAGS) -DFCS_STATE_STORAGE=FCS_STATE_STORAGE_LIBAVL_AVL_TREE
	END_OLFLAGS := $(END_OLFLAGS) -lavl
	LINK_WITH_LIBAVL := 1
else
ifeq ($(WHICH_STATE_ALLOCATION),$(USE_AVL_REDBLACK_STATE_ALLOCATION))
	OFLAGS := $(OFLAGS) -DFCS_STATE_STORAGE=FCS_STATE_STORAGE_LIBAVL_REDBLACK_TREE
	END_OLFLAGS := $(END_OLFLAGS) -lavl
	LINK_WITH_LIBAVL := 1
else
ifeq ($(WHICH_STATE_ALLOCATION),$(USE_LIBREDBLACK_STATE_ALLOCATION))
	OFLAGS := $(OFLAGS) -DFCS_STATE_STORAGE=FCS_STATE_STORAGE_LIBREDBLACK_TREE
	END_OLFLAGS := $(END_OLFLAGS) -lredblack
	OLFLAGS := $(OLFLAGS) -L/home/users/shlomif/libredblack/lib
	OFLAGS := $(OFLAGS) -I/home/users/shlomif/libredblack/include
	LINK_WITH_LIBREDBLACK := 1
else
ifeq ($(WHICH_STATE_ALLOCATION),$(USE_GLIB_TREE_STATE_ALLOCATION))
	OFLAGS := $(OFLAGS) -DFCS_STATE_STORAGE=FCS_STATE_STORAGE_GLIB_TREE `glib-config --cflags`
	LINK_WITH_GLIB := 1
	END_OLFLAGS := $(END_OLFLAGS) -lglib
else
ifeq ($(WHICH_STATE_ALLOCATION),$(USE_GLIB_HASH_STATE_ALLOCATION))
	OFLAGS := $(OFLAGS) -DFCS_STATE_STORAGE=FCS_STATE_STORAGE_GLIB_HASH `glib-config --cflags`
	LINK_WITH_GLIB := 1
	END_OLFLAGS := $(END_OLFLAGS) -lglib
else
ifeq ($(WHICH_STATE_ALLOCATION),$(USE_DB_FILE_STATE_ALLOCATION))
	OFLAGS := $(OFLAGS) -DFCS_STATE_STORAGE=FCS_STATE_STORAGE_DB_FILE
	END_OLFLAGS := $(END_OLFLAGS) -ldb
else
ifeq ($(WHICH_STATE_ALLOCATION),$(USE_INTERNAL_HASH_STATE_ALLOCATION))
	OFLAGS := $(OFLAGS) -DFCS_STATE_STORAGE=FCS_STATE_STORAGE_INTERNAL_HASH
	END_OLFLAGS := $(END_OLFLAGS)
else
EXIT = 1
endif
endif
endif
endif
endif
endif
endif
endif

ifeq ($(WHICH_STACK_ALLOCATION),$(USE_GLIB_TREE_STACK_ALLOC))
	OFLAGS += -DFCS_STACK_STORAGE=FCS_STACK_STORAGE_GLIB_TREE
	ifneq ($(LINK_WITH_GLIB),1)
		OFLAGS += `glib-config --cflags`
		END_OLFLAGS += `glib-config --libs`
	endif
else
ifeq ($(WHICH_STACK_ALLOCATION),$(USE_LIBAVL_AVL_STACK_ALLOC))
	OFLAGS += -DFCS_STACK_STORAGE=FCS_STACK_STORAGE_LIBAVL_AVL_TREE
	ifneq ($(LINK_WITH_LIBAVL),1)
		END_OLFLAGS += -lavl
	endif
else
ifeq ($(WHICH_STACK_ALLOCATION),$(USE_LIBAVL_REDBLACK_STACK_ALLOC))
	OFLAGS += -DFCS_STACK_STORAGE=FCS_STACK_STORAGE_LIBAVL_REDBLACK_TREE
	ifneq ($(LINK_WITH_LIBAVL),1)
		END_OLFLAGS += -lavl
	endif
else
ifeq ($(WHICH_STACK_ALLOCATION),$(USE_GLIB_HASH_STACK_ALLOC))
	OFLAGS += -DFCS_STACK_STORAGE=FCS_STACK_STORAGE_GLIB_HASH
	ifneq ($(LINK_WITH_GLIB),1)
		OFLAGS += `glib-config --cflags`
		END_OLFLAGS += `glib-config --libs`
	endif
else
ifeq ($(WHICH_STACK_ALLOCATION),$(USE_LIBREDBLACK_STACK_ALLOC))
	OFLAGS += -DFCS_STACK_STORAGE=FCS_STACK_STORAGE_LIBREDBLACK_TREE
	ifneq ($(LINK_WITH_LIBREDBLACK),1)
		END_OLFLAGS += -lredblack
	endif
else
ifeq ($(WHICH_STACK_ALLOCATION),$(USE_INTERNAL_HASH_STACK_ALLOC))
	OFLAGS += -DFCS_STACK_STORAGE=FCS_STACK_STORAGE_INTERNAL_HASH
else
endif
endif
endif
endif
endif
endif


# OFLAGS += -DCARD_DEBUG_PRES -DFCS_WITH_MHASH -I /usr/local/include

# END_OLFLAGS += -lmhash
# END_OLFLAGS += -lefence

OFLAGS += -DDMALLOC
END_OLFLAGS += -ldmalloc

# END_OFLAGS := -pg

# END_OLFLAGS += -pg -lc_p

DFLAGS = $(OFLAGS) -DDEBUG
END_DLFLAGS = $(END_OLFLAGS)

DLFLAGS = $(OLFLAGS)

INCLUDES = alloc.h app_str.h caas.h card.h config.h fcs.h fcs_dm.h fcs_enums.h fcs_hash.h fcs_isa.h fcs_move.h jhjtypes.h move.h pqueue.h preset.h rand.h state.h tests.h test_arr.h

TARGETS = fc-solve

ifeq ($(EXIT),1)

error:
	@echo "Error! WHICH_STATE_ALLOCATION must be defined to a legal value"
	@echo $(OLFLAGS)

else

all: $(TARGETS)

board_gen: dummy
	make -C board_gen/

dummy:




OBJECTS = alloc.o           \
          app_str.o         \
          caas.o            \
          card.o            \
          fcs_dm.o          \
          fcs_hash.o        \
          fcs_isa.o         \
          freecell.o        \
          intrface.o        \
          move.o            \
          pqueue.o          \
          preset.o          \
          rand.o            \
          scans.o           \
          simpsim.o         \
          state.o

$(OBJECTS) i_main.o:: %.o : %.c $(INCLUDES)
	$(CC) -c $(OFLAGS) -o $@ $< $(END_OFLAGS)

lib.o: lib.c $(INCLUDES) fcs_user.h
	$(CC) -c $(OFLAGS) -o $@ $< $(END_OFLAGS)

cmd_line.o: cmd_line.c fcs_user.h fcs_cl.h
	$(CC) -c $(OFLAGS) -o $@ $< $(END_OFLAGS)

main.o: main.c fcs_user.h fcs_cl.h
	$(CC) -c $(OFLAGS) -o $@ $< $(END_OFLAGS)

cl-fc-solve: $(OBJECTS) cl_main.o lib.o cmd_line.o
	$(CC) $(OLFLAGS) -o $@ $(OBJECTS) cl_main.o lib.o cmd_line.o $(END_OLFLAGS)

#fc-solve-debug: freecell-debug.o state-debug.o card-debug.o fcs_dm-debug.o fcs_isa-debug.o main-debug.o fcs_hash-debug.o md5c-debug.o move-debug.o caas-debug.o preset-debug.o pqueue-debug.o intrface-debug.o scans-debug.o
#	$(CC) $(DLFLAGS) -o $@ freecell-debug.o state-debug.o card-debug.o fcs_dm-debug.o fcs_isa-debug.o main-debug.o fcs_hash-debug.o md5c-debug.o move-debug.o caas-debug.o preset-debug.o pqueue-debug.o intrface-debug.o scans-debug.o $(END_DLFLAGS)

fc-solve: $(OBJECTS) main.o lib.o cmd_line.o
	$(CC) $(OLFLAGS) -o $@ $(OBJECTS) main.o lib.o cmd_line.o $(END_OLFLAGS)

libfcs.a: $(OBJECTS) lib.o cmd_line.o fcs_user.h fcs_cl.h
	ar r $@ $(OBJECTS) lib.o cmd_line.o
	ranlib $@

test-lib: test_lib.c libfcs.a fcs_user.h fcs_cl.h
	gcc -Wall -g -o $@ -L. test_lib.c -lfcs -lm $(END_OLFLAGS)

mtest: test_multi.o libfcs.a
	gcc -Wall -o $@ -L. test_multi.o -lfcs $(END_OLFLAGS)

test_multi.o: test_multi.c fcs_user.h fcs_cl.h
	$(CC) -c $(OFLAGS) -o $@ $< $(END_OFLAGS)

test_multi_parallel.o: test_multi_parallel.c fcs_user.h fcs_cl.h
	$(CC) -c $(OFLAGS) -o $@ $< $(END_OFLAGS)

mptest: test_multi_parallel.o libfcs.a fcs_user.h fcs_cl.h
	gcc -Wall -o $@ -L. $< -lfcs $(END_OLFLAGS)

i-fc-solve: i_main.o libfcs.a
	gcc -Wall -g -o $@ -L. i_main.o -lfcs $(END_OLFLAGS)

clean:
	rm -f *.o $(TARGETS) libfcs.a test-lib mtest mptest

endif

