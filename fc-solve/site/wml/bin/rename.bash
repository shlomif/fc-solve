#! /bin/bash
#
# rename.bash
# Copyright (C) 2019 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.
#
orig="$1"
wfn="${orig%%.wml}.jinja"
git mv "$orig" "$wfn"
perl -i -0777 -p bin/wml2jinja.pl "$wfn"
