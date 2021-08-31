#! /usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2021 Shlomi Fish < https://www.shlomifish.org/ >
#
# Licensed under the terms of the MIT license.

"""
fc-solve/CI-testing/translate-travis.yml-to-github-actions.py :

This program translates fc-solve's .travis.yml to GitHub actions
and ACT workflows ( https://github.com/nektos/act ).

While ostensibly FOSS, it most probably is not generic enough
for your use cases.

"""

import re

import yaml


def _add_condition_while_excluding_gh_actions(job_dict, is_act):
    """
    See:

    https://github.com/actions/runner/issues/480

    Workflow level `env` does not work properly in all fields. · Issue #480

    """
    if is_act:
        job_dict['if'] = \
            "${{ ! contains(env.ACT_SKIP, matrix.env.WHAT) }}"


def generate(output_path, is_act):
    """docstring for main"""
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
            _add_condition_while_excluding_gh_actions(
                job_dict=o['jobs'][job],
                is_act=is_act,
            )
        else:
            assert False
    with open(output_path, "wt") as outfh:
        # yaml.safe_dump(o, outfh)
        yaml.safe_dump(o, stream=outfh, canonical=False, indent=4, )


def main():
    generate(
        output_path=".github/workflows/use-github-actions.yml",
        is_act=False,
    )
    generate(
        output_path=".act-github/workflows/use-github-actions.yml",
        is_act=True,
    )


if __name__ == "__main__":
    main()
