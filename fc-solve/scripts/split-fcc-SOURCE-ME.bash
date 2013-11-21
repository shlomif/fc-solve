rel2abs()
{
    perl -MFile::Spec -e 'print File::Spec->rel2abs(shift(@ARGV))' "$1"
}

export FCS_PATH="$(rel2abs ".")" FCS_SRC_PATH="$(rel2abs "..")"
export SCRIPTS_DIR="$(rel2abs "../../scripts/")"
export PERL5LIB="$PERL5LIB:$(rel2abs "../t/t/lib"):$SCRIPTS_DIR"
export BOARD_FN="$(rel2abs "384243.board")"

run()
{
    local id
    id="$1"
    shift

    id="$(echo "$id" | perl -lape 'print $1 if m{([^/]+)/*\z}')"

    perl -MSplitFcc -e 'SplitFcc->new->driver_run()' -- --fingerprint="$id"
}

