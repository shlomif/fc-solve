#!/bin/bash
d="$(dirname "$0")"
ctags "$d"/*.[ch] "$d"/patsolve-shlomif/patsolve/*.[ch]
