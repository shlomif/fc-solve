#!/bin/bash
perl -lne '/^\Q[[Num FCS Moves]]\E=(\d+)/ && ($n=$1);/^\Q[[Num FCPro Moves]]\E=(\d+)/ && print "$n\t$1";' "$@"
