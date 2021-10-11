#! /bin/bash
#
# spell.bash
# Copyright (C) 2018 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.
#

spelling_errors_filename_1='foo.txt'
spelling_errors_filename_2='y.txt'

runspell()
{
    (
        set -e -x
        gmake
        perl -I ./lib -M"Shlomif::Spelling::Iface_Local" -E 'Shlomif::Spelling::Iface_Local->new->run()' | perl -lp -E 's/[ \t]+$//' > "$spelling_errors_filename_1"
        perl bin/extract-spelling-errors.pl > "$spelling_errors_filename_2"
    )
}

editspell()
{
    nvim-qt -- -o lib/hunspell/whitelist1.txt "$spelling_errors_filename_1" "$spelling_errors_filename_2"
}
