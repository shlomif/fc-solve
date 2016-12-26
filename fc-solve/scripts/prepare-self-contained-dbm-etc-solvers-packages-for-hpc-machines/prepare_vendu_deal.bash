#!/bin/bash

deal="$1"
shift
output="$1"
shift

target="dbm_fc_solver"

board="$deal.board"

offload_dir="$HOME/dbm_fc_solver-offload/deal-$deal"
final_out="$HOME/dbm_fc_solver-results/deal-$deal"
base_dir="`pwd`"

echo '#!/bin/bash' > "$output"
echo  >> "$output"
# shar "$board" "$target" | grep -vE '^ *exit *0 *$' >> "$output"
echo >> "$output"
echo "mkdir -p '$offload_dir' '$final_out'" >> "$output"
echo "$base_dir/$target --batch-size 20 --num-threads ${THREADS:-16} --offload-dir-path '$offload_dir' '$base_dir/$board' | perl -lnE 'use Time::HiRes qw/time/; printf qq#%.9f\\t%s\\n#, time(), \$_; STDOUT->flush;' | tee '$final_out/$deal.dump'" >> "$output"
