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
set breakpoint pending on
# b fc_solve_user_INTERNAL_find_fcc_start_points
b perform_FCC_brfs
set args scripts/Test-Find-FCC-Start-Points.pl 24m2.board
r
b 239
set $X = 0
set $FILE = fopen("log.txt", "wt")
while $X < 10
    c
    call fprintf($FILE, "<<<\n%s\n>>>\n", fc_solve_state_as_string (&state, 2, 8, 1, 1, 0, 1))
    set $X = $X + 1
end
call fclose($FILE)
q
