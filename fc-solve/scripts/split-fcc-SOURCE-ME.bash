rel2abs()
{
    perl -MFile::Spec -e 'print File::Spec->rel2abs(shift(@ARGV))' "$1"
}

if test -z "$FCS_PATH"
then
    export FCS_PATH="$(rel2abs ".")"
fi

if test -z "$FCS_SRC_PATH"
then
    export FCS_SRC_PATH="$(rel2abs "../source")"
fi

export DEAL_IDX=384243
PATH="$FCS_PATH:$PATH"

export SCRIPTS_DIR="$(rel2abs "$FCS_SRC_PATH/../scripts/")"
export PERL5LIB="$PERL5LIB:$(rel2abs "$FCS_SRC_PATH/t/lib"):$SCRIPTS_DIR"
export BOARD_FN="$(rel2abs "$FCS_PATH/$DEAL_IDX.board")"
export INIT_BOARD_FN="$(rel2abs "$FCS_PATH/SPLIT_FCC/${DEAL_IDX}.init.board")"
export INIT_INPUT_TXTISH="$(rel2abs "$FCS_PATH/SPLIT_FCC/${DEAL_IDX}.input.txtish")"

# mkdir -p SPLIT_FCC/by-depth/{0..104}/active
run()
{
    local id
    id="$1"
    shift

    id="$(echo "$id" | perl -lane 'print $1 if m{([^/]+)/*\z}')"

    perl -MSplitFcc -e 'SplitFcc->new->driver_run()' -- --fingerprint="$id"
}

_is_int()
{
    local n="$1"
    shift
    echo "$n" | grep -E '^[0-9]+$'
}

depth_run()
{
    depth="$1"
    shift
    if ! _is_int "$depth"
    then
        echo "Usage : depth_run [integer]" 1>&2
        return
    fi
    (for i in SPLIT_FCC/by-depth/"$depth"/active/* ; do
        echo "===[[[ Running $i ]]]==="
        if ! test -d "$i"
        then
            echo "Incorrect glob"
            exit -1
        fi
        echo
        run "$i" || exit -1
    done)
}

____debug_run()
{
    local id
    id="$1"
    shift

    id="$(echo "$id" | perl -lane 'print $1 if m{([^/]+)/*\z}')"

    perl -d -MSplitFcc -e 'SplitFcc->new->driver_run()' -- --fingerprint="$id"
}

startup()
{
    (
    set -e
    bash ../scripts/sample-split_fcc_fc_solver-invocation-1.bash
    cp -f "$INIT_BOARD_FN" "$BOARD_FN"
    mkdir -p SPLIT_FCC/by-depth/0/active/AAAAAAAAAAAAAAAAAA==
    cp "$INIT_INPUT_TXTISH" SPLIT_FCC/by-depth/0/active/AAAAAAAAAAAAAAAAAA==/input.txtish.1
    depth_run 0
    )
}

bootstrap()
{
    startup
}
