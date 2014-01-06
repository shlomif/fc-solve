SHELL = bash

DUMPS = $(shell ls *.dump.xz)

SUMMARIES = $(patsubst %.dump.xz,%.summary,$(DUMPS))

all: $(SUMMARIES)

$(SUMMARIES): %.summary: %.dump.xz
	perl ../../source/scripts/stat-analysis-2.pl <(xzcat "$<") > "$@"
