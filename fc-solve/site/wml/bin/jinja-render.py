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
import yaml

from jinja2 import Environment
from jinja2 import FileSystemLoader


def _calc_env():
    return Environment(
        loader=FileSystemLoader(['./src', './lib'])
        )


def _slurp(fn):
    with open(fn, 'rt') as f:
        return f.read()


with open('lib/static_constant_params.yaml', 'rt') as f:
    static_constant_params = yaml.safe_load(f)['static_constant_params']


def _htmlish(base):
    return _slurp('lib/' + base + '.htmlish')


games1 = _htmlish('games')
news1 = _htmlish('front-page-news')
old_news = _htmlish('old-news')
charts1 = _htmlish('4fc-deals-charts')
charts2 = _htmlish('4fc-deals-charts2')
msfreecell_note = _htmlish('msfreecell-note')
tocs = []

listbox_template = (
    '<div class="widget_wrapper {id}">'
    '<label for="{id}">{label_text}</label>' +
    '<select id="{id}" class="phoenix">' +
    '<option value="default" selected="selected">' +
    'Default</option>{options}</select>' +
    '</div>'
)

fc_listbox_id = "num_freecells"
fc_listbox = (listbox_template).format(
        id=fc_listbox_id,
        label_text='Freecells Number:',
        options=''.join(
            ['<option value="{num}">{num}</option>'.format(num=num)
             for num in range(8+1)]))

cols_listbox_id = "num_columns"
cols_listbox = (listbox_template).format(
        id=cols_listbox_id,
        label_text='Columns Number:',
        options=''.join(
            ['<option value="{num}">{num}</option>'.format(num=num)
             for num in range(1, 12+1)]))


def _get_files():
    with open('lib/make/jinja.txt', 'rt') as f:
        return f.readlines()


for line in _get_files():
    fn = line.strip()
    enable_jquery_ui = (fn != 'js-fc-solve/text/gui-tests.xhtml')
    base_path = "../"*len([x for x in fn if x == '/'])
    use_online_wrapper = ""
    if not re.match("^js-fc-solve/", fn):
        use_online_wrapper = """
<div class="use_online_wrapper"><a class="solve_online_button"
id="try_online_link" href="{}js-fc-solve/text/"><span
class="try_main">Use</span><br/>
<span class="try_main">Online</span><br/>
<br/>
<span class="try_note">Firefox, Chrome, Opera, or IE10+</span></a></div>
""".format(base_path)

    def requirejs_conf():
        return "requirejs.config({{ baseUrl: '{base_path}js', }});".format(
            base_path=base_path)

    def load_javascript_srcs(srcs):
        if isinstance(srcs, str):
            return load_javascript_srcs([srcs])
        return "".join([
            """<script src="{base_path}js/{js}"></script>""".format(
                base_path=base_path, js=js)
            for js in srcs])

    def pre_requirejs_javascript_tags():
        return load_javascript_srcs(
            [
                "jquery.querystring.js",
                "jquery.phoenix.js",
                "lodash.custom.min.js"])
    for production, dest in [(False, 'dest'), (True, 'dest-prod'), ]:
        env = _calc_env()
        env.globals['base_path'] = base_path
        env.globals['production'] = production
        env.globals['requirejs_conf'] = requirejs_conf
        env.globals['pre_requirejs_javascript_tags'] = \
            pre_requirejs_javascript_tags
        env.globals['load_javascript_srcs'] = \
            load_javascript_srcs
        template = env.get_template(fn+'.jinja')
        text = template.render(
                **static_constant_params,
                cols_listbox=cols_listbox,
                enable_jquery_ui=enable_jquery_ui,
                fc_listbox=fc_listbox,
                production=production,
                front_page_news=news1,
                games1=games1,
                old_news=old_news,
                charts1=charts1,
                charts2=charts2,
                msfreecell_note=msfreecell_note,
                base_path=base_path,
                desc="Freecell Solver " +
                "- a Program and a Library written in C for" +
                " Solving Games of Freecell and similar Solitaire Variants",
                filename=fn,
                index_filename=re.sub(
                    '(\\A|/)index\\.(?:x)?html\\Z',
                    lambda m: m.group(1), fn
                ),
                host='fc-solve',
                use_online_wrapper=use_online_wrapper,
                doxygen_url=base_path + "michael_mann/",
                arch_doc_url=base_path + "arch_doc/",
                )
        out_fn = dest+'/'+fn
        text = re.sub("\\A[\\s\\n]+", '', text)
        if re.search('<toc */ *>', text):
            tocs.append(out_fn)
        with open(out_fn, 'wt') as f:
            f.write(text)

# subprocess.check_call(["perl", "bin/tt-render.pl", ])
if False:
    subprocess.call(["perl", "-0777", "-i", "-p", "-I./lib", "-e",
                     "use HTML::Latemp::AddToc (); " +
                     "HTML::Latemp::AddToc->new->add_toc(\\$_);", "--"] + tocs)
