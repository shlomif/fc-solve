#! /usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2021 Shlomi Fish < https://www.shlomifish.org/ >
#
# Licensed under the terms of the MIT license.

"""

"""

import re

import yaml


env_keys = set()


def _process_env(env):
    ret = {}
    for line in env:
        m = re.match(
            '\\A([A-Za-z0-9_]+)=([A-Za-z0-9_]+)\\Z',
            line)
        key = m.group(1)
        val = m.group(2)
        assert key not in ret
        ret[key] = val
        env_keys.add(key)
    return ret


def main():
    """docstring for main"""
    with open("./.travis.yml", "rt") as infh:
        data = yaml.safe_load(infh)
    steps = []
    steps.append({"uses": ("actions/checkout@v2"), })
    if 0:
        steps.append({
            "run":
            ("cd workflow ; (ls -lrtA ; false)"), })
    elif 0:
        steps.append({
            "run":
            ("cd . ; (ls -lrtA ; false)"), })
    steps.append({"run": ("sudo apt-get update -qq"), })
    steps.append({
        "run": ("sudo apt-get --no-install-recommends install -y " +
                " ".join(["eatmydata"])
                ),
        }
    )
    steps.append({
        "run": ("sudo eatmydata apt-get --no-install-recommends install -y " +
                " ".join(data['addons']['apt']['packages'])
                ),
        }
    )
    local_lib_shim = 'local_lib_shim() { eval "$(perl ' + \
        '-Mlocal::lib=$HOME/' + \
        'perl_modules)"; } ; local_lib_shim ; '
    for arr in ['before_install', 'install', 'script']:
        steps += [{"run": local_lib_shim + x} for x in data[arr]]
    job = 'test-fc-solve'
    o = {'jobs': {job: {'runs-on': 'ubuntu-latest',
         'steps': steps, }},
         'name': 'use-github-actions', 'on': ['push', ], }
    if 'matrix' in data:
        if 'include' in data['matrix']:
            o['jobs'][job]['strategy'] = {'matrix': {'include': [
                {'env': _process_env(x['env']), }
                for x in data['matrix']['include']
            ], }, }
            o['jobs'][job]['env'] = {
                x: "${{ matrix.env." + x + " }}"
                for x in env_keys
            }
            o['jobs'][job]['if'] = \
                "${{ ! contains(env.ACT_SKIP, matrix.env.WHAT) }}"
        else:
            assert False
    with open(".github/workflows/use-github-actions.yml", "wt") as outfh:
        # yaml.safe_dump(o, outfh)
        yaml.safe_dump(o, stream=outfh, canonical=False, indent=4, )


main()
