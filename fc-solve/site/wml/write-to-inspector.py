#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2019 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.

"""

"""

import time


def main():
    """docstring for main"""
    o = open("/home/shlomif/in.pipe", "wt")
    o.write("breakOnUncaught\n")
    o.flush()
    time.sleep(2)
    o.write("c\n")
    o.flush()
    time.sleep(0.2)


main()
