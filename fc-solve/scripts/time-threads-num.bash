#!/usr/bin/env bash

run_serial=false
PROG="${PROG:-./freecell-solver-multi-thread-solve}"
# max_board="32000"
# NOTE: on the newer computers, we default to benchmarking the first 320,000
# deals rather than only the first 32,000 . This is done because otherwise the
# timing is very short .
#
# "the difference between the two is most probably below the noise level"
# https://www.shlomifish.org/humour/fortunes/show.cgi?id=sharp-pypy--got-better-things-to-do
max_board="320000"
suf='t'

while getopts "spt:" flag
do
    # Run the serial scan first.
    if [ "$flag" = "s" ]
    then
        run_serial=true
    # Run the multi-process version instead of the multi-threaded version.
    elif [ "$flag" = "p" ]
    then
        PROG="./freecell-solver-fork-solve"
        suf='p'
    # Maximal board - mnemonic - "to".
    elif [ "$flag" = "t" ]
    then
        max_board="$OPTARG"
    fi
done

while test "$OPTIND" -ne 1
do
    shift
    let --OPTIND
done

min="${1:-1}"
shift

max="${1:-6}"
shift

OUT_DIR="${OUT_DIR:-.}"
ARGS="${ARGS:--l gi}"

# strip * > /dev/null 2>&1

dumps_dir="$OUT_DIR/$(date +"DUMPS-%s")-$suf"
mkdir -p "$dumps_dir"

p_dir="__p"
if ! test -e "$p_dir"
then
    cp -R "$(dirname "$0")"/../source/Presets "$p_dir"
    cp "Presets/presetrc" "$p_dir/"

    P="$(cd "$p_dir"/presets && pwd)" \
        perl -lpi -e 's!\A(dir=).*\z!$1$ENV{P}/!ms' ./"$p_dir"/presetrc
fi

export FREECELL_SOLVER_PRESETRC="$(ls "$(pwd)"/"$p_dir"/presetrc)"

if test "$run_serial" = "true"
then
    echo "Testing Serial Run"
    # For letting the screen update.
    sleep 1
    ./freecell-solver-range-parallel-solve 1 "$max_board" 4000 $ARGS > "$dumps_dir"/dump
fi

for (( num=min ; num<=max ; ++num ))
do
    echo "Testing $num"
    # For letting the screen update.
    sleep 1
    $PROG 1 "$max_board" 4000 \
        --num-workers "$num" \
        $ARGS > "$(printf "%s/dump%.3i" "$dumps_dir" "$num")"
done
