# This file is part of Freecell Solver. It is subject to the license terms in
# the COPYING.txt file found in the top-level directory of this distribution
# and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
# Freecell Solver, including this file, may be copied, modified, propagated,
# or distributed except according to the terms contained in the COPYING file.
#
# Copyright (c) 2012 Shlomi Fish

# ================================================
#
# This GNU makefile solves the range of deals from MIN_DEAL to MAX_DEAL
# using the fc-solve "tea-for-two" preset, possibly in parallel (if one
# specifies the "-j4"/etc. flag). Typing "make" will eventually
# produce a summary of the results.
#
MIN_DEAL = 100001
MAX_DEAL = 400000

SOLS_DIR = 2fc-sols
DEALS = $(shell seq $(MIN_DEAL) $(MAX_DEAL))

DEALS_SOLS = $(patsubst %,$(SOLS_DIR)/%.sol,$(DEALS))
DEALS_VERIFIES = $(patsubst %,$(SOLS_DIR)/%.verify,$(DEALS))

UNSOLVED_SUMMARY_FILE = 2fc-unsolved.txt

SOLS_DIR_STAMP = $(SOLS_DIR)/stamp

all: summary

$(SOLS_DIR_STAMP):
	mkdir -p $(SOLS_DIR)
	touch $@

$(DEALS_SOLS): %: $(SOLS_DIR_STAMP)
	pi-make-microsoft-freecell-board -t "$(patsubst $(SOLS_DIR)/%.sol,%,$@)" | \
		fc-solve --freecells-num 2 -sam -sel -p -t -l "tea-for-two" -mi 2000000 -o "$@" -

$(DEALS_VERIFIES): %.verify: %.sol
	if grep -qP '^This game is solveable' "$<" && ! verify-solitaire-solution --freecells-num 2 "$<" ; then \
		echo "Error at deal No. $@"; \
		exit 1 ; \
	else \
		touch "$@" ; \
	fi

summary: $(UNSOLVED_SUMMARY_FILE)

.PHONY: all summary

$(UNSOLVED_SUMMARY_FILE): $(DEALS_VERIFIES)
	grep --include='*.sol' -rlP '^(?:I could not solve|Iterations count exceeded)' $(SOLS_DIR) | \
	perl -lne 'print $$1 if m{\A2fc-sols/(\d+)\.sol\z}' | sort -n > $@

%.show:
	@echo "$* = $($*)"
