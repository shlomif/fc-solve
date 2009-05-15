Dir["*.c"].each do |fn|
    File.open(fn, "r") do |file|
        ids_by_indents = Array.new
        active_ids = Hash.new
        line_num = 1
        while (l = file.gets)
            if mo = /^(\s+)fcs_cards_column_t\s+(\w+(?:\s*,\s*\w+)*)\s*;\s*\z/.match(l)
                indent, identifiers = mo[1], mo[2]
                identifiers.split(/\s*,\s*/).each do |id|
                    ids_by_indents[indent.length()] ||= Hash.new
                    ids_by_indents[indent.length()][id] = 1
                    if (active_ids.has_key?(id)) then
                        raise "ID #{id} already defined in #{fn}:#{active_ids[id]['def']} now at line #{line_num}"
                    end
                    active_ids[id] = { "def" => line_num, "occur" => [] }
                end
            elsif l =~ /^(\s*)\}\s*\z/
                indent = $1.length
                while ((ids_by_indents.length-1) > indent) do
                    (ids_by_indents.pop || {}).keys.sort.each do |id|
                        rec = active_ids.delete(id)
                        puts "#{fn}:#{rec["def"]}:#{id} defined"
                        rec["occur"].each { |occur|
                            puts "#{fn}:#{occur["line_num"]}:#{occur["string"]}"
                        }
                    end
                end
            else
                l.scan(/\w+/).each do |id| 
                    if (r = active_ids[id]) then 
                        r["occur"].push({"line_num" => line_num, "string" => l})
                    end
                end
            end
            line_num += 1
        end
    end
end
