# This program should be called with a bunch of C (and possibly C++)
# source file paths on the command line and it emits the identifiers
# with their filenames and line numbers (useful for vim's quickfix command
# or equivalent - see :help :cbuf) in the files, sorted by:
#
# 1. Frequency - less frequent identifiers appear first.
# 2. Lexicographical order.
#
# This proved useful for refactoring / code cleanups

ignore_list = IO.read("scripts/ids-whitelist.txt").split(/\s+/)

ignore = Hash.new
for s in ignore_list do
    ignore[s] = 0
end

col = Hash.new
ARGV.each do |fn|
    if fn == "test_inline.c"
        next
    end
    File.open(fn, "r") do |file|
        begin
        line_num = 1
        while (l = file.gets)
            l.gsub!(/\/\*.*?\*\//m, "");
            l.gsub!(/(?:"([^\\]|\\.)*?")|(?:'[^'']+?')/m, " ");
            l.gsub!(/\/\/.*/m, "");
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
        rescue ArgumentError
            puts "#{fn} is malformed UTF8-wise"
            exit
        end
    end
end

puts col.keys.select { |id| ret = ignore.has_key?(id); if (ret) then ignore[id] += 1 ; end;  !ret }. \
    sort_by { |x| [col[x].length,x] }. \
    map{ |x| col[x].map { |arr| arr[0] + ":" + arr[1].to_s + ":" + x }}. \
    flatten

STDERR.puts(ignore.keys.select { |id| ignore[id] == 0 })
