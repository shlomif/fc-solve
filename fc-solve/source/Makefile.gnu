CC = gcc


DEBUG = 0

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




OBJECTS = alloc.o           \
          app_str.o         \
          caas.o            \
          card.o            \
          cmd_line.o        \
          fcs_dm.o          \
          fcs_hash.o        \
          fcs_isa.o         \
          freecell.o        \
          intrface.o        \
          lib.o             \
          lookup2.o         \
          move.o            \
          pqueue.o          \
          preset.o          \
          rand.o            \
          scans.o           \
          simpsim.o         \
          state.o


# MYOBJ.o ==> .deps/MYOBJ.P
DEP_FILES = $(addprefix .deps/,$(addsuffix .pp,$(basename $(OBJECTS))))

-include $(DEP_FILES)

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

