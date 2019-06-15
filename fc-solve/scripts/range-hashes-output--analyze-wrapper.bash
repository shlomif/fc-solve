#! /bin/bash
#
# wrapper.bash
# Copyright (C) 2019 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.
#

set -e -x
for fn in *.txt
do
    rm -fr parts foo
    mkdir parts foo
    pypy3 /home/shlomif/conf/trunk/shlomif-settings/home-bin-executables/bin/split-file-into-buckets.py "$fn"
    for buc in parts/*
    do
        echo "fn = $fn buc = $buc"
        sort < "$buc" > foo/t.txt
        cat foo/t.txt | perl -lanE 'sub scan { my @k=keys%t;if (@k > 1){my%b;++$b{`pi-make-microsoft-freecell-board -t "$_"`} for @k; die "@k" if (grep {$_>1} values%b);};};my ($h, $d)=@F;if ($h ne $prev_h){scan();%t=();}$t{$d}=1; END{scan();}'
    done
done
