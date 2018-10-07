#!/usr/bin/env python3

import re
import unittest
import vnu_validator


class MyTests(vnu_validator.VnuTest):
    def test_main(self):
        def _create_cb(s):
            """docstring for _create_cb"""
            _re = re.compile(s)

            def _cb(path):
                return re.search(_re, path)
            return _cb

        def _skip_cb(path):
            return False
        return self.vnu_test_dir('./dest', _create_cb('jquery-ui'), _skip_cb)


if __name__ == '__main__':
    from pycotap import TAPTestRunner
    suite = unittest.TestLoader().loadTestsFromTestCase(MyTests)
    TAPTestRunner().run(suite)
