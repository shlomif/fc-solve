#!/bin/bash
cd src && echo $(ls freecell-solver-* | 
    grep -v win32 | 
    sed 's/^freecell-solver-//' | 
    sed 's/\.tar\.gz$//' | 
    grep '[0-9]*\.[0-9]*[02468]\.' | 
    perl -e '@a=(map { chomp($_); [split(/\./, $_) ] } <>) ;print join("", (map { join(".", @$_) ."\n"; } (sort { ($a->[0] <=> $b->[0]) || ($a->[1] <=> $b->[1]) || ($a->[2] <=> $b->[2]) } @a)));' | 
    tail -1)
    
