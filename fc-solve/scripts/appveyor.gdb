set breakpoint pending on
b freecell_solver_user_alloc
r -m trace --trace "..\\..\\source\\t\t\\by-depth-tests-order.py"
b 292
c
call printf("apple bloom ret=<%p> soft_thread=<%p>\n", ret, ret->soft_thread)
b freecell_solver_user_free
c
bt full
q
