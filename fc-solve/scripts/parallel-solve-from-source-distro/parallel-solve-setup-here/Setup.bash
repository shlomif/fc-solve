#!/bin/bash

rebuild()
{
    ./Tatzer -l x64bb --nobuild-docs --prefix="$HOME/fcs-bin" && \
        make
}

# Comment out to rebuild everything.
rebuild() { true ; }

set -x

script_dirname="$(dirname $BASH_SOURCE[-1])"
source "$script_dirname/fc_solve_common.bash"

rsync -a -v --progress --exclude='**/.svn/**' "$script_dirname/../.." "$FCS_PATH"

# if ! test -e "$FCS_PATH" ; then
#     svn export "$FCS_SOURCE_URL" "$FCS_PATH" && \
# fi

# true && # ./Tatzer -l x64bb --nobuild-docs --prefix="$HOME/fcs-bin" && \

cd "$FCS_PATH" && \
    rebuild && \
rm -f FAULT.txt sync.lock sync.txt summary.txt summary.lock \
    global-params.txt summary.stats.perl-storable &&
perl scripts/parallel-range-solver-total --mode setup &&
rm -f range-queue.sock
