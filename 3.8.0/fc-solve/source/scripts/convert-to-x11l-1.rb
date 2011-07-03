require "FcsLicense.rb"

Dir["*.[ch]"].each do |fn|
    if fn !~ /config|lookup2|jhjtypes|pqueue|prefix/ then
        change_c_file(fn)
    end
end
