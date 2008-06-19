#!/bin/sh
ctags -f tags --recurse --totals \
    --exclude='**/blib/**' --exclude='**/t/lib/**' \
    --exclude='**/.svn/**' --exclude='*~' \
    --languages=Perl --langmap=Perl:+.t
