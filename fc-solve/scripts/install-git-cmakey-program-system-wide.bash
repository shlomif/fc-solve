#!/bin/bash

set -x

vcs_tool="$1"
shift
d="$1"
shift
url="$1"
shift

clone_dir='mygit'
"$vcs_tool" clone "$url" "$clone_dir"
(cd "$clone_dir/$d"

( mkdir b && cd b && cmake .. && make && sudo make install ) || exit -1
) || exit -1
# Cleanup to avoid trailing space problems.
rm -fr "$clone_dir"
