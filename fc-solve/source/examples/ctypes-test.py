#!/usr/bin/env python3
import sys
from ctypes import CDLL, Structure, c_byte, c_char_p, c_void_p, \
    pointer, string_at


def py_str_at(addr):
    return string_at(addr).decode('UTF-8')


def main(argv):
    fcs = CDLL("libfreecell-solver.so")
    libc = CDLL("libc.so.6")

    user = fcs.freecell_solver_user_alloc()
    board = open(argv.pop(0)).read()
    fcs.freecell_solver_user_solve_board(
        user,
        c_char_p(
           bytes(board, 'UTF-8')
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

        print("%s\n\n====================\n" % (py_str_at(as_string)))

        libc.free(as_string)

    print("-=-=-=-=-=-=-=-=-=-=-=-\n\n")
    print_state()

    while (fcs.freecell_solver_user_get_next_move(user, pointer(move)) == 0):
        as_string = fcs.freecell_solver_user_move_to_string_w_state(
            user, move, 0
        )
        print("\n%s\n\n" % (py_str_at(as_string)))
        libc.free(as_string)
        print_state()
    fcs.freecell_solver_user_free(user)


if __name__ == "__main__":
    main(sys.argv[1:])
