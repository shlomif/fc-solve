#!/bin/bash

test_num=1
if [ "$FCS" == "" ] ; then
    FCS="fc-solve"
fi

for board_num in 1 9 24 100 ; do
    if [ ! -e $board_num.board.txt ] ; then
        pi-make-microsoft-freecell-board $board_num > $board_num.board.txt
    fi
done

for dir in good new ; do    
    if [ ! -d $dir ] ; then
        mkdir $dir 
    fi
done

mytest()
{
    local which_boards="$1"
    shift
    for board_num in $which_boards ; do
        local fn="$test_num-$board_num.txt"
        echo "Test ($test_num): Testing #$board_num : $*"
        local cmd_line="$FCS -p -t -s -i $* $board_num.board.txt"
        if [ ! -e good/$fn ] ; then
             $cmd_line > dump.txt
             cat dump.txt | md5sum > good/$fn
        fi
        $cmd_line > dump.txt
        cat dump.txt | md5sum > new/$fn
        if cmp -s new/$fn good/$fn ; then
            echo "ok"
        else
            echo "not ok"
            exit -1
        fi
    done
    let test_num++
}

test_all_boards()
{
    mytest "1 9 24 100" $*
}

mytest "1 24"
mytest "24" --method random-dfs
mytest "1 9 24" --method a-star
mytest "1 9 24" --method random-dfs -to "[012345]"
mytest "1 9 24 100" --method soft-dfs -step 100 -nst --method a-star -step 100

# Presets
presets_dir=../../source/Presets/presets/
cool_jives="--read-from-file 4,$presets_dir/cool-jives.sh"
fools_gold="--read-from-file 4,$presets_dir/fools-gold.sh"

mytest "1 9 24 100" $cool_jives
mytest "1 9 24 100" $fools_gold
mytest "1 9 24 100" $cool_jives -ni $fools_gold

# Testing the A* weights
mytest "1 24 100" --method a-star -asw "0.2,0.3,0.5,0,0"
mytest "1 24 100" --method a-star -asw "0.5,0,0.3,0,0"

