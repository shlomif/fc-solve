#!/usr/bin/env python3

import subprocess
import os
import unittest

py_prog = os.environ['FCS_SRC_PATH'] + '/board_gen/transpose-freecell-board.py'


def _normalize_lf(txt):
    return txt.replace("\r\n", "\n")


class MyTests(unittest.TestCase):
    def mytest(self, input_text, want_out, msg):
        global py_prog
        process = subprocess.Popen(
            ['python3', py_prog, '-'],
            shell=False, stdin=subprocess.PIPE, stdout=subprocess.PIPE
        )
        process.stdin.write(bytes(input_text, 'utf_8'))
        process.stdin.close()
        got_out = process.stdout.read().decode('utf-8')

        self.assertEqual(_normalize_lf(got_out), _normalize_lf(want_out), msg)

    def test_main(self):
        # plan(6)

        # TEST
        self.mytest(
            """Foundations:
Freecells:
4C 5H QC 5D 2D 7H AH 7S
2C QH 9S 2S KD JS 5S 6C
9C 3C 6H JC TH KH 6S 7D
8C AC 9H 5C TC TS AD 4D
QS 3H 3S JH TD KC 8H 8S
4S 4H KS 6D 8D 7C JD 9D
2H QD 3D AS
""",
            """Foundations:
Freecells:
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D
""",
            "Freecell MS deal 24 initial",
        )

        # TEST
        self.mytest(
            """Foundations:
Freecells:
4C 5H QC 5D 2D 7H AH 7S
2H QH 9S 2S KD JS 5S 6C
9C 3C 6H JC TH KH 6S 7D
8C AC 9H 5C TC TS AD 4D
QS 3H 3S JH TD KC 8H 8S
4S 4H KS 6D 8D 7C JD 9D
   QD 3D AS
      2C
""",
            """Foundations:
Freecells:
: 4C 2H 9C 8C QS 4S
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D 2C
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D
""",
            "With longer column."
        )

        # TEST
        self.mytest(
            """Foundations:
Freecells:
4C 5H QC 5D 2D 7H AH 7S
2H QH 9S 2S KD JS 5S 6C
9C 3C 6H JC TH KH 6S 7D
8C AC 9H 5C TC TS AD 4D
QS 3H 3S JH TD    8H 8S
   4H KS 6D 8D    JD
   QD 3D AS       KC
      2C          9D
      4S
      7C
""",
            """Foundations:
Freecells:
: 4C 2H 9C 8C QS
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D 2C 4S 7C
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 8D
: 7H JS KH TS
: AH 5S 6S AD 8H JD KC 9D
: 7S 6C 7D 4D 8S
""",
            "With two longer columns."
        )

        # TEST
        self.mytest(
            """Freecells:
4C 5H QC 5D 2D 7H AH 7S
2H QH 9S 2S KD JS 5S 6C
9C 3C 6H JC TH KH 6S 7D
8C AC 9H 5C TC TS AD 4D
QS 3H 3S JH TD    8H 8S
   4H KS 6D 8D    JD
   QD 3D AS       KC
      2C          9D
      4S
      7C
""",
            """Freecells:
: 4C 2H 9C 8C QS
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D 2C 4S 7C
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 8D
: 7H JS KH TS
: AH 5S 6S AD 8H JD KC 9D
: 7S 6C 7D 4D 8S
""",
            "Without a foundations line"
        )

        # TEST
        self.mytest(
            """Foundations:
4C 5H QC 5D 2D 7H AH 7S
2H QH 9S 2S KD JS 5S 6C
9C 3C 6H JC TH KH 6S 7D
8C AC 9H 5C TC TS AD 4D
QS 3H 3S JH TD    8H 8S
   4H KS 6D 8D    JD
   QD 3D AS       KC
      2C          9D
      4S
      7C
""",
            """Foundations:
: 4C 2H 9C 8C QS
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D 2C 4S 7C
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 8D
: 7H JS KH TS
: AH 5S 6S AD 8H JD KC 9D
: 7S 6C 7D 4D 8S
""",
            "Without a Freecells line"
        )

        # TEST
        self.mytest(
            """4C 5H QC 5D 2D 7H AH 7S
2H QH 9S 2S KD JS 5S 6C
9C 3C 6H JC TH KH 6S 7D
8C AC 9H 5C TC TS AD 4D
QS 3H 3S JH TD    8H 8S
   4H KS 6D 8D    JD
   QD 3D AS       KC
      2C          9D
      4S
      7C
""",
            """: 4C 2H 9C 8C QS
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D 2C 4S 7C
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 8D
: 7H JS KH TS
: AH 5S 6S AD 8H JD KC 9D
: 7S 6C 7D 4D 8S
""",
            "With neither a Freecells line nor a Foundations line."
        )


if __name__ == '__main__':
    from pycotap import TAPTestRunner
    suite = unittest.TestLoader().loadTestsFromTestCase(MyTests)
    TAPTestRunner().run(suite)
