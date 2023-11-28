#! /usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2020 Shlomi Fish < https://www.shlomifish.org/ >
#
# Licensed under the terms of the MIT license.

"""

"""

import os
from os.path import join
import unittest

from lxml import html


class HtmlTestsDocQuery:
    """Single query results"""
    def __init__(self, doc, xpath_results):
        self.doc = doc
        self.xpath_results = xpath_results

    def __len__(self):
        """implementing len(self)"""
        return len(self.xpath_results)


class HtmlTestsDoc:
    """A single HTML document wrapper"""
    def __init__(self, harness, fn):
        self.harness = harness
        if isinstance(fn, dict):
            assert fn['type'] == 'text'
            self.fn = fn['fn']
            self.root = html.document_fromstring(html=fn['text'])
            return
        self.fn = fn
        self.root = html.parse(fn)

    def xpath(self, xpath_s):
        return HtmlTestsDocQuery(self, self.root.xpath(xpath_s))

    def has_count(self, xpath_s, count_, blurb=""):
        """is the length of xpath_s’s results count_"""
        self.harness.assertEqual(len(self.xpath(xpath_s)), count_, blurb)

    def has_one(self, xpath_s, blurb=""):
        """is the length of xpath_s’s results 1"""
        self.has_count(xpath_s=xpath_s, count_=1, blurb=blurb)

    def has_none(self, xpath_s, blurb=""):
        """is the length of xpath_s’s results 0"""
        self.has_count(xpath_s=xpath_s, count_=0, blurb=blurb)


class TestCase(unittest.TestCase):
    def doc(self, path):
        return HtmlTestsDoc(self, path)


def _find_htmls(root):
    """find X/HTML files in 'root'"""
    for dirpath, _, fns in os.walk(root):
        for fn in fns:
            if fn.endswith(('.html', '.xhtml')):
                path = join(dirpath, fn)
                yield path


def _test_finder():
    for x in _find_htmls(root="dest/post-incs/t2/"):
        print(x)


# _test_finder()


class HtmlCheckDuplicateIDs:
    def check_dir_tree(self, root):
        return self.check_files(
            paths_list=_find_htmls(root=root)
        )

    def check_files(self, paths_list):
        offending_files = {}
        for input_fn in paths_list:
            # doc = self.doc(input_fn)
            doc = HtmlTestsDoc(None, input_fn)
            ids_list = doc.xpath('//*/@id').xpath_results
            found = set()
            dups = set()
            for x in ids_list:
                if x in found:
                    # print("{}: [ {} ]".format(input_fn, x))
                    dups.add(x)
                else:
                    found.add(x)
            if len(dups) > 0:
                print("DUPLICATES! {}:".format(input_fn), sorted(list(dups)))
                offending_files[input_fn] = dups
                # raise BaseException(x)
            # print("{}:".format(input_fn), sorted(list(ids_list)))
        ret = (len(offending_files) == 0)
        assert ret
        return ret, offending_files
