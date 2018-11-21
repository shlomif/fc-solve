#!/usr/bin/env python3

# TEST:source "$^CURRENT_DIRNAME/../lib/FC_Solve/__init__.py"
from FC_Solve import FC_Solve_Suite
import re
import os
import unittest


class MyTests(unittest.TestCase):
    def _eq(self, x, y, blurb):
        return self.assertEqual(x, y, blurb)

    def test_resume_solution(self):
        testname = "With RunIndef - "

        fcs = FC_Solve_Suite(self)

        limit = 10
        fcs.limit_iterations(limit)

        ret = fcs.solve_board(
            """4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD TH TC TD 8D
7H JS KH TS KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
"""
        )

        iters_count_ok = 1

        while (ret == 5):
            if (fcs.get_num_times() != limit):
                iters_count_ok = 0

            limit += 10
            fcs.limit_iterations(limit)
            ret = fcs.resume_solution()

        # TEST
        self._eq(ret, 0, testname + "State was successfully solved.")

        # TEST
        self._eq(iters_count_ok, 1,
                 testname + "Iters count was OK throughout the solution.")

    def test_num_states_in_collection_after_recycle(self):
        testname = "NumStatesInCol-After-Recycle - "

        fcs = FC_Solve_Suite(self)

        # MS-Freeceel board No. 24.
        fcs.solve_board(
            """4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD TH TC TD 8D
7H JS KH TS KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
"""
        )

        # TEST
        self._eq(fcs.get_num_times(), 137, testname + "Get num times is OK.")

        # TEST
        self._eq(fcs.get_num_states_in_col(), 191,
                 testname + "Num-states-in-collection is OK.")

        fcs.recycle()

        # TEST
        self._eq(fcs.get_num_times(), 0,
                 testname + "Get num times immediately after recycle.")

        # TEST
        self._eq(fcs.get_num_states_in_col(), 0, testname +
                 "Num-states-in-collection immediately after recycle.")

        # MS-Freeceel board No. 24.
        fcs.solve_board(
            """4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD TH TC TD 8D
7H JS KH TS KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
"""
        )

        # TEST
        self._eq(fcs.get_num_times(), 137, testname +
                 "Get num times after recycle.")

        # TEST
        self._eq(fcs.get_num_states_in_col(), 191,
                 testname + "Num-states-in-collection after recycle.")

    def test_num_states_in_collection_after_unsolved(self):
        testname = "NumStatesInCol-After-unsolved - "

        tags_str = os.getenv('FCS_TEST_TAGS')
        if not tags_str:
            tags_str = ''

        if re.search(r'\bfc_only\b', tags_str):
            return

        fcs = FC_Solve_Suite(self)

        # TEST*$input_cmd_line
        fcs.input_cmd_line("bakers_game", ['-g', 'bakers_game'])

        # MS-Freeceel board No. 10
        fcs.solve_board(
            """5S KD JC TS 9D KH 8D
5H 2S 9H 7H TD AD 6D
6H QD 6C TC AH 8S TH
6S 2D 7C QC QS 7D 3H
5D AS 7S KC 3D AC
4D 9C QH 4H 4C 5C
2H 3S 8H 9S JS 4S
JH JD 3C KS 2C 8C
"""
        )

        # TEST
        self._eq(fcs.get_num_times(), 3436, testname + "Get num times is OK.")

        # TEST
        self._eq(fcs.get_num_states_in_col(), 3436,
                 testname + "Num-states-in-collection is OK.")

    def test_resume_solution_with_flares(self):
        testname = "-l ve on iterative limiting - "

        fcs = FC_Solve_Suite(self)

        step = 1000
        hard_limit = 100000
        limit = step
        # TEST*$input_cmd_line
        fcs.input_cmd_line("video-editing", ['-l', 'video-editing'])
        fcs.limit_iterations(limit)

        # MS deal No. 124
        ret = fcs.solve_board(
            """7S AS 2C QS TH QD 7D
5D 8D 9D JH QH 5C JD
6D TD 5H 2S 6S TC KS
TS 4S 3D 9C 3C KD 7C
6H 5S 9H 6C KC AH
AC 4C 8S 2D QC JS
9S KH 8C 4D 7H 4H
2H 3S 8H AD 3H JC
"""
        )

        iters_count_ok = 1

        while (ret == 5 and limit < hard_limit):
            if (fcs.get_num_times() != limit):
                iters_count_ok = 0

            limit += step
            fcs.limit_iterations(limit)
            ret = fcs.resume_solution()

        # TEST
        self._eq(ret, 0, testname + "State was successfully solved.")

        # TEST
        self._eq(iters_count_ok, 1,
                 testname + "Iters count was OK throughout the solution.")


if __name__ == "__main__":
    # plan(14)
    from pycotap import TAPTestRunner
    suite = unittest.TestLoader().loadTestsFromTestCase(MyTests)
    TAPTestRunner().run(suite)
