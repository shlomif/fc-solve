AUTOMAKE_OPTIONS = 1.4

EXTRA_DIST += freecell-solver-range-parallel-solve.pod
EXTRA_DIST += freecell-solver-range-parallel-solve.6

man_MANS = freecell-solver-range-parallel-solve.6

freecell-solver-range-parallel-solve.6: freecell-solver-range-parallel-solve.pod ../ver.txt
	cat $< | sed 's/FCS_VERSION/@VERSION@/g' > fc-solve.pod 
	pod2man --section=6 --center="Freecell Solver" --date="2001/06/09" --release="Freecell Solver @VERSION@" fc-solve.pod > $@
	rm -f fc-solve.pod

