python2 /home/shlomif/mainline/metrix++.py collect --db-file=/home/shlomif/mpp.db
python2 /home/shlomif/mainline/metrix++.py export --db-file=/home/shlomif/mpp.db | perl -lnaF/,/ -E 'if ($F[0] eq "./lib.c"){say $F[-1]-$F[-2], "\t", $F[1];}' | sort -n
