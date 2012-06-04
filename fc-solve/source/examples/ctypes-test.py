import sys
from ctypes import *

def main(argv):
    fcs = CDLL("libfreecell-solver.so")
    libc = CDLL("libc.so.6")

    user = fcs.freecell_solver_user_alloc()

    board = file(argv.pop(0)).read()

    fcs.freecell_solver_user_solve_board(user,
        c_char_p(
           board
        )
    )

    fcs.freecell_solver_user_current_state_as_string.restype = c_void_p

    class MOVE(Structure):
        _fields_ = [("c", c_byte * 4)]

    move = MOVE()

    def print_state(user=user):
        as_string = fcs.freecell_solver_user_current_state_as_string(
            user, 1, 0, 1
            )

        print "%s\n\n====================\n" % (string_at(as_string)),

        libc.free(as_string)

    print "-=-=-=-=-=-=-=-=-=-=-=-\n\n",

    print_state()

    while (fcs.freecell_solver_user_get_next_move(user, pointer(move)) == 0):
        as_string = fcs.freecell_solver_user_move_to_string_w_state(user, move, 0)
        print "\n%s\n\n" % (string_at(as_string)),
        libc.free(as_string)
        print_state()
    fcs.freecell_solver_user_free(user);

#----------------------------------------------------------------------

if __name__ == "__main__":
    main(sys.argv[1:])
