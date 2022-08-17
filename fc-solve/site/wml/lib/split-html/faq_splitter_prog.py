#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2019 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.

"""
Split Shlomi Fish's XHTML5 / HTML5 documents (for example his FAQ-list)
into individual pages per sections.
"""

import re

from lxml.html import XHTML_NAMESPACE

# from split_into_sections import XHTML_ARTICLE_TAG
# from split_into_sections import XHTML_SECTION_TAG
from split_into_sections import XhtmlSplitter


TITLE_RE = re.compile(
    " - Shlomi Fish['’]s Homesite\\Z",
    re.M | re.S,
)


def _process_title(self, title):
    return TITLE_RE.sub(
        "",
        title,
    )


COMMON_TITLE_CALLBACKS = {
    'process_header_text': _process_title,
    'process_main_title': _process_title,
}


def _tag_xpath(tag, TOP_LEVEL_CLASS):
    return (
        "self::node()[local-name() = '{tag}' and {clas}]".format(
            tag=tag,
            clas="@class='" + TOP_LEVEL_CLASS + "'"
        )
    )


def _xpath(TOP_LEVEL_CLASS):
    return _tag_xpath(
        tag='section', TOP_LEVEL_CLASS=TOP_LEVEL_CLASS
    )


# Removed:
# <script src="{base_path}js/main_all.js"></script>
FAQ_SECTION_FORMAT = '''<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE html>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
<title>Freecell Solver’s FAQ - {title}</title>
<meta charset="utf-8"/>
<meta name="description" content=
"Freecell Solver’s Frequently Asked Questions (FAQ) List - {title}"/>
<link rel="stylesheet" href="{base_path}style.css" media="screen" title=
"Normal"/>
<link rel="stylesheet" href="{base_path}print.css" media="print"/>
<link rel="shortcut icon" href="{base_path}favicon.ico" type=
"image/x-icon"/>
<meta name="viewport" content="width=device-width,initial-scale=1"/>
</head>
<body class="fancy_sects faq_indiv_entry">
<div class="header" id="header">
<a href="{base_path}"><img src="{base_path}images/evilphish-flipped.png"
alt="EvilPHish site logo"/></a>
<div class="leading_path"><a href="{base_path}">Freecell Solver’s
Homepage</a> → <a href="{base_path}faq.html" title=
"Frequently Asked Questions and Answers List (FAQ)">FAQ</a>
{breadcrumbs_trail}
→ <b>{title}</b>
</div>
</div>
<main class="faq faux main">
{body}</main>
<footer>
<div class="foot_left">
<ul class="bt_nav">
<li><a href="{base_path}">Home</a></li>
<li><a href="https://www.shlomifish.org/me/contact-me/">Contact Us</a></li>
</ul>
<p>Written, designed, and maintained by Shlomi Fish, <a href=
"mailto:shlomif@shlomifish.org">shlomif@shlomifish.org</a>.</p>
</div>
<a href="https://www.shlomifish.org/"><img
src="{base_path}images/bk2hp-v2.min.svg" class="bk2hp"
alt="Back to my Homepage"/></a></footer>
</body>
</html>'''

BACK_TO_SOURCE_PAGE_CSS_CLASS = "back_to_faq"
INDIVIDUAL_NODE_CSS_CLASS = "indiv_node"


def _wrap(ns):
    return ns


XML_NS = "http://www.w3.org/XML/1998/namespace"
NAMESPACES = {
    "xhtml": _wrap(XHTML_NAMESPACE),
    "xml": _wrap(XML_NS),
}


def _faq_gen(dest_dir):
    OUT_DN = dest_dir + "/faq-indiv-nodes"
    input_fn = dest_dir + "/faq.html"
    TOP_LEVEL_CLASS = 'faq fancy_sects lim_width wrap-me'
    TOP_LEVEL_CLASS = 'h2 faq'
    splitter = XhtmlSplitter(
        **COMMON_TITLE_CALLBACKS,
        back_to_source_page_css_class=BACK_TO_SOURCE_PAGE_CSS_CLASS,
        individual_node_css_class=INDIVIDUAL_NODE_CSS_CLASS,
        input_fn=input_fn,
        output_dirname=OUT_DN,
        path_to_all_in_one="../faq.html",
        path_to_images="../",
        relative_output_dirname="./faq-indiv-nodes/",
        section_format=FAQ_SECTION_FORMAT,
        container_elem_xpath=_xpath(TOP_LEVEL_CLASS=TOP_LEVEL_CLASS),
        ns=NAMESPACES,
    )
    splitter.process()


def main():
    dest_dir = "./dest-prod"
    _faq_gen(dest_dir=dest_dir)


if __name__ == '__main__':
    main()
