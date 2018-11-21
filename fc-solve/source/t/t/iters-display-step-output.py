#!/usr/bin/env python3

import subprocess
import os
import re
import unittest

prog = os.environ['FCS_PATH'] + '/fc-solve'


def _normalize_lf(txt):
    return txt.replace("\r\n", "\n")


class MyTests(unittest.TestCase):
    def mytest(self, cmd, input_text, want_prefix, msg):
        process = subprocess.Popen(
            cmd,
            shell=False, stdin=subprocess.PIPE, stdout=subprocess.PIPE
        )
        process.stdin.write(bytes(input_text, 'utf_8'))
        process.stdin.close()
        got_out = _normalize_lf(process.stdout.read().decode('utf-8'))
        self.assertTrue(re.match(want_prefix, got_out), msg)

    def test_main(self):
        # plan(6)

        # TEST
        self.mytest(
            [prog, '-i', '--iter-output-step', '100', '-mi', '1000', '-'],
            """9D 6S 4D AH 5C 4H KH
JH JC JS 7S 6D AC 4S
AD 8D AS 2H KC KD 7H
KS TS 9S 8H JD QD 3H
TC 4C 2S 5D 3S 6H
3C QC 5S QS QH 7D
9H 5H 7C 6C 3D TD
2C 8C 8S TH 2D 9C
""",
            ('^' + "".join([(("Iteration: %d\nDepth: [0-9]+\n" +
                            "Stored-States: [0-9]+\nScan:[^\\n]+\n\n") %
                             (i)) for i in [0, 100, 200, 300]])),
            "Freecell MS deal 1941 initial",
        )


if __name__ == '__main__':
    from pycotap import TAPTestRunner
    suite = unittest.TestLoader().loadTestsFromTestCase(MyTests)
    TAPTestRunner().run(suite)
