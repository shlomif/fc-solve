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


noinst_SCRIPTS = Makefile.win32

Makefile.win32: gen_makefile.pl
	perl $< --arch=win32 | add_cr.pl - > $@

#<<<HEADERS.START
EXTRA_DIST += alloc.h app_str.h caas.h card.h config.h
EXTRA_DIST += fcs_cl.h fcs.h fcs_dm.h fcs_enums.h fcs_hash.h
EXTRA_DIST += fcs_isa.h fcs_move.h fcs_user.h inline.h jhjtypes.h
EXTRA_DIST += move.h ms_ca.h pqueue.h preset.h rand.h
EXTRA_DIST += state.h test_arr.h tests.h
#>>>HEADERS.END

EXTRA_DIST += USAGE CREDITS README.win32.txt
EXTRA_DIST += gen_makefile.pl Makefile.lite Makefile.gnu Makefile.win32
EXTRA_DIST += config.h.orig config.h.dkds config.h.freecell config.h.win32
EXTRA_DIST += devel_docs/ devel_docs/Architecture.summary.txt devel_docs/Architecture.txt devel_docs/strategies.txt devel_docs/variants.txt
EXTRA_DIST += gen_presets.pl gen_def.sh
EXTRA_DIST += freecell-solver.def

EXTRA_DIST += ver.txt change_ver.sh
EXTRA_DIST += freecell-solver.spec freecell-solver.spec.in

EXTRA_DIST += make-dist.sh

EXTRA_DIST += freecell-solver-config.in config.h.in

EXTRA_DIST += scripts/test.pl scripts/FreeCell/Object.pm 
EXTRA_DIST += scripts/FreeCell/Card.pm scripts/FreeCell/State.pm




