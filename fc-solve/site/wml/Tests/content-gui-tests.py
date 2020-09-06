#! /usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2019 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.

import unittest

from lxml import etree


def _html_tree(input_fn):
    with open(input_fn, "rb") as fh:
        root = etree.HTML(fh.read())
    return root


def _count_script_srcs(root, needle):
    return len(root.xpath(
        ".//script[contains(@src, '{}')]".format(needle)))


class MyTests(unittest.TestCase):
    def test_main(self):
        def _count_jquery_ui(root):
            return len(root.xpath(
                ".//script[contains(@src, 'js/jquery-ui/jquery-ui.min.js')]"))
        input_fn = './dest/js-fc-solve/text/gui-tests.xhtml'
        root = _html_tree(input_fn)
        self.assertTrue(len(root.xpath(
            ".//script")))
        self.assertTrue(len(root.xpath(
            ".//textarea")))
        self.assertEqual(_count_jquery_ui(root), 0)
        input_fn = './dest/js-fc-solve/text/index.html'
        root = _html_tree(input_fn)
        self.assertTrue(_count_jquery_ui(root))

    def test_production(self):
        input_fn = './dest-prod/js-fc-solve/text/index.html'
        root = _html_tree(input_fn)
        self.assertEqual(
            _count_script_srcs(root, '../../js/jq.js'),
            1,
            "loading jquery"
        )
        self.assertEqual(
            _count_script_srcs(root, 'jquery-dev.js'),
            0,
            "not loading jquery-dev"
        )


if __name__ == '__main__':
    from pycotap import TAPTestRunner
    suite = unittest.TestLoader().loadTestsFromTestCase(MyTests)
    TAPTestRunner().run(suite)
