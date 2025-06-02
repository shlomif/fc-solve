#! /bin/sh
#
# Copyright (C) 2018 shlomif <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.
#
set -e
npm install
npx puppeteer browsers install firefox
exit

#     amdefine big-integer browserify camel-case coffeescript eslint eslint-config-google flatted html-minifier jquery prettier qunit requirejs rollup sass typescript uglify-es yarn \
#     shlomif/qunit-puppeteer \
#     @babel/cli @babel/core @babel/preset-env \
#     @types/jquery @types/qunit \
#     @typescript-eslint/parser \
#     @typescript-eslint/eslint-plugin \
