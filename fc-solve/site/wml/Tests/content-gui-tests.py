#! /usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2019 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.

import unittest

from lxml import etree


class MyTests(unittest.TestCase):
    def test_main(self):
        input_fn = './dest/js-fc-solve/text/gui-tests.xhtml'
        with open(input_fn, "rb") as fh:
            root = etree.HTML(fh.read())
            self.assertTrue(len(root.xpath(
                ".//script")))
            self.assertTrue(len(root.xpath(
                ".//textarea")))


if __name__ == '__main__':
    from pycotap import TAPTestRunner
    suite = unittest.TestLoader().loadTestsFromTestCase(MyTests)
    TAPTestRunner().run(suite)
