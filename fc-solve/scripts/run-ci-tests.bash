#! /bin/bash
#
# run-ci-tests.bash
# Copyright (C) 2018 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.
#
set -e
set -x
set -o pipefail
export FC_SOLVE__MULT_CONFIG_TESTS__TRACE=1 SKIP_SPELL_CHECK=1
cd fc-solve/source
perl ../scripts/multi_config_tests.pl 2>&1 | zenfilter --count-step=500 --last=500 --filter='^Running:\s*\{' --suppress-last-on="All tests successful\\.\\n*\\Z"
