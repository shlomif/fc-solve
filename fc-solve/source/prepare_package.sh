#!/bin/sh

RPM=false
AUTOCONF=true

for OPT do
    if [ "$OPT" == "--rpm" ] ; then
        RPM=true
    elif [ "$OPT" == "--noac" ] ; then
        AUTOCONF=false
    else
        echo "Unknown option \"$OPT\"!"
        exit -1
    fi
done


if $AUTOCONF ; then
    automake
    autoconf
    (cd board_gen && automake && autoconf)
fi

./configure && make dist

if $RPM ; then
    rpm -tb freecell-solver-`cat ver.txt`.tar.gz
fi


