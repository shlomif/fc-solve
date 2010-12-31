#!/usr/bin/env python

import sys
sys.path.append("./t/lib");

from TAP.Simple import *
# TEST:source "$^CURRENT_DIRNAME/lib/FC_Solve/__init__.py"
from FC_Solve import FC_Solve

plan(1)

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
    
    while (ret == 5):
        limit += 10
        fcs.limit_iterations(limit)
        ret = fcs.resume_solution()
    
    # TEST
    is_ok (ret, 0, "State was successfully solved.")
   
def main():

    test_resume_solution()

#----------------------------------------------------------------------

if __name__ == "__main__":
    main()

