#! /bin/bash
#
# 0fcpar.bash
# Copyright (C) 2018 shlomif <shlomif@telaviv1.shlomifish.org>
#
# Distributed under terms of the MIT license.
#
mkdir -p results
seq 26000 85000 | parallel -u -j4 "$HOME"/Download/unpack/prog/python/pypy2-v5.7.1-src/pypy/goal/pypy-c ../scripts/0fc_sol.py
