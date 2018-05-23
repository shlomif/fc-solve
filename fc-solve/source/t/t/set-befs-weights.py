#!/usr/bin/env python3

from FC_Solve import FC_Solve_Suite
import unittest
# TEST:source "$^CURRENT_DIRNAME/../lib/FC_Solve/__init__.py"


class MyTests(unittest.TestCase):
    def test_main(self):
        fcs = FC_Solve_Suite(self)
        # TEST*$test_befs
        fcs.test_befs_weights(
            "Simple - all integers",
            "5,4,3,0,2",
            [5.0, 4.0, 3.0, 0.0, 2.0])

        # TEST*$test_befs
        fcs.test_befs_weights(
            "Simple - fractions",
            "0.2,0.3,0.4,0.5,0.6",
            [0.2, 0.3, 0.4, 0.5, 0.6])

        # TEST*$test_befs
        fcs.test_befs_weights(
            "Testing end after three numbers",
            "0.2,0.35,0.4\x00500\x00600.7\x00",
            [0.2, 0.35, 0.4, 0, 0])

        # TEST*$test_befs
        fcs.test_befs_weights(
            "One number with a trailing comma",
            "100,\x002,3,4,5,6,7,8,9,100\x00",
            [100, 0, 0, 0, 0])


if __name__ == "__main__":
    # plan(24)
    from pycotap import TAPTestRunner
    suite = unittest.TestLoader().loadTestsFromTestCase(MyTests)
    TAPTestRunner().run(suite)
