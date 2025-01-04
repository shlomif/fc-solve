#! /bin/bash
#
# ci-before-install.bash
# Copyright (C) 2019 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.
#
set -e -x
. fc-solve/scripts/common-defs.bash
mkdir -p ~/bin
ln -fs /usr/bin/make ~/bin/gmake
# rsync is needed by fc-solve/site/wml/bin/my-cookiecutter.pl
which rsync
export PATH="$PATH:$HOME/bin"
eval "$(GIMME_GO_VERSION=1.16 gimme)"
# go get -u github.com/tdewolff/minify/cmd/minify

(
    mkdir -p $HOME/src
    cd $HOME/src
    rm -fr minify
    git clone https://github.com/tdewolff/minify.git
    cd minify
    go install ./cmd/minify
)
which minify
      # - docker run -dit --name emscripten -v $(pwd):/src trzeci/emscripten:sdk-incoming-64bit bash
      # - sudo cpanm Text::Hunspell
cpanm local::lib
eval "$(perl -Mlocal::lib=$HOME/perl_modules)"
    # For the fc-solve docs
gem install --bindir "$userbin" --user asciidoctor
    # For wml
cpanm --notest Bit::Vector Carp::Always Class::XSAccessor Docker::CLI::Wrapper File::Which GD Getopt::Long HTML::Toc HTML::TocGenerator IO::All Image::Size Path::Tiny Perl::Critic Perl::Tidy Test::Code::TidyAll Term::ReadKey
cpanm --quiet --notest Alien::Tidyp Pod::Xhtml Task::Latemp
    # - bash fc-solve/scripts/install-git-cmakey-program-system-wide.bash 'git' 'libtap' 'https://github.com/shlomif/libtap-prev.git'
LIBS="-lm" bash fc-solve/scripts/install-git-autoconfy-program-system-wide.bash 'git' '.' 'https://github.com/hypermail-project/hypermail.git' ""
# bash fc-solve/scripts/install-git-cmakey-program-system-wide.bash 'git' '.' 'https://github.com/shlomif/libtap.git' "cmake"
bash fc-solve/scripts/install-git-cmakey-program-system-wide.bash 'git' 'installer' 'https://github.com/thewml/latemp.git'
( cd .. && rm -fr "wml-extended-apis" && git clone https://github.com/thewml/wml-extended-apis.git && cd wml-extended-apis/xhtml/1.x && bash Install.bash )
( cd .. && rm -fr "wml-affiliations" && git clone https://github.com/shlomif/wml-affiliations.git && cd wml-affiliations/wml && bash Install.bash )
( cd .. && rm -fr "latemp" && git clone https://github.com/thewml/latemp.git && cd latemp/support-headers && perl install.pl )
