#! /usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2019 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.

"""

"""
from jinja2 import Environment
from jinja2 import FileSystemLoader

env = Environment(
        loader=FileSystemLoader(['./src', './lib'])
        )
for line in open('lib/make/jinja.txt', 'rt'):
    template = env.get_template(line.strip()+'.jinja')
    open('dest/'+line, 'wt').write(template.render())
