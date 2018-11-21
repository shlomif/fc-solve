#! /bin/bash
#
# dl-require.js.bash
# Copyright (C) 2018 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.

ver='2.3.5'
wget -O src/js/require.js https://requirejs.org/docs/release/"$ver"/minified/require.js
wget -O src/js/require--debug.js https://requirejs.org/docs/release/"$ver"/comments/require.js
