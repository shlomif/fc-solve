col = Hash.new
ignore_list = %w{
    DB DBT DB_BTREE FCS_MOVE_TYPE_NULL FCS_MOVE_TYPE_SEPARATOR
    FCS_STATE_OPTIMIZED FCS_STATE_ORIGINAL_STATE_IS_NOT_SOLVEABLE
    FREECELL_SOLVER_PREFIX Judy O_CREAT O_RDWR PACKAGE PACKAGE_BUGREPORT
    PACKAGE_NAME PACKAGE_STRING PACKAGE_TARNAME PACKAGE_VERSION
    PTHREAD_MUTEX_INITIALIZER SEEK_SET SIGUSR1 SIGUSR2
    __attribute__ arg_index atof bsearch
    close db_open do
}
ignore = Hash.new
for s in ignore_list do
    ignore[s] = 1
end

ARGV.each do |fn|
    $_ = IO.read(fn)
    gsub(/\/\*.*?\*\//m, ""); 
    gsub(/(?:"([^\\]|\\.)*?")|(?:'[^'']+?')/m, " ");
    $_.scan(/([a-zA-Z_]\w+)/).each{ |s| id = s[0]; col[id] ||= 0; col[id] += 1}
end

puts col.keys.select { |id| !ignore[id] }.sort_by { |x| [col[x],x] }.map{ 
    |k| sprintf("%10d  %s", col[k], k)
}
