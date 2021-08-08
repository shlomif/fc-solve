./gen-helpers && make fastrender && make -j 17
a="dest/code-of-conduct/index.html"
b="/home/shlomif/Backup/Arcs/fcs-site--post-dest/post-incs/${a##dest/}"
adest=have.xhtml
bdest=want.xhtml
tidy < $a > $adest
tidy < $b > $bdest
# gvimdiff $adest $bdest +colorscheme" apprentice" +"exe \"normal \\<c-w>J\""
gvimdiff $adest $bdest +"exe \"normal \\<c-w>J\""
