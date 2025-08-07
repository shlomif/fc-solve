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
@click.option('--byorder/--no-byorder')
@click.option('--clean/--no-clean')
@click.option('--maxnum', default=10)
@click.option('-g/--no-g')
@click.option('-t/--no-t')
@click.option('--rb/--no-rb')
def main(byorder, clean, maxnum, g, t, rb):
    if g:
        os.environ["FCS_GCC"] = "1"
        seed = 1 if rb else 1
    else:
        os.environ["CC"] = check_output(['which', 'clang']).decode().strip()
        os.environ["CXX"] = check_output(['which', 'clang++']).decode().strip()
        os.environ["FCS_CLANG"] = "1"
        seed = 1 if rb else 1
    num_flags = -1
    if byorder:
        query_flag = "FCS_THEME_RAND_TELL_FLAGS_COUNT"
        os.environ[query_flag] = "1"
        query_cmd = "../scripts/Tatzer"
        num_flags = int(check_output([query_cmd]).decode().strip())
        del os.environ[query_flag]
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
    if byorder:
        os.environ["FCS_THEME_RAND_BY_ORDER"] = "1"
        while True:
            for num_args in [1,]:
                for pos in range(num_flags):
                    print("Checking pos = {} numargs = {}".format(
                        pos, num_args))
                    os.environ["FCS_THEME_RAND"] = "{}".format(pos)
                    os.environ["FCS_THEME_RAND_COUNT"] = "{}".format(num_args)
                    try:
                        check_call(["bash", "-c", cmd, ])
                    except Exception as e:
                        print(e)
                        print("pos = {} numargs = {} failed".format(
                            pos, num_args))
                        sys.exit(1)
                    if clean:
                        fn = "run-t-{}-{}.bash".format(pos, num_args)
                        check_call(["cat", fn])
                        os.unlink(fn)
            return

    while True:
        for num_args in range(1, maxnum+1):
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
