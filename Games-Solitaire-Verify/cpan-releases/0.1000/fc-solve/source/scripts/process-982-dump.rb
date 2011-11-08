File.open("982.dump", "r") do |file|
    # Position of the first line like that.
    file.seek(834785485)
    while (l = file.gets)
        if (l == "Depth: 68\n") then
            while (not l =~ /^Foundations: /)
                l = file.gets
            end
            board = l + ""
            while (l =~ /\S/)
                l = file.gets
                board += l
            end
            board.gsub!(/^: /, '')
            puts "Processing: <<<\n#{board}\n>>>\n"
            IO.popen("./fc-solve --freecells-num 2 -to 01ABCDE -sp r:tf -p -t -sam -", "r+") { |io|
            # IO.popen("cat -", "r+") { |io|
                io.print(board)
                io.close_write()
                while (fcs_line = io.gets)
                    print fcs_line
                end
            }

        end
    end
end
