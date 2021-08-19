#! /usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2021 Shlomi Fish < https://www.shlomifish.org/ >
#
# Licensed under the terms of the MIT license.

"""

"""
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

def out(out_fn, text):
    """docstring for out"""
    with open(out_fn, 'wt') as f:
        f.write(text)

def o_htmlish(base, text):
    return out('lib/' + base + '.htmlish', text)

o_htmlish('fc_listbox', fc_listbox)
o_htmlish('cols_listbox', cols_listbox)
