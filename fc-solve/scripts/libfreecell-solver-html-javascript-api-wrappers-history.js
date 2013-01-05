var obj = Module.ccall('freecell_solver_user_alloc', 'number', [], []);
// var ret = Module.ccall('freecell_solver_user_cmd_line_read_cmd_line_preset', 'number', ['number', 'string', 'number', 'number', 'number', 'string'], [obj, 'as', 0, 0, 0, null]);
var err_code = Module.ccall('freecell_solver_user_solve_board', 'number', ['number', 'string'], [obj, "4C 2C 9C 8C QS 4S 2H\n5H QH 3C AC 3H 4H QD\nQC 9S 6H 9H 3S KS 3D\n5D 2S JC 5C JH 6D AS\n2D KD 10H 10C 10D 8D\n7H JS KH 10S KC 7C\nAH 5S 6S AD 8H JD\n7S 6C 7D 4D 8S 9D\n"]);
var move_buffer = Module.ccall('malloc', 'number', ['number'], [128]);
var move_ret = Module.ccall('freecell_solver_user_get_next_move', 'number', ['number', 'number'], [obj, move_buffer]);
var state_as_string = Module.ccall('freecell_solver_user_current_state_as_string', 'string', ['number', 'number', 'number', 'number'], [obj, 1, 0, 1]);
var move_as_string = Module.ccall('freecell_solver_user_move_ptr_to_string_w_state', 'string', ['number', 'number', 'number'], [obj, move_buffer, 0]);
