#!/bin/bash
export FCS_PATH="$HOME/fc-solve-bakers-game-range"
export FCS_MAX_DEAL_IDX="4000" FCS_PRESET_CONFIG="-g bakers_game"
export FCS_SOLVER_THEME_CONFIG="-g bakers_game"
export FCS_MIN_DEAL_IDX="1"
export FCS_DEALS_PER_BATCH="1"

if test -z "$FCS_SOURCE_URL"
then
    export FCS_SOURCE_URL="http://fc-solve.googlecode.com/svn/fc-solve/trunk/fc-solve/source"
fi

_run_mode()
{
    opt="$1"
    shift
    if test "$opt" != "--mode"
    then
        printf "wrong opt '%s'.\\n" "$opt"
        exit 1
    fi
    mode="$1"
    shift

    cd "$FCS_PATH" && perl "$FCS_SRC_PATH"/../scripts/parallel-solve-from-source-distro/parallel-range-solver-total --mode "$mode"
}
