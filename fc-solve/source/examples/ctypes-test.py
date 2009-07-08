from ctypes import *

fcs = CDLL("libfreecell-solver.so")
libc = CDLL("libc.so.6")

user = fcs.freecell_solver_user_alloc()

print user

board = """4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD 10H 10C 10D 8D
7H JS KH 10S KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
"""

print "Hello == ", fcs.freecell_solver_user_solve_board(user, 
    c_char_p(
       board
    )
)

print "Foo"

fcs.freecell_solver_user_current_state_as_string.restype = c_void_p

p = create_string_buffer(100)

class MOVE(Structure):
    _fields_ = [("c", c_byte * 4)]

move = MOVE()
while (fcs.freecell_solver_user_get_next_move(user, pointer(move)) == 0):
    # print ("%d,%d,%d,%d") % (ord(p[0]), ord(p[1]), ord(p[2]), ord(p[3]))
    as_string = fcs.freecell_solver_user_move_to_string_w_state(user, move, 0)
    print string_at(as_string), "\n\n"
    libc.free(as_string)
    as_string = fcs.freecell_solver_user_current_state_as_string(
            user, 1, 0, 1
            )

    print "%s\n\n---------------------\n\n" % (string_at(as_string))
    libc.free(as_string)

