seed=100
max_seed=0
max_num_solved=0

while true ; do
    echo "seed is $seed"
    num_solved="`bash Analyze.sh --method random-dfs -seed $seed`"
    if expr $num_solved \> $max_num_solved > /dev/null ; then
        max_seed=$seed
        max_num_solved=$num_solved
    fi
    echo "$max_num_solved were solved with $max_seed"

    seed=`expr $seed + 1`
done
    
