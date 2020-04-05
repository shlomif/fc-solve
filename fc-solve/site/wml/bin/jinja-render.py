#! /usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2019 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.

"""

"""
import re
import subprocess

from jinja2 import Environment
from jinja2 import FileSystemLoader

env = Environment(
        loader=FileSystemLoader(['./src', './lib'])
        )


def _slurp(fn):
    return open(fn, 'rt').read()


def _htmlish(base):
    return _slurp('lib/' + base + '.htmlish')


games1 = _htmlish('games')
news1 = _htmlish('front-page-news')
old_news = _htmlish('old-news')
charts1 = _htmlish('4fc-deals-charts')
charts2 = _htmlish('4fc-deals-charts2')
msfreecell_note = _htmlish('msfreecell-note')
tocs = []

fc_listbox_id = "num_freecells"
fc_listbox = (
    '<label for="{id}">Freecells Number:</label>' +
    '<select id="{id}" class="phoenix">' +
    '<option value="default" selected="selected">' +
    'Default</option>{options}</select>').format(
        id=fc_listbox_id,
        options=''.join(
            ['<option value="{num}">{num}</option>'.format(num=num)
             for num in range(8+1)]))

cols_listbox_id = "num_columns"
cols_listbox = (
    '<label for="{id}">Columns Number:</label>' +
    '<select id="{id}" class="phoenix">' +
    '<option value="default" selected="selected">' +
    'Default</option>{options}</select>').format(
        id=cols_listbox_id,
        options=''.join(
            ['<option value="{num}">{num}</option>'.format(num=num)
             for num in range(1, 12+1)]))

for line in open('lib/make/jinja.txt', 'rt'):
    fn = line.strip()
    template = env.get_template(fn+'.jinja')
    base_path = "../"*len([x for x in fn if x == '/'])
    try_online_wrapper = ""
    if not re.match("^js-fc-solve/", fn):
        try_online_wrapper = """
<div class="try_online_wrapper"><a class="solve_online_button"
id="try_online_link" href="{}js-fc-solve/text/"><span
class="try_main">Try</span><br/>
<span class="try_main">Online</span><br/>
<br/>
<span class="try_note">Firefox, Chrome, Opera, or IE10+</span></a></div>
""".format(base_path)
    for production, dest in [(False, 'dest'), (True, 'dest-prod'), ]:
        text = template.render(
                cols_listbox=cols_listbox,
                fc_listbox=fc_listbox,
                production=production,
                front_page_news=news1,
                games1=games1,
                old_news=old_news,
                charts1=charts1,
                charts2=charts2,
                msfreecell_note=msfreecell_note,
                base_path=base_path,
                common_keywords="Freecell, Freecell Solver, solvers, " +
                "AI, artificial intelligence, solitaire, Simple Simon, " +
                "Baker's Game, Seahaven Towers, Shlomi Fish, games",
                desc="Freecell Solver " +
                "- a Program and a Library written in ANSI C for" +
                " Solving Games of Freecell and similar Solitaire Variants",
                filename=fn,
                host='fc-solve',
                try_online_wrapper=try_online_wrapper,
                solitairey='<a href="https://foss-card-games.github.io/' +
                'Solitairey/">Solitairey</a>',
                presentation_url="http://www.shlomifish.org/" +
                "lecture/Freecell-Solver/",
                doxygen_url=base_path + "michael_mann/",
                arch_doc_url=base_path + "arch_doc/",
                )
        out_fn = dest+'/'+fn
        text = re.sub("\\A[\\s\\n]+", '', text)
        if re.search('<toc */ *>', text):
            tocs.append(out_fn)
        open(out_fn, 'wt').write(text)

subprocess.call(["perl", "-0777", "-i", "-p", "-I./lib", "-e",
                 "use HTML::Latemp::AddToc (); " +
                 "HTML::Latemp::AddToc->new->add_toc(\\$_);", "--"] + tocs)
