#! /bin/bash
#
# mint-new-release.bash
# Copyright (C) 2018 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under the terms of the MIT license.
#

set -x
set -e
set -u
which cookiecutter
which qunit-cli
src="$(pwd)"
build="$src/../prerel-build"
assets_dir="$src/../../../Arcs/fc-solve-site-assets/fc-solve-site-assets"
mkdir "$build"
cd "$build"
"$src/Tatzer" -l n2t
make
FCS_TEST_BUILD=1 perl "$src"/run-tests.pl
cd "$src"
perl ../scripts/multi_config_tests.pl
cd "$build"
make package_source
unxz *.tar.xz
arc="$(echo *.tar)"
xz -9 --extreme *.tar
cp "$arc.xz" "$assets_dir/dest/fc-solve/"
cd "$assets_dir"
commit_fn="dest/commit.msg"
arc_path="dest/fc-solve/$arc.xz"
bash gen_src_arc_commit_msg.bash "$arc_path"  > "$commit_fn"
git add "$arc_path"
git commit -F "$commit_fn"
git push
make upload
rm -fr "$build"
