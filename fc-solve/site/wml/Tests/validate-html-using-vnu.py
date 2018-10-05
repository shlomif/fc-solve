#!/usr/bin/env python3
"""

=head1 COPYRIGHT & LICENSE

Copyright 2018 by Shlomi Fish

This program is distributed under the MIT / Expat License:
L<http://www.opensource.org/licenses/mit-license.php>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut
"""

import os
from os.path import join
import re
import tempfile
from subprocess import call
import unittest


class VnuValidate:
    """docstring for VnuValidate"""
    def __init__(self, path, jar, non_xhtml_cb, skip_cb):
        self.path = path
        self.jar = jar
        self.non_xhtml_cb = non_xhtml_cb
        self.skip_cb = skip_cb

    def run(self):
        """docstring for run"""
        t = tempfile.TemporaryDirectory()
        for dirpath, _, fns in os.walk(self.path):
            dn = join(t.name, dirpath)
            os.makedirs(dn)
            for fn in fns:
                path = join(dirpath, fn)
                if self.skip_cb(path):
                    continue
                html = re.match(r'.*\.html?$', fn)
                if re.match('.*\\.xhtml$', fn) or (
                        html and not self.non_xhtml_cb(path)):
                    open(join(dn, re.sub('\.[^\.]*$', '.xhtml',
                                         fn)), 'w').write(
                        open(join(dirpath, fn)).read())
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
            def _create_cb(s):
                """docstring for _create_cb"""
                _re = re.compile(s)

                def _cb(path):
                    return re.search(_re, path)
                return _cb

            def _skip_cb(path):
                return False
            _non_xhtml_cb = _create_cb('jquery-ui')
            self.assertTrue(
                VnuValidate(dir_, os.environ[key], _non_xhtml_cb,
                            _skip_cb).run())
        else:
            self.assertTrue(True, key + ' not set')


if __name__ == '__main__':
    from pycotap import TAPTestRunner
    suite = unittest.TestLoader().loadTestsFromTestCase(MyTests)
    TAPTestRunner().run(suite)
