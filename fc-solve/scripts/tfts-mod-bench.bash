#! /bin/bash
#
# tfts-mod-bench.bash
# Copyright (C) 2018 shlomif <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.
#
perl ../scripts/gen-tfts-mod-1.pl > tfts-mod1.sh
time ./freecell-solver-multi-thread-solve 1 32000 1000 --num-workers 4 --read-from-file 4,tfts-mod1.sh
