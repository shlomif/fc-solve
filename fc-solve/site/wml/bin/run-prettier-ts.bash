#!/usr/bin/env bash
prettier --parser typescript --arrow-parens always --tab-width 4 --trailing-comma all --write src/js/*.ts
