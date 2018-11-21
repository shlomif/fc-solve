#!/bin/bash
perl ../scripts/time-fcs.pl DUMPS-*/* | perl -lapE 's#:#\t#' |
    sort -n -k 2 | head -1 |
    {
        read fn t
        git show | head -1
        echo "(commit $(git show | grep -E "^Date:" | head -1) )"
        echo "${t}s"
        echo
        echo "    Highlight:"
        echo
        echo '<DUMP>'
        cat "$fn"
        echo '</DUMP>'
        echo
        echo "    ( $fn )"
        echo
    }
