#!/usr/bin/env python

import re
import sys
from os.path import expanduser, isfile
import json

if sys.version_info > (3,):
    xrange = range

MAX_SEED = 10294

home = expanduser("~")

basedir = home + "/Backup/Arcs"

STATE_FN = basedir + "/FC_SOLVE_SUMMARIZE_RESULTS--state.json"

extract_len_re = re.compile(r'(?<=Length: )(-?[0-9]+)')


def extract(l):
    return int(extract_len_re.search(l).group(1))


NUM_DEALS = 32000


def calc_init_state():
    state = {'start_nums': [], 'reached_seed': 1, 'mysum': 0,
             'my_num_improved': 0, 'my_max': 0, 'output': '', }
    with open(basedir + "/FC_SOLVE_SUMMARIZE_RESULTS--fif-10.cat.txt") as fh:
        for l in fh:
            state['start_nums'].append(extract(l))
    assert len(state['start_nums']) == NUM_DEALS

    state['run_nums'] = list(state['start_nums'])

    return state


def write_state(state):
    with open(STATE_FN, 'w') as fh:
        fh.write(json.dumps(state))


if not isfile(STATE_FN):
    write_state(calc_init_state())

state = None
with open(STATE_FN) as fh:
    state = json.loads(fh.read())

print("Seed\tN\tSum\tMax")
sys.stdout.write(state['output'])
for seed in xrange(state['reached_seed'], MAX_SEED + 1):
    with open(basedir +
              "/fcs-summary-len-seed/lens-theme1--seed=%d.txt" % seed) as fh:
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
    out_line = ("%d\t%d\t%d\t%d" %
                (seed, state['my_num_improved'],
                 state['mysum'], state['my_max']))
    state['output'] += out_line + "\n"
    print(out_line)

state['reached_seed'] = MAX_SEED
write_state(state)
