#!/bin/bash

start_rev="2910"
end_rev="2940"

checkout_dir="source"

export PATH="/home/shlomi/apps/prog/gcc-4.5.0/bin/:$PATH"

if ! test -d "$checout_dir" ; then
    svn co 'http://svn.berlios.de/svnroot/repos/fc-solve/branches/add-depth-dependent-moves-order/fc-solve/source/' "$checkout_dir"
fi

cd "$checkout_dir"
for rev in $(seq "$start_rev" "$end_rev") ; do
    echo "Rev == $rev"
    echo
    if test -e CMakeLists.txt.orig ; then
        mv -f CMakeLists.txt.orig CMakeLists.txt
    fi
    svn up -r "$rev" -q
    cp -f CMakeLists.txt CMakeLists.txt.orig 
    perl -ni.bak -le 'print qq{LIST(APPEND COMPILER_FLAGS_TO_CHECK "-DFCS_COMPILE_DEBUG_FUNCTIONS=1")\n} if m{SET \(IDX 1\)}; print;' CMakeLists.txt 
    ./Tatzer -l p4b --prefix="$HOME/fcs"
    make
    make install

    echo "Running Timing $rev"
    echo

    ARGS="--worker-step 16 -l tic" time bash scripts/time-threads-num.bash 2 2
    
    echo "Times of $rev"

    perl scripts/time-fcs.pl *DUMPS*/*

    echo "Running Second Timing $rev"
    echo

    ARGS="--worker-step 16 -l tic" time bash scripts/time-threads-num.bash 2 2
    
    echo "Second Times of $rev"

    perl scripts/time-fcs.pl *DUMPS*/*

done
