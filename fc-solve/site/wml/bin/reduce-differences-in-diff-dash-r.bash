#! /bin/bash
#
# reduce-differences-in-diff-dash-r.bash
# Copyright (C) 2019 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.
#
src="$1"
shift
dest="$1"
shift
rm -fr "$dest"
cp -a "$src" "$dest"
# find "$dest" -type f | grep -E '(\.epub$)' | xargs rm -f
