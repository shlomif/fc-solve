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


def _count_link_rels(root, needle):
    return len(root.xpath(
        ".//link[@rel='canonical' and @href='{}']".format(needle)))


CANONICAL_URL = 'https://fc-solve.shlomifish.org/'


class MyTests(unittest.TestCase):
    def test_main(self):
        input_fn = './dest/index.html'
        root = _html_tree(input_fn)
        self.assertEqual(
            _count_link_rels(
                root,
                CANONICAL_URL,
            ),
            1,
            'root link rel'
        )

    def test_non_dir_path(self):
        input_fn = './dest/links.html'
        root = _html_tree(input_fn)
        self.assertEqual(
            _count_link_rels(
                root,
                CANONICAL_URL + "links.html",
            ),
            1,
            'non-dir path test',
        )

    def test_production(self):
        input_fn = './dest-prod/js-fc-solve/text/index.html'
        root = _html_tree(input_fn)
        self.assertEqual(
            _count_link_rels(
                root,
                CANONICAL_URL + 'js-fc-solve/text/',
            ),
            1,
            'production js-fc-solve link rel')


if __name__ == '__main__':
    from pycotap import TAPTestRunner
    suite = unittest.TestLoader().loadTestsFromTestCase(MyTests)
    TAPTestRunner().run(suite)
