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

( a="$(pwd)"; cmdir="$a/cmake"; mkdir -p "$cmdir" ; mkdir B2 && cd B2 && hg clone https://bitbucket.org/shlomif/shlomif-cmake-modules && cd shlomif-cmake-modules/shlomif-cmake-modules && cp -f "$(pwd)"/Shlomif_Common.cmake "$cmdir" )

( mkdir b && cd b && cmake .. && make && sudo make install ) || exit -1
) || exit -1
# Cleanup to avoid trailing space problems.
rm -fr "$clone_dir"
