#!/usr/bin/env python

import re
from os.path import expanduser

home = expanduser("~")

basedir = home + "/Backup/Arcs"


regex = re.compile(r'(?<=Length: )(-?[0-9]+)')

def extract(l):
    return int(regex.search(l).group(1))

NUM = 32000

def calc_init_state():
    state = {'start_nums' : [], 'mysum': 0, 'my_num_improved': 0, 'my_max': 0, }
    with open(basedir + "/FC_SOLVE_SUMMARIZE_RESULTS--fif-10.cat.txt") as fh:
        for l in fh:
            state['start_nums'].append(extract(l))
    assert len(state['start_nums']) == NUM

    state['run_nums'] = [x for x in state['start_nums']]

    return state

state = calc_init_state()

MAX_SEED = 4839
print( "Seed\tN\tSum\tMax" )
for seed in xrange(1, MAX_SEED + 1):
    with open(basedir + "/fcs-summary-len-seed/lens-theme1--seed=%d.txt" % seed) as fh:
        i = 0
        for l in fh:
            new = extract(l)
            if new >= 0:
                delta = state['run_nums'][i] - new
                if delta > 0:
                    orig = state['start_nums'][i]
                    if state['run_nums'][i] == orig:
                        state['my_num_improved'] += 1
                    state['mysum'] += delta
                    init_delta = orig - new
                    if init_delta > state['my_max']:
                        state['my_max'] = init_delta
                    state['run_nums'][i] = new
            i += 1
    print( "%d\t%d\t%d\t%d" % (seed, state['my_num_improved'], state['mysum'], state['my_max']))

state['reached_seed'] = MAX_SEED
