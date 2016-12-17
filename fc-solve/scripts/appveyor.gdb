set breakpoint pending on
b freecell_solver_user_alloc
r -m trace --trace "..\\..\\source\\t\t\\by-depth-tests-order.py"
b 292
c
p "apple bloom ret="
p ret
p "prettyfour soft_thread="
p ret->soft_thread
p &(ret->soft_thread)
watch *(long long *)$
b freecell_solver_user_free
c
bt full
q
