metrix=~/Download/unpack/prog/mainline/metrix++.py
pym="python2 $metrix"
db=~/mpp.db
$pym collect --db-file="$db"
$pym export --db-file="$db" | perl -lnaF/,/ -E 'if ($F[0] and $F[1] and $F[1] ne "__global__"){say $F[-1]-$F[-2], "\t", $F[1], "\t", $F[0];}' | sort -n
