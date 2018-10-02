#!/usr/bin/env python3

import os
from os.path import join
import re
import tempfile
from subprocess import call
import unittest


class VnuValidate:
    """docstring for VnuValidate"""
    def __init__(self, path, non_xhtml_re):
        self.path = path
        self.non_xhtml_re = non_xhtml_re

    def run(self):
        """docstring for run"""
        t = tempfile.TemporaryDirectory()
        for dirpath, _, fns in os.walk(self.path):
            dn = join(t.name, dirpath)
            os.makedirs(dn)
            for fn in fns:
                path = join(dirpath, fn)
                html = re.match(r'\.html?$', fn)
                if re.match(r'\.xhtml$', fn) or (
                        html and not re.match(self.non_xhtml_re, path)):
                    open(join(dn, fn), 'w').write(
                        open(join(dirpath,
                                  re.sub('\.[^\.]*$', '.xhtml',
                                         fn)).read()))
                elif html:
                    open(join(dn, fn), 'w').write(
                        open(path).read())

        return call(['java', '-jar', os.environ['VNU_JAR'], '--Werror',
                     '--skip-non-html', t.name]) == 0


class MyTests(unittest.TestCase):
    def test_main(self):
        dir_ = './dest'
        self.assertTrue(VnuValidate(dir_, r'jquery-ui').run())


if __name__ == '__main__':
    from pycotap import TAPTestRunner
    suite = unittest.TestLoader().loadTestsFromTestCase(MyTests)
    TAPTestRunner().run(suite)
