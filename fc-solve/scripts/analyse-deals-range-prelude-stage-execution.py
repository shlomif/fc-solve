# NOTE : this script needs to be rethought because it does not take into
# account soft threads (= scans) that finished without solving before their
# quota completion.

import re
import statistics
from six import print_


def iters_count_iterator(fn):
    with open(fn, 'r') as f:
        last = 0
        skip = False
        for l in f:
            if re.match(r'^Started', l):
                pass
            elif re.match(r'^Unsolved', l):
                skip = True
            else:
                m = re.match(r'^Reached Board.*\(total_num_iters=([0-9]+)\)$',
                             l)
                if m:
                    new = int(m.group(1))
                    if skip:
                        yield 'skip'
                        skip = False
                    else:
                        yield new - last
                    last = new


class PreludeAnalyzer(object):
    """docstring for PreludeAnalyzer"""
    def __init__(self, fn):
        with open(fn, 'r') as f:
            for l in f:
                m = re.match(r'^\s*--prelude "([^"]+)"', l)
                if m:
                    s = m.group(1)
                    lst = s.split(',')
                    _stages = []
                    for stage in lst:
                        m = re.match(r'^([0-9]+)@([0-9A-Za-z_]+)$', stage)
                        if not m:
                            raise BaseException('Invalid stage: ' + stage)
                        _stages.append({'quota': int(m.group(1)),
                                        'id': m.group(2), 'data': []})
                    _stages.append({'quota': int('1000000000000'),
                                    'id': 'AFTER', 'data': []})
                    self._stages = _stages
                    break
            else:
                raise BaseException('Cannot find prelude.')

    def insert(self, idx, i):
        print_("Got idx=%d iters=%d" % (idx, i))
        for s in self._stages:
            quota = s['quota']
            if i <= quota:
                print_("Put %d in %s" % (idx, s['id']))
                s['data'].append(i)
                break
            else:
                i -= quota
        else:
            raise BaseException('Cannot fit in prelude.')

    def report(self):
        for s in self._stages:
            d = s['data']
            prefix = '%d @ %s : ' % (s['quota'], s['id'])
            length = len(d)
            if length == 0:
                print_("%s%s" % (prefix, '[EMPTY]'))
            else:
                print_("%snum=%d min=%d max=%d avg=%f median=%f" %
                       (prefix, length, min(d), max(d), statistics.mean(d),
                        statistics.median(d)))


def main():
    p = PreludeAnalyzer('../scripts/TEST_OPTIMIZATIONS/obf-mod4.sh')
    for idx, i in enumerate(iters_count_iterator('serial.dump')):
        # print_(i)
        if i != 'skip':
            p.insert(idx+1, i)
    p.report()


if __name__ == "__main__":
    main()
