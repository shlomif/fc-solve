col = Hash.new
ignore_list = %w{
    DB DBT DB_BTREE FCS_MOVE_TYPE_NULL FCS_MOVE_TYPE_SEPARATOR
    FCS_STATE_OPTIMIZED FCS_STATE_ORIGINAL_STATE_IS_NOT_SOLVEABLE
    FREECELL_SOLVER_PREFIX Judy O_CREAT O_RDWR PACKAGE PACKAGE_BUGREPORT
    PACKAGE_NAME PACKAGE_STRING PACKAGE_TARNAME PACKAGE_VERSION
    PTHREAD_MUTEX_INITIALIZER SEEK_SET SIGUSR1 SIGUSR2
    __attribute__ arg_index atof bsearch
    close db_open do fc_solve_hash_c_dummy
    fc_solve_simple_simon_nothing fcntl
    fcs_libavl2_stacks_tree_create
    fcs_libavl2_stacks_tree_destroy
    fcs_libavl2_stacks_tree_insert
    fcs_libavl2_stacks_tree_table_t
    fcs_libavl2_states_tree_create
    fcs_libavl2_states_tree_destroy
    fcs_libavl2_states_tree_insert
    fcs_libavl2_states_tree_table_t
    fgets
    get
    gint
    glib
    gypsy_talon
    gypsy_talon_len
    lp_instance
    no
    pthread
    pthread_create
    pthread_join
    pthread_t
    put
    qsort
    redblack
    short
    stat
    stdin
    strcat
    strchr
    strnicmp
    union
    unistd
    unused
    va_list
    va_start
    vsprintf
    Column
    END_OF_LOOP
    FCS_A_STAR_WEIGHT_CARDS_OUT
    FCS_A_STAR_WEIGHT_CARDS_UNDER_SEQUENCES
    FCS_A_STAR_WEIGHT_DEPTH
    FCS_A_STAR_WEIGHT_MAX_SEQUENCE_MOVE
    FCS_A_STAR_WEIGHT_SEQS_OVER_RENEGADE_CARDS
    FCS_CMD_LINE_USER
    FCS_COMPILE_PRELUDE_NO_AT_SIGN
    FCS_COMPILE_PRELUDE_OK
    FCS_COMPILE_PRELUDE_UNKNOWN_SCAN_ID
    FCS_FOUNDATIONS_OFFSET
    FCS_INLINE_KEYWORD
    FCS_METHOD_HARD_DFS
    FCS_METHOD_NONE
    FCS_MOVE_NUM_CARDS_FLIPPED
    FCS_MOVE_TYPE_DEAL_GYPSY_TALON
    FCS_MOVE_TYPE_KLONDIKE_FLIP_TALON
    FCS_MOVE_TYPE_KLONDIKE_REDEAL_TALON
    FCS_MOVE_TYPE_KLONDIKE_TALON_TO_STACK
    FCS_PRESET_CODE_DECKS_EXCEED_MAX
    FCS_PRESET_YUKON
    FCS_STATE_DOES_NOT_EXIST
    FCS_STATE_EXCEEDS_MAX_DEPTH

}

ignore = Hash.new
for s in ignore_list do
    ignore[s] = 1
end

ARGV.each do |fn|
    if fn == "test_inline.c"
        next
    end
    $_ = IO.read(fn)
    gsub(/\/\*.*?\*\//m, ""); 
    gsub(/(?:"([^\\]|\\.)*?")|(?:'[^'']+?')/m, " ");
    $_.scan(/\b([a-zA-Z_]\w+)/).each{ |s| id = s[0]; col[id] ||= 0; col[id] += 1}
end

puts col.keys.select { |id| !ignore[id] }.sort_by { |x| [col[x],x] }.map{ 
    |k| sprintf("%10d  %s", col[k], k)
}
