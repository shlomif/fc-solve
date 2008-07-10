perl analyze.pl | 
    grep '^[0-9]\+:' | 
    sed 's/:.*//' | 
    (while read T ; do 
        pi-make-microsoft-freecell-board $T | 
            fc-solve $@ -mi 2000 |
            tail -3 ; 
    done) | 
    grep -F "This game is solveable" | 
    wc -l
    
