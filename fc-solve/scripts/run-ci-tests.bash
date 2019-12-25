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
emsdk_path="`pwd`/../emsdk"
git clone https://github.com/emscripten-core/emsdk "$emsdk_path"
a="`pwd`"
cd "$emsdk_path"
./emsdk update-tags
./emsdk install latest
./emsdk activate latest
. "emsdk_env.sh"
cd "$a"
skip_emscripten_tests__disable_me_after_fixing_the_emcc_issue()
{
    export SKIP_EMCC='1'
}
skip_emscripten_tests__disable_me_after_fixing_the_emcc_issue
export FC_SOLVE__MULT_CONFIG_TESTS__TRACE=1 SKIP_SPELL_CHECK=1
which dbtoepub
export DBTOEPUB="/usr/bin/ruby $(which dbtoepub)"
export AVOID_NUM=5
export DOCBOOK5_XSL_STYLESHEETS_PATH=/usr/share/xml/docbook/stylesheet/docbook-xsl-ns
# For https://reproducible-builds.org/ and better caching.
enable_reproducible_builds()
{
    export REPRODUCIBLE_BUILDS=1
    slightly_wrong_gcc_flag_see_man_gcc="-frandom-seed=1977";
    export CFLAGS="$slightly_wrong_gcc_flag_see_man_gcc"
}
enable_reproducible_builds
export FC_SOLVE__TESTS_RERUNS_CACHE_DATA_DIR="$HOME/fc-solve-tests-reruns-cache"
cd fc-solve/source
echo "\$HOME = < $HOME > ; PWD = < $PWD > ; TRAVIS_BUILD_DIR = < $TRAVIS_BUILD_DIR >"
# For avoiding valgrind issues on travis:
avoid_valgrind_issues_on_travis()
{
    export FC_SOLVE__MULT_CONFIG_TESTS__GCC_ARCH="x64"
}
avoid_valgrind_issues_on_travis
perl ../scripts/multi_config_tests.pl 2>&1 | zenfilter --count-step=500 --last=500 --filter='^Running:\s*\{' --suppress-last-on="All tests successful\\.\\n*\\Z"
