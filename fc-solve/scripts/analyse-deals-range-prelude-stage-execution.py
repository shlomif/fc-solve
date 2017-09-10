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


def main():
    for i in iters_count_iterator('serial.dump'):
        print_(i)


if __name__ == "__main__":
    main()
