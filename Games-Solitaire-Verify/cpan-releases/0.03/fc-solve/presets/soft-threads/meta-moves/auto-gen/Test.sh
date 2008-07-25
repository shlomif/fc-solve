#!/bin/bash
perl process.pl > output.got
if cmp -s output.good output.got ; then
    echo "Test Passed"
    exit 0
else
    echo 'ERROR!'
    exit 1
fi

