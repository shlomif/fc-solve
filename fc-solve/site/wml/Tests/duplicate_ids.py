#! /usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2019 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.

import unittest

import html_unit_test
from html_unit_test import HtmlCheckDuplicateIDs


class MyTests(html_unit_test.TestCase):
    def test_main(self):
        ret, dups = HtmlCheckDuplicateIDs().check_dir_tree(
            root="dest/"
        )
        self.assertTrue(ret, "HtmlCheckDuplicateIDs")
        ret, dups = HtmlCheckDuplicateIDs().check_dir_tree(
            root="dest-prod/"
        )
        self.assertTrue(ret, "HtmlCheckDuplicateIDs")


if __name__ == '__main__':
    from pycotap import TAPTestRunner
    suite = unittest.TestLoader().loadTestsFromTestCase(MyTests)
    TAPTestRunner().run(suite)
