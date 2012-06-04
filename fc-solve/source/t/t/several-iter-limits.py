#!/usr/bin/env python

import sys
sys.path.append("./t/lib");

from TAP.Simple import *
# TEST:source "$^CURRENT_DIRNAME/lib/FC_Solve/__init__.py"
from FC_Solve import FC_Solve

plan(11)

def test_resume_solution():
    testname = "With RunIndef"

    fcs = FC_Solve()

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
""")

    iters_count_ok = 1

    while (ret == 5):
        if (fcs.get_num_times() != limit):
            iters_count_ok = 0

        limit += 10
        fcs.limit_iterations(limit)
        ret = fcs.resume_solution()

    # TEST
    ok (ret == 0, "State was successfully solved.")

    # TEST
    ok (iters_count_ok == 1, "Iters count was OK throughout the solution.")

def test_num_states_in_collection_after_recycle():
    testname = "NumStatesInCol-After-Recycle"

    fcs = FC_Solve()

    # MS-Freeceel board No. 24.
    ret = fcs.solve_board(
"""4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD TH TC TD 8D
7H JS KH TS KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
""")

    # TEST
    ok (fcs.get_num_times() == 137, "Get num times is OK.");

    # TEST
    ok (fcs.get_num_states_in_col() == 191, "Num-states-in-collection is OK.");

    fcs.recycle();

    # TEST
    ok (fcs.get_num_times() == 0, "Get num times immediately after recycle.");

    # TEST
    ok (fcs.get_num_states_in_col() == 0, "Num-states-in-collection immediately after recycle.");

    # MS-Freeceel board No. 24.
    ret = fcs.solve_board(
"""4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD TH TC TD 8D
7H JS KH TS KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
""")

    # TEST
    ok (fcs.get_num_times() == 137, "Get num times after recycle.");

    # TEST
    ok (fcs.get_num_states_in_col() == 191, "Num-states-in-collection after recycle.");

def test_num_states_in_collection_after_unsolved():
    testname = "NumStatesInCol-After-unsolved"

    fcs = FC_Solve()

    # TEST*$input_cmd_line
    fcs.input_cmd_line("bakers_game", ['-g', 'bakers_game']);

    # MS-Freeceel board No. 10
    ret = fcs.solve_board(
"""5S KD JC TS 9D KH 8D
5H 2S 9H 7H TD AD 6D
6H QD 6C TC AH 8S TH
6S 2D 7C QC QS 7D 3H
5D AS 7S KC 3D AC
4D 9C QH 4H 4C 5C
2H 3S 8H 9S JS 4S
JH JD 3C KS 2C 8C
""")

    # TEST
    ok (fcs.get_num_times() == 3436, "Get num times is OK.");

    # TEST
    ok (fcs.get_num_states_in_col() == 3436, "Num-states-in-collection is OK.");

def main():

    test_resume_solution()

    test_num_states_in_collection_after_recycle()

    test_num_states_in_collection_after_unsolved()
#----------------------------------------------------------------------

if __name__ == "__main__":
    main()

