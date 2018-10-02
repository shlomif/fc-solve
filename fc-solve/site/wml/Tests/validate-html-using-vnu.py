#!/usr/bin/env python3

import os
from os.path import join
import re
import tempfile
from subprocess import call
import unittest


class VnuValidate:
    """docstring for VnuValidate"""
    def __init__(self, path, jar, non_xhtml_re):
        self.path = path
        self.jar = jar
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

        return call(['java', '-jar', self.jar, '--Werror',
                     '--skip-non-html', t.name]) == 0


class MyTests(unittest.TestCase):
    def test_main(self):
        dir_ = './dest'
        key = 'HTML_VALID_VNU_JAR'
        if key in os.environ:
            self.assertTrue(
                VnuValidate(dir_, os.environ[key], r'jquery-ui').run())
        else:
            self.assertTrue(True, key + ' not set')


if __name__ == '__main__':
    from pycotap import TAPTestRunner
    suite = unittest.TestLoader().loadTestsFromTestCase(MyTests)
    TAPTestRunner().run(suite)
