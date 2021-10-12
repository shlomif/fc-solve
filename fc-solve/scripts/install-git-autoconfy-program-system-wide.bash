#!/bin/bash

set -x

vcs_tool="$1"
shift
d="$1"
shift
url="$1"
shift
branch="$1"
shift

if test -n "$branch"
then
    branch="-b $branch"
fi

clone_dir='mygit'
"$vcs_tool" clone $branch "$url" "$clone_dir" || exit -1
(cd "$clone_dir/$d" &&

( ./configure && make && sudo make install ) || exit -1
) || exit -1
# Cleanup to avoid trailing space problems.
rm -fr "$clone_dir"
