mypath="$HOME/progs/freecell/new_ss/freecell-solver-2.8.1/"
cat $mypath/after.dump.txt | 
    grep '^\(\[\|This game is solveable\)' | 
    perl -e 'while (<>) { if (/\[(\d+)\]/) { $n = $1; } elsif (/solve/) { print "$n\n" }}' |
    (while read T ; do
        echo [$T]
        make_pysol_freecell_board.py $T simple_simon | 
            "$mypath/fc-solve" -g simple_simon -to abcdefghi -p -t -sam |
            perl simple_simon_verify.pl -
    done)
    
