#!/usr/bin/env python3
import os, os.path
import shutil
import subprocess

SROOT = 'src'
DROOT = 'dist'

def run(cmd):
    print('\t' + cmd)
    subprocess.run(cmd, shell=True, check=True)

def main():

    # setup
    if not os.path.exists(DROOT):
        os.mkdir(DROOT)
    if os.path.exists(os.path.join(SROOT, '__javascript__')):
        shutil.rmtree(os.path.join(SROOT, '__javascript__'))

    # transpile src/
    run('transcrypt -xni -b -m --parent=.none src/fc_solve_find_index_s2ints.py')
    shutil.copy(os.path.join(SROOT, '__javascript__', 'fc_solve_find_index_s2ints.min.js'), os.path.join(DROOT, 'fc_solve_find_index_s2ints.min.js'))
    shutil.copy(os.path.join(SROOT, '__javascript__', 'fc_solve_find_index_s2ints.js'), os.path.join(DROOT, 'fc_solve_find_index_s2ints.js'))




#########################
###  Helper functions




#########################
###  Start the program

if __name__ == '__main__':
    main()
