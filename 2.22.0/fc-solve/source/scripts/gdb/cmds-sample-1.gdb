set args -l gi -s -i -p -t 1941.board
b main
r
# Breakpoint 1
b scans.c:1336 if instance->num_times == 3151
c
# Now we're at main.
call fc_solve_state_as_string(ptr_state_with_locations_key, ptr_state_with_locations_val, 4, 8, 1, 1, 0, 1)
call strstr($ , ": 9D 6S 4D AH\n")
while ($ == 0)
    c
    call fc_solve_state_as_string(ptr_state_with_locations_key, ptr_state_with_locations_val, 4, 8, 1, 1, 0, 1)
    call strstr($ , ": 9D 6S 4D AH\n")
end
