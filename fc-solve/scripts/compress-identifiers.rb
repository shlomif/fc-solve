#!/usr/bin/ruby

# What this script does is shorten the fc_solve_.* identifiers in the
# *.c and *.h files to be very short (while still starting with "fc_solve_").
# It was meant as an experiment to see how well it reduces the size of the
# generated libraries considerably.
#
# The savings are:
# Before: 108,028 bytes.
# After: 105,820 bytes.
#
# A bad side-effect of this script is that it reallly obfuscates the code.

$map = Hash.new
$next_num = 0
$chars = (["_"] + ('a'..'z').to_a + ('A'..'Z').to_a + ('0'..'9').to_a)

def gen_ident(num)
    if (num == 0)
        return ""
    else
        return $chars[num % $chars.length()] \
            + gen_ident(num / $chars.length())
    end
end

def calc_first_short(id)
    ret = "fc_solve_" + gen_ident($next_num)
    $next_num += 1
    return ret
end

def calc_short(id)
    if (! $map.has_key?(id))
        $map[id] = calc_first_short(id)
    end
    return $map[id]
end

Dir["*.[ch]"].each do |fn|
    outfn = "#{fn}.new"
    outfh = File.open(outfn, "w")
    File.open(fn, "r") do |file|
        while l = file.gets
            outfh.puts(
                l.gsub(/\b(fc_solve[a-zA-Z0-9_]+)\b/) { |s|
                    calc_short(s)
                }
            )
        end
    end
    outfh.close
    File.rename(outfn, fn)
end
