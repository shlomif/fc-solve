#!/bin/bash
d="$(dirname "$0")"
ctags "$d"/*.[ch] "$d"/patsolve/patsolve/*.[ch]
