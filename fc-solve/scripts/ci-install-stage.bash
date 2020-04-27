#! /bin/bash
#
# ci-before-install.bash
# Copyright (C) 2019 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.
#
set -e -x
python3 -m pip install --user --upgrade cffi cookiecutter freecell_solver pycotap pysol_cards random2 six vnu_validator Zenfilter
# For G-S-V dzil
cpanm --quiet --notest Dist::Zilla Pod::Coverage::TrustPod Pod::Weaver::Section::Support Test::EOL Test::NoTabs Test::Kwalitee Test::Perl::Critic::Policy Test::Pod Test::Pod::Coverage
# For the website
cpanm --quiet App::Deps::Verify App::XML::DocBook::Builder
(cd fc-solve/site/wml/ && perl bin/my-cookiecutter.pl && deps-app plinst -i bin/common-required-deps.yml -i bin/required-modules.yml )
cpanm --quiet --notest Code::TidyAll::Plugin::ClangFormat Code::TidyAll::Plugin::TestCount Games::Solitaire::Verify::HorneAutomovePrune Task::FreecellSolver::Testing
(cd cpan/Games-Solitaire-Verify/Games-Solitaire-Verify/ && dzil authordeps --missing | cpanm --notest)
( cd fc-solve/site/wml && bash -x bin/install-npm-deps.sh )
git clone https://github.com/shlomif/rinutils && cd rinutils && mkdir b && cd b && cmake -DWITH_TEST_SUITE=OFF .. && make && sudo make install && cd ../.. && rm -fr rinutils
