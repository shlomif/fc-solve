#!/bin/bash
set -e
set -x
mkdir -p ../B
cd ../B
# ../scripts/Tatzer -l extra_speed2 -l pdfs --nfc=2 --dbm-single-thread
../scripts/Tatzer -l pdfs --nfc=2 --dbm-single-thread
make
# Defines startup() and depth_run()
. ../scripts/split-fcc-SOURCE-ME.bash
startup
let max_depth=4*13*2

for ((d=1;d<=max_depth;d++))
do
    depth_run "$d"
done
