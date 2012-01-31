#!/usr/bin/ruby

# Copyright 2012 by Shlomi Fish
# 
# This program is distributed under the MIT (X11) License:
# L<http://www.opensource.org/licenses/mit-license.php>
# 
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use,
# copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following
# conditions:
# 
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.

# This script should be run on dumps like these:
# 
# http://fc-solve.shlomifish.org/downloads/fc-solve/data/2fc-solvability/dbm-fc-solver-2fc-results.tar.xz
#
# It produces a report like this:
#
# http://tech.groups.yahoo.com/group/fc-solve-discuss/message/1097

daj_results = <<'EOF';
Of the 33 you have listed, he [= Danny A. Jones] reported:
8 impossible (891, 7214, 14445, 15957, 16462, 17184, 19678, 27799),
11 intractable (982, 3129, 11266, 12038, 12064, 14790, 15804, 20792, 21779, 26124, 
29577),
14 solved (5435, 6090, 7728, 9034, 13659, 13705, 14262, 16322, 17684, 17760, 17880, 
18446, 19671, 28188)
EOF

daj = Hash.new
daj_results.scan(
    /^\d+\s+(impossible|intractable|solved)\s+\(([\d,\s\n]+)\)/ms) do
    |which, list|
    list.scan(/(\d+)/) { |i_s| daj[i_s[0].to_i] = which; }
end

deals = []
Dir.glob("*.dump").each do |fn| 
    if not fn =~ /^(\d+)\.dump$/ then 
        raise "#{fn} is wrong." 
    end
    deals << $1.to_i
end

deals.sort! { |a,b| a <=> b }

# puts(deals.join(","))

# $d = daj

deals.each do |i|
    contents = IO.read("#{i}.dump")
    timestamps = contents.scan(/^Reached (\d+) ; States-in-collection: (\d+) ; Time: (\d+\.\d+)$/ms)
    verdict = "intractable"
    depth = 0
    if (contents =~ /^Could not solve successfully/ms)
        verdict = "impossible"
    elsif (contents =~ /^Success\!/ms)
        verdict = "solved"
        depth = contents.scan(/^Foundations: /ms).length
    end
    puts "#{i}: #{verdict.capitalize} (DAJ: #{daj[i].capitalize}) ; Reached: ~#{timestamps[-1][0]}/#{timestamps[-1][1]} \\\n\tTime: #{timestamps[-1][2].to_f - timestamps[0][2].to_f}"
end
