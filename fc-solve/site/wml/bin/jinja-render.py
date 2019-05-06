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
    fn = line.strip()
    template = env.get_template(fn+'.jinja')
    base_path = "../"*len([x for x in fn if x == '/'])
    open('dest/'+fn, 'wt').write(
        template.render(
            base_path=base_path,
            common_keywords="Freecell, Freecell Solver, solvers, " +
            "AI, artificial intelligence, solitaire, Simple Simon, " +
            "Baker's Game, Seahaven Towers, Shlomi Fish, games",
            desc="Freecell Solver " +
            "- a Program and a Library written in ANSI C for" +
            " Solving Games of Freecell and similar Solitaire Variants",
            filename=fn,
            host='fc-solve',
            solitairey='<a href="https://foss-card-games.github.io/' +
            'Solitairey/">Solitairey</a>',
            presentation_url="http://www.shlomifish.org/" +
            "lecture/Freecell-Solver/",
            doxygen_url=base_path + "michael_mann/",
            ))
