# This file is part of Freecell Solver. It is subject to the license terms in
# the COPYING.txt file found in the top-level directory of this distribution
# and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
# Freecell Solver, including this file, may be copied, modified, propagated,
# or distributed except according to the terms contained in the COPYING file.
#
# Copyright (c) 2018 Shlomi Fish
export PATSOLVE_START=1 PATSOLVE_END=32000
date +"Started at %s.%N"
time ./threaded-pats -S
date +"Ended at %s.%N"
