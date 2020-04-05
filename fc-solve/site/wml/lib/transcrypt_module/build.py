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
    js_dir = '__target__'
    if os.path.exists(os.path.join(SROOT, js_dir)):
        shutil.rmtree(os.path.join(SROOT, js_dir))

    # transpile src/
    run('transcrypt --esv 6 -xni -b -m --parent=.none src/fc_solve_find_index_s2ints.py')
    run('rollup ' + os.path.join(SROOT, js_dir, 'fc_solve_find_index_s2ints.js') + ' --o ' + os.path.join(DROOT, 'fc_solve_find_index_s2ints.min.js') + ' --f cjs')
    run('rollup ' + os.path.join(SROOT, js_dir, 'fc_solve_find_index_s2ints.js') + ' --o ' + os.path.join(DROOT, 'fc_solve_find_index_s2ints.js') + ' --f cjs')
    # shutil.copy(os.path.join(SROOT, js_dir, 'fc_solve_find_index_s2ints.js'), os.path.join(DROOT, 'fc_solve_find_index_s2ints.js'))




#########################
###  Helper functions




#########################
###  Start the program

if __name__ == '__main__':
    main()
