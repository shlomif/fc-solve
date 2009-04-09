#!/usr/bin/ruby

Dir["*.[ch]"].each do |fn|
    outfn = "#{fn}.new"
    outfile = File.open(outfn, "w")
    File.open(fn,"r") do |file|
        while l = file.gets
            outfile.puts(
                l.gsub(/\b(freecell_solver_[a-zA-Z0-9_]+)\b/) { |s|
                    if (s !~ /^freecell_solver_user/) then
                        s.sub!(/^freecell_solver/, "fc_solve")
                    end
                    s
                }
            );
        end
    end
    outfile.close
    File.rename(outfn, fn)
end
