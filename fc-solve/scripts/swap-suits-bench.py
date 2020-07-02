#! /usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2020 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under the terms of the MIT license.
import os
import subprocess


def main():
    for count_deals in [32000, 200000]:
        for FCS_SWAP_COLOR in range(4):
            os.putenv('FCS_SWAP_COLOR', str(FCS_SWAP_COLOR))
            for FCS_SWAP_BLACK in range(4):
                os.putenv('FCS_SWAP_BLACK', str(FCS_SWAP_BLACK))
                for FCS_SWAP_RED in range(4):
                    os.putenv('FCS_SWAP_RED', str(FCS_SWAP_RED))
                    fn = (("noloadlg--FCS_SWAP_COLOR={}--FCS_SWAP_BLACK={}--" +
                           "FCS_SWAP_RED={}--" +
                           "count_deals={}.report.txt").format(
                            FCS_SWAP_COLOR,
                            FCS_SWAP_BLACK,
                            FCS_SWAP_RED,
                            count_deals))
                    with open(fn, 'wt') as f:
                        output = subprocess.getoutput(
                                ('./freecell-solver-multi-thread-solve 1 ' +
                                 '{} 4000').format(count_deals))
                        print(fn, output, fn)
                        f.write(output)


main()
