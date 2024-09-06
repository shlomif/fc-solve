#!/usr/bin/env python
#
# fuzz-build.py
# Copyright (C) 2018 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.
#

# importing click
import click
import os
import sys
from subprocess import check_call, check_output


@click.command()
@click.option('--clean/--no-clean')
@click.option('-g/--no-g')
@click.option('-t/--no-t')
@click.option('--rb/--no-rb')
def main(clean, g, t, rb):
    if g:
        os.environ["FCS_GCC"] = "1"
        seed = 1 if rb else 1
    else:
        os.environ["CC"] = check_output(['which', 'clang']).decode().strip()
        os.environ["CXX"] = check_output(['which', 'clang++']).decode().strip()
        os.environ["FCS_CLANG"] = "1"
        seed = 1 if rb else 1
    os.environ["HARNESS_BREAK"] = "1"
    SLIGHTLY_WRONG_GCC_FLAG_SEE_man_gcc =\
        "-frandom-seed = 24"
    os.environ["CFLAGS"] = "-Werror" + (
        (" " + SLIGHTLY_WRONG_GCC_FLAG_SEE_man_gcc) if (g and rb) else ""
    )
    os.environ["SOURCE_DATE_EPOCH"] = "0"
    cmd = "../scripts/Tatzer && make -j5"
    if t:
        cmd += " && (set -e -x; if perl ../source/run-tests.pl " + \
            "--should-skip-tests ; then true ; else " + \
            "perl ../source/run-tests.pl ; fi )"
    if rb:
        os.environ["REPRODUCIBLE_BUILDS"] = "1"
        cmd = 'bash -c ". ~/.bashrc && Theme fcs && _reprb_diff_builds"'
    while True:
        for num_args in range(1, 10+1):
            print("Checking seed = {} numargs = {}".format(seed, num_args))
            os.environ["FCS_THEME_RAND"] = "{}".format(seed)
            os.environ["FCS_THEME_RAND_COUNT"] = "{}".format(num_args)
            try:
                check_call(["bash", "-c", cmd, ])
            except Exception as e:
                print(e)
                print("seed = {} numargs = {} failed".format(seed, num_args))
                sys.exit(1)
            if clean:
                os.unlink("run-t-{}-{}.bash".format(seed, num_args))
        seed += 1


if __name__ == "__main__":
    main()
