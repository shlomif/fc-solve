#! /bin/bash
#
# 0fcpar.bash
# Copyright (C) 2018 shlomif <shlomif@telaviv1.shlomifish.org>
#
# Distributed under terms of the MIT license.
#
mkdir -p results
seq 14800 85000 | parallel -u -j4 python2 ../scripts/0fc_sol.py
