#!/bin/sh

automake
autoconf
(cd board_gen && automake && autoconf)
./configure && make dist

