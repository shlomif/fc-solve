#!/usr/bin/ruby

(1 .. 1000).each do |board|
    system("pi-make-microsoft-freecell-board -t #{board} | perl fill_pos.pl > fill_pos.cpp");
    system("make");
    if (!system("./out-ms-moves 100000 -l gi")) then
        puts "out-ms-moves for board #{board} failed. Terminating."
        exit(1);
    end
end

