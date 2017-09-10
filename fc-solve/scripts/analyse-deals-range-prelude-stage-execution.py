import re
import sys
from six import print_

if sys.version_info > (3,):
    long = int
    xrange = range


def iters_count_iterator(fn):
    with open(fn, 'r') as f:
        last = long(0)
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
                    new = long(m.group(1))
                    if skip:
                        skip = False
                    else:
                        yield new-last
                    last = long(m.group(1))


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
                        _stages.append({'quota': long(m.group(1)),
                                        'id': m.group(2), 'data': []})
                    _stages.append({'quota': long('1000000000000'),
                                    'id': 'AFTER', 'data': []})
                    self._stages = _stages
                    break
            else:
                raise BaseException('Cannot find prelude.')

    def insert(self, i):
        for s in self._stages:
            quota = s['quota']
            if i <= quota:
                s['data'].append(i)
                break
            else:
                i -= quota
        else:
            raise BaseException('Cannot fit in prelude.')


def main():
    p = PreludeAnalyzer('../scripts/TEST_OPTIMIZATIONS/obf-mod4.sh')
    for i in iters_count_iterator('serial.dump'):
        print_(i)
        p.insert(i)


if __name__ == "__main__":
    main()
