AUTOMAKE_OPTIONS = 1.4

SUBDIRS = board_gen

bin_PROGRAMS = fc-solve freecell-solver-range-parallel-solve
fc_solve_SOURCES = main.c
fc_solve_LDADD = libfreecell-solver.la
freecell_solver_range_parallel_solve_SOURCES = test_multi_parallel.c
freecell_solver_range_parallel_solve_LDADD = libfreecell-solver.la

bin_SCRIPTS=freecell-solver-config
BUILT_SOURCES=freecell-solver-config
freecell-solver-config: freecell-solver-config.in

EXTRA_DIST += fc-solve.pod fc-solve.6

man_MANS = fc-solve.6

lib_LTLIBRARIES = libfreecell-solver.la
libfreecell_solverincludedir=$(includedir)/freecell-solver
libfreecell_solver_la_SOURCES = alloc.c app_str.c caas.c card.c cmd_line.c fcs_dm.c fcs_hash.c fcs_isa.c freecell.c intrface.c lib.c lookup2.c move.c pqueue.c preset.c rand.c scans.c simpsim.c state.c
libfreecell_solver_la_LDFLAGS = -version-info 4:0:4
libfreecell_solverinclude_HEADERS = fcs_user.h fcs_enums.h fcs_move.h fcs_cl.h

fc-solve.6: fc-solve.pod ver.txt
	cat $< | sed 's/FCS_VERSION/@VERSION@/g' | pod2man --section=6 --center="Freecell Solver" --date="2001/06/09" --release="Freecell Solver @VERSION@" > $@


EXTRA_DIST += alloc.h app_str.h caas.h card.h fcs_dm.h fcs.h fcs_hash.h
EXTRA_DIST += fcs_isa.h jhjtypes.h inline.h lookup2.h ms_ca.h move.h
EXTRA_DIST += pqueue.h preset.h rand.h state.h test_arr.h tests.h

EXTRA_DIST += USAGE CREDITS
EXTRA_DIST += gen_makefile.pl Makefile.win32 Makefile.lite Makefile.gnu
EXTRA_DIST += config.h.orig config.h.dkds config.h.freecell
EXTRA_DIST += devel_docs/ devel_docs/Architecture.summary.txt devel_docs/Architecture.txt devel_docs/strategies.txt devel_docs/variants.txt
EXTRA_DIST += gen_presets.pl gen_def.sh
EXTRA_DIST += freecell-solver.def

EXTRA_DIST += ver.txt change_ver.sh
EXTRA_DIST += freecell-solver.spec freecell-solver.spec.in

EXTRA_DIST += make-dist.sh

EXTRA_DIST += freecell-solver-config.in config.h.in

EXTRA_DIST += scripts/test.pl scripts/FreeCell/Object.pm 
EXTRA_DIST += scripts/FreeCell/Card.pm scripts/FreeCell/State.pm




