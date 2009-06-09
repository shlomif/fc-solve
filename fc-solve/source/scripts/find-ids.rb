col = Hash.new
ARGV.each do |fn|
    $_ = IO.read(fn)
    gsub(/\/\*.*?\*\//m, ""); 
    gsub(/(?:"([^\\]|\\.)*?")|(?:'[^'']+?')/m, " ");
    $_.scan(/([a-zA-Z_]\w+)/).each{ |s| id = s[0]; col[id] ||= 0; col[id] += 1}
end

puts col.keys.sort { |x,y| col[x] <=> col[y] || (x <=> y)}.map{ 
    |k| sprintf("%10d  %s", col[k], k)
}
