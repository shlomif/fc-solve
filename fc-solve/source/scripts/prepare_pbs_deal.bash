#!/bin/bash

deal="$1"
shift
output="$1"
shift

target="dbm_fc_solver"

board="$deal.board"

offload_dir="/fastscratch/dhingralab/2fc-dbm_fc_solver-offload/deal-$deal"
final_out="/scratch/dhingralab/2fc-dbm_fc_solver-results/deal-$deal"

echo '#!/bin/bash' > "$output"
echo "#PBS -N 2_fc_Freecell_deal_$deal" >> "$output"
echo "#PBS -l walltime=120:00:00,mem=200gb" >> "$output"
echo "#PBS -m abe" >> "$output"
echo "#" >> "$output"
echo  >> "$output"
shar "$board" "$target" | grep -vE '^ *exit *0 *$' >> "$output"
echo >> "$output"
echo "mkdir -p '$offload_dir' '$final_out'" >> "$output"
echo "./$target --num-threads 16 --offload-dir-path '$offload_dir' '$board' | tee '$final_out/$deal.dump' '$final_out/$deal.dump.backup'" >> "$output"
