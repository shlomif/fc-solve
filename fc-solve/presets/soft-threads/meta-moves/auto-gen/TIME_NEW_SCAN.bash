#!/bin/bash
# This time-scan.pl invocation creates a scan that looks good, but inflicts a
# a much more decreased speed after it is timed and processed.
perl time-scan.pl --method random-dfs -to '[01]=rand()[23456789]=rand()' -dto '13,[0123456]=asw(1)' -sp r:tf
