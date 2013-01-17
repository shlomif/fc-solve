#!/bin/sh
ctags -f tags --recurse --totals \
    --exclude='**/blib/**' --exclude='blib/**' --exclude='**/t/lib/**' \
    --exclude='**/.svn/**' --exclude='*~' \
    --exclude='Games-Solitaire-Verify-*/**' \
    --languages=Perl --langmap=Perl:+.t
