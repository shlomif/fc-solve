ignore_list = IO.read("scripts/ids-whitelist.txt").split(/\s+/)

ignore = Hash.new
for s in ignore_list do
    ignore[s] = 1
end

col = Hash.new
ARGV.each do |fn|
    if fn == "test_inline.c"
        next
    end
    File.open(fn, "r") do |file|
        line_num = 1
        while (l = file.gets)
            l.gsub!(/\/\*.*?\*\//m, ""); 
            l.gsub!(/(?:"([^\\]|\\.)*?")|(?:'[^'']+?')/m, " ");
            if (m = /\A(.*?)((?:\/\*)|\z)/m.match(l))
                before = m[1]
                open_comment = m[2]
                before.scan(/\b([a-zA-Z_]\w+)/).each{
                    |s| id = s[0]; col[id] ||= Array.new; col[id] << [fn, line_num]
                }
                if (open_comment == "/*")
                    found_line = false
                    while (l = file.gets)
                        line_num += 1
                        if (l.gsub!(/\A.*?\*\//, ""))
                            found_line = true
                            break
                        end
                    end
                    if (found_line)
                        redo
                    end
                else
                    line_num += 1                
                end
            end
        end
    end
end

puts col.keys.select { |id| !ignore[id] }. \
    sort_by { |x| [col[x].length,x] }. \
    map{ |x| col[x].map { |arr| arr[0] + ":" + arr[1].to_s + ":" + x }}. \
    flatten
