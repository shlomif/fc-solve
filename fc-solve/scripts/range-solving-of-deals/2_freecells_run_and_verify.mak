# Copyright 2012 by Shlomi Fish
#
# This program is distributed under the MIT (X11) License:
# L<http://www.opensource.org/licenses/mit-license.php>
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use,
# copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following
# conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.

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
