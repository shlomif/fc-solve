#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2018 shlomif <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.

"""

"""

import os.path
import re
import subprocess
import sys

idx = int(sys.argv[1])
SEG = 100000
start = idx * SEG + 1
MAX = ((1 << 33) - 1)
end = min((idx + 1) * SEG, MAX)

fn = 'results/%010d' % idx
if os.path.exists(fn):
    sys.exit()
with open(fn, 'wt') as o:
    def w(cmd, deal):
        o.write("%s\t%d\n" % (cmd, int(deal)))
        o.flush()
    w('Start', start)
    pipe = subprocess.Popen([
        './freecell-solver-range-parallel-solve',
        str(start), str(end), '10000',
        '--freecells-num', '0', '-to', '0AB', '-sp', 'r:tf', '-mi', '1000000'
        ],
        bufsize=10000, stdout=subprocess.PIPE).stdout
    for line in pipe:
        m = re.search(r'(Solved|Intractable) Board No\. ([0-9]+)', line)
        if m:
            w(('Int' if m.group(1) == 'Intractable' else 'S'), m.group(2))
        elif re.search('Reached', line):
            sys.stderr.write(line)
    w('End', end)
