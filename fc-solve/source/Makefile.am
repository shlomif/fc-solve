AUTOMAKE_OPTIONS = 1.4

SUBDIRS = board_gen man Presets

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
libfreecell_solver_la_SOURCES = alloc.c app_str.c caas.c card.c cl_chop.c cmd_line.c fcs_dm.c fcs_hash.c fcs_isa.c freecell.c intrface.c lib.c lookup2.c move.c pqueue.c preset.c rand.c scans.c simpsim.c state.c tests.c
libfreecell_solver_la_LDFLAGS = -version-info 5:0:5
libfreecell_solverinclude_HEADERS = fcs_user.h fcs_enums.h fcs_move.h fcs_cl.h

fc-solve.6: fc-solve.pod ver.txt
	cat $< | sed 's/FCS_VERSION/@VERSION@/g' > fc-solve.pm 
	pod2man --section=6 --center="Freecell Solver" --date="2001/06/09" --release="Freecell Solver @VERSION@" fc-solve.pm > $@
	rm -f fc-solve.pm

noinst_SCRIPTS = Makefile.win32

Makefile.am: gen_makefile.pl
	perl $<

Makefile.gnu: gen_makefile.pl
	perl $<

Makefile.win32: gen_makefile.pl
	perl $< --arch=win32

test_num.h: gen_tests_lists.pl
	perl $<

#<<<HEADERS.START
EXTRA_DIST += alloc.h app_str.h caas.h card.h cl_chop.h
EXTRA_DIST += ds_order.h fcs_cl.h fcs.h fcs_dm.h fcs_enums.h
EXTRA_DIST += fcs_hash.h fcs_isa.h fcs_move.h fcs_user.h inline.h
EXTRA_DIST += jhjtypes.h lookup2.h move.h ms_ca.h pqueue.h
EXTRA_DIST += preset.h rand.h state.h test_arr.h test_num.h
EXTRA_DIST += tests.h
#>>>HEADERS.END

EXTRA_DIST += USAGE CREDITS README.win32.txt
EXTRA_DIST += gen_makefile.pl Makefile.lite Makefile.gnu Makefile.win32
EXTRA_DIST += config.h.orig config.h.dkds config.h.freecell config.h.win32
EXTRA_DIST += prefix.h.freecell
EXTRA_DIST += devel_docs/ devel_docs/Architecture.summary.txt devel_docs/Architecture.txt devel_docs/strategies.txt devel_docs/variants.txt
EXTRA_DIST += gen_presets.pl gen_def.sh gen_tests_lists.pl
EXTRA_DIST += freecell-solver.def

EXTRA_DIST += ver.txt change_ver.sh
EXTRA_DIST += freecell-solver.spec freecell-solver.spec.in

EXTRA_DIST += make-dist.sh

EXTRA_DIST += freecell-solver-config.in config.h.in prefix.h.proto.in 
EXTRA_DIST += prefix.h.win32

EXTRA_DIST += scripts/test.pl scripts/FreeCell/Object.pm 
EXTRA_DIST += scripts/FreeCell/Card.pm scripts/FreeCell/State.pm




