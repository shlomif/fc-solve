#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2024 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.

"""
Check a URL for being online, and somewhat functional.

( https://en.wiktionary.org/wiki/litmus_test )
"""

import click
import re
import requests


@click.command()
@click.option('--url')
@click.option('--title')
@click.option('--match')
def main(url, title, match):
    # dest_dir = sys.argv[1]
    r = requests.get(url)
    assert r.status_code == 200
    assert re.search(pattern=match, string=r.content.decode('utf-8'))


if __name__ == '__main__':
    main()
