#!/usr/bin/env python

import re
from os.path import expanduser

home = expanduser("~")

basedir = home + "/Backup/Arcs"

start_nums = []

regex = re.compile(r'(?<=Length: )(-?[0-9]+)')

def extract(l):
    return int(regex.search(l).group(1))

with open(basedir + "/FC_SOLVE_SUMMARIZE_RESULTS--fif-10.cat.txt") as fh:
    for l in fh:
        start_nums.append(extract(l))

NUM = 32000
assert len(start_nums) == NUM

run_nums = [x for x in start_nums]
mysum = 0
my_num_improved = 0
my_max = 0


print( "Seed\tN\tSum\tMax" )
for seed in xrange(1, 4227):
    with open(basedir + "/fcs-summary-len-seed/lens-theme1--seed=%d.txt" % seed) as fh:
        i = 0
        for l in fh:
            new = extract(l)
            if new >= 0:
                delta = run_nums[i] - new
                if delta > 0:
                    if run_nums[i] == start_nums[i]:
                        my_num_improved += 1
                    mysum += delta
                    init_delta = start_nums[i] - new
                    if init_delta > my_max:
                        my_max = init_delta
                    run_nums[i] = new
            i += 1
    print( "%d\t%d\t%d\t%d" % (seed, my_num_improved, mysum, my_max))

