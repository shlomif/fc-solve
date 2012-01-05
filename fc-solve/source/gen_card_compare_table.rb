values = ([-1] * 256)
(1 .. 13).each do |rank|
    (0 .. 3).each do |suit|
        values[(suit << 4) | rank] = ((rank << 2) | suit)
    end
end

File.open("card_compare_lookups.c", "w") do |fh|
    fh.puts("/* WARNING! This file is generated from gen_card_compare_table.rb */");
    fh.puts("signed char fc_solve_card_compare_lookup[256] = { " + 
            values.map{ |i| i.to_s() }.join(',') + "};")
end
