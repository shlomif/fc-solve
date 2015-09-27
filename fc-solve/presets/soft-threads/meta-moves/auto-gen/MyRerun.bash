#!/bin/bash
perl -lnE '@F=split/\t/;print $F[1] if $F[1] =~ s/ -dto / -dto2 /g' < scans.txt | \
    (while read l; do
        echo "<<<<RETIMING $l>>>>>"
        perl time-scan.pl $l
    done) | \
    timestamper | tee ~/fcs-retime.log.txt
