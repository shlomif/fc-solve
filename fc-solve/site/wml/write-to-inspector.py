#! /usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2019 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.

import time


def main():
    o = None

    def pump(s, delay=2):
        o.write(s + "\n")
        o.flush()
        time.sleep(delay)

    o = open("/home/shlomif/in.pipe", "wt")

    pump("breakOnUncaught")
    pump("c", 5)
    pump("c")


main()
