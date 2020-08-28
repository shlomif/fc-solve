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
        def _count_jquery_ui(root):
            return len(root.xpath(
                ".//script[contains(@src, 'js/jquery-ui/jquery-ui.min.js')]"))
        input_fn = './dest/js-fc-solve/text/gui-tests.xhtml'
        with open(input_fn, "rb") as fh:
            root = etree.HTML(fh.read())
            self.assertTrue(len(root.xpath(
                ".//script")))
            self.assertTrue(len(root.xpath(
                ".//textarea")))
            self.assertEqual(_count_jquery_ui(root), 0)
        input_fn = './dest/js-fc-solve/text/index.html'
        with open(input_fn, "rb") as fh:
            root = etree.HTML(fh.read())
            self.assertTrue(_count_jquery_ui(root))


if __name__ == '__main__':
    from pycotap import TAPTestRunner
    suite = unittest.TestLoader().loadTestsFromTestCase(MyTests)
    TAPTestRunner().run(suite)
