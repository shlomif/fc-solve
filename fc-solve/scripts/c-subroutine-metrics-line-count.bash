metrix=~/Download/unpack/prog/mainline/metrix++.py
pym="python2 $metrix"
db=~/mpp.db
$pym collect --db-file="$db"
$pym export --db-file="$db" | perl -lnaF/,/ -E 'if ($F[0] eq "./lib.c"){say $F[-1]-$F[-2], "\t", $F[1];}' | sort -n
