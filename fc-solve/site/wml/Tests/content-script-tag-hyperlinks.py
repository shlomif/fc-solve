#! /usr/bin/env python3
#
# Copyright © 2020 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under the terms of the MIT license.

import re
import unittest

from lxml import etree


class MyTests(unittest.TestCase):
    def test_main(self):
        input_fn = './dest/js-fc-solve/text/index.html'
        with open(input_fn, "rb") as fh:
            root = etree.HTML(fh.read())
            matches = root.xpath(".//script[@src]")
            filtered = []
            for x in matches:
                m = re.match('^((?:\\.\\./)*)js/', x.get('src'))
                if m:
                    filtered.append(m.group(1))

            self.assertTrue(len(filtered), "matches")
            self.assertEqual(
                [x for x in filtered if x != '../../'],
                [],
                "all are ok")

    def test_individual_faq_pages(self):
        input_fn = './dest/faq-indiv-nodes/a_move_seems_wrong.xhtml'
        with open(input_fn, "rb") as fh:
            root = etree.HTML(fh.read())
            matches = root.xpath(
                ".//link[ @rel = 'stylesheet' and @media = 'screen' and @href]"
            )
            filtered = []
            for x in matches:
                m = re.match(
                    '^((?:\\.\\./)*faq-indiv-pages\\.css)\\Z',
                    x.get('href'),
                )
                if m:
                    filtered.append(m.group(1))

            self.assertTrue(len(filtered), "matches")
            self.assertEqual(
                [x for x in filtered if re.match('\\.\\./\\.\\./', x)],
                [],
                "all are ok")


if __name__ == '__main__':
    from pycotap import TAPTestRunner
    suite = unittest.TestLoader().loadTestsFromTestCase(MyTests)
    TAPTestRunner().run(suite)
