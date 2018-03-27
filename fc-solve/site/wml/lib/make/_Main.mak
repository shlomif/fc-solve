SHELL = /bin/bash

all: dummy

include rules.mak
include include.mak
include lib/make/shlomif_common.mak

WITH_DEVEL_VERSION = 1

DEVEL_VER_USE_CACHE = 1

# Toggle to generate production code with compressed and merged JS code/etc.
PROD = 0

SKIP_EMCC = 0

D = ./dest
WML_FLAGS = -DBERLIOS=BERLIOS

#D = /home/httpd/html/ip-noise

TEMP_UPLOAD_URL = $${__HOMEPAGE_REMOTE_PATH}/fc-solve-temp
UPLOAD_URL = $(TEMP_UPLOAD_URL)

ifeq ($(PROD),1)

	D = ./dest-prod

	WML_FLAGS += -DPRODUCTION=1

	UPLOAD_URL = hostgator:domains/fc-solve/public_html

endif

IMAGES_PRE1 = $(SRC_IMAGES)
IMAGES = $(addprefix $(D)/,$(IMAGES_PRE1))

# WML_FLAGS = -DBERLIOS=BERLIOS

HTMLS = $(addprefix $(D)/,$(SRC_DOCS))

DOCS_PROTO = AUTHORS COPYING INSTALL NEWS README TODO USAGE
ADOC_CSS = asciidoc.css
ADOC_JS = asciidoc.js
DOCS_AUX_PROTO = $(ADOC_CSS) $(ADOC_JS)
DOCS_AUX_DIR = $(D)/docs/distro
ARC_DOCS = $(patsubst %,$(D)/%,$(DOCS_PROTO))
DOCS_AUX = $(patsubst %,$(DOCS_AUX_DIR)/%,$(DOCS_AUX_PROTO))
DOCS_HTMLS = $(patsubst %,$(D)/docs/distro/%.html,$(DOCS_PROTO))

INCLUDES_PROTO = std/logo.wml
INCLUDES = $(addprefix lib/,$(INCLUDES_PROTO))

# Remming out because it confuses the validator and no longer needed because
# the web-server now supports indexes.
# SUBDIRS_WITH_INDEXES = $(WIN32_BUILD_SUBDIRS)
#
SUBDIRS = $(addprefix $(D)/,$(SRC_DIRS))

INDEXES = $(addsuffix /index.html,$(SUBDIRS_WITH_INDEXES))


COMMON_PREPROC_FLAGS = -I $$HOME/conf/wml/Latemp/lib -I../lib
LATEMP_WML_FLAGS =$(shell latemp-config --wml-flags)

TTML_FLAGS += $(COMMON_PREPROC_FLAGS)
WML_FLAGS += $(COMMON_PREPROC_FLAGS)

WML_FLAGS += --passoption=2,-X3074 \
	-DLATEMP_SERVER=fc-solve -DLATEMP_THEME=better-scm \
	$(LATEMP_WML_FLAGS) --passoption=3,-I../lib/ \
	-I $${HOME}/apps/wml \
	--passoption=7,--skip=summary

JS_MEM_BASE = libfreecell-solver.js.mem
LIBFREECELL_SOLVER_JS_DIR = lib/fc-solve-for-javascript
LIBFREECELL_SOLVER_JS = $(LIBFREECELL_SOLVER_JS_DIR)/libfreecell-solver.js
DEST_LIBFREECELL_SOLVER_JS = $(D)/js/libfreecell-solver.min.js
DEST_LIBFREECELL_SOLVER_JS_NON_MIN = $(D)/js/libfreecell-solver.js
DEST_LIBFREECELL_SOLVER_JS_MEM = $(patsubst %,%/$(JS_MEM_BASE),$(D)/js $(D)/js-fc-solve/find-deal $(D)/js-fc-solve/text $(D)/js-fc-solve/automated-tests lib/for-node .)
DEST_QSTRING_JS = dest/js/jquery.querystring.js
DEST_BIGINT_JS = $(D)/js/big-integer.js

CSS_TARGETS = $(D)/style.css $(D)/print.css $(D)/jqui-override.css $(D)/web-fc-solve.css

DEST_WEB_FC_SOLVE_UI_MIN_JS = $(D)/js/web-fcs.min.js

FIND_INDEX__PYJS__SRC_BN = fc_solve_find_index_s2ints.py
FIND_INDEX__PYJS__pivot =  dist/fc_solve_find_index_s2ints.js

FIND_INDEX__PYJS__DEST_DIR = $(D)/js
FIND_INDEX__PYJS__DEST = $(FIND_INDEX__PYJS__DEST_DIR)/$(FIND_INDEX__PYJS__pivot)
FIND_INDEX__PYJS__TGT_DIR = lib/transcrypt_module
FIND_INDEX__PYJS__PY = $(FIND_INDEX__PYJS__TGT_DIR)/src/$(FIND_INDEX__PYJS__SRC_BN)
PIV = $(FIND_INDEX__PYJS__TGT_DIR)/dist
FIND_INDEX__PYJS__TGT = $(FIND_INDEX__PYJS__TGT_DIR)/$(FIND_INDEX__PYJS__pivot)

FIND_INDEX__PYJS__NODE_DIR = lib/for-node/js
FIND_INDEX__PYJS__NODE = $(FIND_INDEX__PYJS__NODE_DIR)/$(FIND_INDEX__PYJS__pivot)
ifeq ($(SKIP_EMCC),1)
	FIND_INDEX__PYJS__TARGETS =
	LIBFREECELL_SOLVER_JS__NODE__TARGETS =
	LIBFREECELL_SOLVER_JS__TARGETS =
else
	FIND_INDEX__PYJS__TARGETS = $(FIND_INDEX__PYJS__TGT) $(FIND_INDEX__PYJS__DEST) $(FIND_INDEX__PYJS__NODE)
	LIBFREECELL_SOLVER_JS__NODE__TARGETS = lib/for-node/js/libfreecell-solver.min.js
	LIBFREECELL_SOLVER_JS__TARGETS = $(DEST_LIBFREECELL_SOLVER_JS) $(DEST_LIBFREECELL_SOLVER_JS_NON_MIN) $(DEST_LIBFREECELL_SOLVER_JS_MEM)
endif

dummy : $(D) $(SUBDIRS) $(HTMLS) $(D)/download.html $(IMAGES) $(RAW_SUBDIRS) $(ARC_DOCS) $(INDEXES) $(DOCS_AUX) $(DOCS_HTMLS)  $(DEST_QSTRING_JS) $(DEST_WEB_FC_SOLVE_UI_MIN_JS) $(CSS_TARGETS) htaccesses_target

dummy: $(LIBFREECELL_SOLVER_JS__TARGETS)

dummy: $(FIND_INDEX__PYJS__TARGETS)

dummy: $(DEST_BIGINT_JS)

$(DEST_BIGINT_JS):
	browserify -s big-integer -r big-integer -o $@

STRIP_TRAIL_SPACE = perl -i -lpe 's/[ \t]+$$//'

$(FIND_INDEX__PYJS__TGT): ../../source/board_gen/$(FIND_INDEX__PYJS__SRC_BN)
	cat $< lib/js-epilogue.py > $(FIND_INDEX__PYJS__PY)
	cd $(FIND_INDEX__PYJS__TGT_DIR) && python3 build.py
	perl -i -lpe 's:(MAX_SHIFTREDUCE_LOOPS\s*=\s*1000)(;):$${1}00$${2}:g' $(PIV)/*.js
	perl -i -lne 'print unless /String.prototype.strip = / .. /^\s*\}/' $(PIV)/*.js
	$(STRIP_TRAIL_SPACE) $$(find $(FIND_INDEX__PYJS__TGT_DIR) -regextype egrep -regex '.*\.(js|html)')
	touch $@

$(FIND_INDEX__PYJS__DEST): $(FIND_INDEX__PYJS__TGT)
	mkdir $$(dirname "$@")
	cp -f $< $@

$(FIND_INDEX__PYJS__NODE): $(FIND_INDEX__PYJS__TGT)
	cp -f $< $@

SASS_STYLE = compressed
# SASS_STYLE = expanded
SASS_CMD = sass --style $(SASS_STYLE)

SASS_HEADERS = lib/sass/common-style.sass

$(CSS_TARGETS): $(D)/%.css: lib/sass/%.sass $(SASS_HEADERS)
	$(SASS_CMD) $< $@

$(D) $(SUBDIRS): % :
	@if [ ! -e $@ ] ; then \
		mkdir $@ ; \
	fi

RECENT_STABLE_VERSION = $(shell ./get-recent-stable-version.sh)

$(ARC_DOCS): $(D)/% : ../../source/%.txt
	cp -f "$<" "$@"

$(DOCS_AUX_DIR)/$(ADOC_CSS): $(DOCS_AUX_DIR)/%: ../../source/%
	cp -f "$<" "$@"

$(DOCS_AUX_DIR)/$(ADOC_JS): $(DOCS_AUX_DIR)/%: ../../source/%
	$(MULTI_YUI) -o $@ $<

$(DOCS_HTMLS): $(D)/docs/distro/% : ../../source/%
	cat "$<" | perl -0777 -lapE 's#<table #<table summary="identifiers on the left, descriptions on the right" #g' > "$@"

$(HTMLS): $(D)/% : src/%.wml src/.wmlrc template.wml $(INCLUDES)
	WML_LATEMP_PATH="$$(perl -MFile::Spec -e 'print File::Spec->rel2abs(shift)' '$@')" ; \
	(cd src && wml -o "$${WML_LATEMP_PATH}" $(WML_FLAGS) -DLATEMP_FILENAME="$(patsubst src/%.wml,%,$<)" $(patsubst src/%,%,$<))

$(IMAGES): $(D)/% : src/%
	cp -f $< $@

$(RAW_SUBDIRS): $(D)/% : src/%
	rm -fr $@
	cp -r $< $@

FC_SOLVE_SOURCE_DIR := $(PWD)/../../source

LIBFREECELL_SOLVER_JS_DIR__CMAKE_DIR = $(LIBFREECELL_SOLVER_JS_DIR)/CMAKE-BUILD-dir
LIBFREECELL_SOLVER_JS_DIR__CMAKE_CACHE = $(LIBFREECELL_SOLVER_JS_DIR__CMAKE_DIR)/CMakeCache.txt
LIBFREECELL_SOLVER_JS_DIR__DESTDIR = $(PWD)/$(LIBFREECELL_SOLVER_JS_DIR)/__DESTDIR
LIBFREECELL_SOLVER_JS_DIR__DESTDIR_DATA = $(LIBFREECELL_SOLVER_JS_DIR__DESTDIR)/fc-solve/share/freecell-solver/presetrc

FC_SOLVE_CMAKE_DIR := $(PWD)/$(LIBFREECELL_SOLVER_JS_DIR__CMAKE_DIR)

$(LIBFREECELL_SOLVER_JS_DIR__CMAKE_CACHE):
	mkdir -p $(LIBFREECELL_SOLVER_JS_DIR__CMAKE_DIR)
	( cd $(LIBFREECELL_SOLVER_JS_DIR__CMAKE_DIR) && cmake -DCMAKE_INSTALL_PREFIX=/fc-solve $(FC_SOLVE_SOURCE_DIR) && make generate_c_files)

$(LIBFREECELL_SOLVER_JS_DIR__DESTDIR_DATA): $(LIBFREECELL_SOLVER_JS_DIR__CMAKE_CACHE)
	( cd $(LIBFREECELL_SOLVER_JS_DIR__CMAKE_DIR) && make && make install DESTDIR=$(LIBFREECELL_SOLVER_JS_DIR__DESTDIR) )
	# We need that to make sure the timestamps are correct because apparently
	# the make install process updates the timestamps of CMakeCache.txt and
	# the Makefile there.
	find $(LIBFREECELL_SOLVER_JS_DIR__DESTDIR) -type f -print0 | xargs -0 touch

$(LIBFREECELL_SOLVER_JS): $(LIBFREECELL_SOLVER_JS_DIR__DESTDIR_DATA)
	( cd $(LIBFREECELL_SOLVER_JS_DIR) && make -f $(FC_SOLVE_SOURCE_DIR)/../scripts/Makefile.to-javascript.mak SRC_DIR=$(FC_SOLVE_SOURCE_DIR) CMAKE_DIR=$(FC_SOLVE_CMAKE_DIR) && $(STRIP_TRAIL_SPACE) *.js *.html)

clean_js:
	rm -f $(LIBFREECELL_SOLVER_JS_DIR)/*.js $(LIBFREECELL_SOLVER_JS_DIR)/*.bc

MULTI_YUI = ./bin/Run-YUI-Compressor

$(DEST_LIBFREECELL_SOLVER_JS): $(LIBFREECELL_SOLVER_JS)
	cp -f $< $@

$(DEST_QSTRING_JS): lib/jquery/jquery.querystring.js
	$(MULTI_YUI) -o $@ $<

WEB_RAW_JS = common-methods.js

DEST_WEB_RAW_JS = $(patsubst %,$(D)/js/%,$(WEB_RAW_JS))

dummy: $(DEST_WEB_RAW_JS)

$(DEST_WEB_RAW_JS): $(D)/js/%: lib/web-raw-js/%
	$(MULTI_YUI) -o $@ $<

WEB_FCS_UI_JS_SOURCES = $(D)/js/ms-rand.js src/js/gen-ms-fc-board.js src/js/web-fc-solve--expand-moves.js $(D)/js/web-fc-solve.js $(D)/js/web-fc-solve-ui.js

$(DEST_WEB_FC_SOLVE_UI_MIN_JS): $(WEB_FCS_UI_JS_SOURCES)
	$(MULTI_YUI) -o $@ $(WEB_FCS_UI_JS_SOURCES)

$(DEST_LIBFREECELL_SOLVER_JS_NON_MIN): $(LIBFREECELL_SOLVER_JS)
	cp -f $< $@

$(DEST_LIBFREECELL_SOLVER_JS_MEM): %: lib/fc-solve-for-javascript/libfreecell-solver.js.mem
	cp -f $< $@

FCS_VALID_DEST = $(D)/js/fcs-validate.js

TYPINGS = src/charts/dbm-solver-__int128-optimisation/typings/index.d.ts src/js/typings/index.d.ts

DEST_BABEL_JSES = $(D)/js/find-fc-deal-ui.js $(D)/js/ms-rand.js $(D)/js/web-fc-solve.js $(D)/js/web-fc-solve-ui.js
OUT_BABEL_JSES = $(patsubst $(D)/js/%,lib/out-babel/js/%,$(DEST_BABEL_JSES))

all: $(TYPINGS) $(DEST_BABEL_JSES)

$(OUT_BABEL_JSES): lib/out-babel/%.js: lib/babel/%.js
	babel -o $@ $<

$(DEST_BABEL_JSES): $(D)/%.js: lib/out-babel/%.js
	$(MULTI_YUI) -o $@ $<

$(TYPINGS):
	cd src/charts/dbm-solver-__int128-optimisation/ && typings install dt~jquery --global --save
	cd src/js && typings install dt~qunit --global --save

TEST_FCS_VALID_DEST = $(D)/js/web-fc-solve-tests--fcs-validate.js

TYPESCRIPT_DEST_FILES = $(FCS_VALID_DEST) $(TEST_FCS_VALID_DEST)
TYPESCRIPT_DEST_FILES__NODE = $(patsubst $(D)/%.js,lib/for-node/%.js,$(TYPESCRIPT_DEST_FILES))
TYPESCRIPT_COMMON_DEFS_FILES = src/js/typings/index.d.ts

JS_DEST_FILES__NODE = $(LIBFREECELL_SOLVER_JS__NODE__TARGETS) lib/for-node/js/web-fc-solve.js lib/for-node/js/web-fc-solve--expand-moves.js lib/for-node/js/web-fc-solve-tests.js

all: $(JS_DEST_FILES__NODE)

$(JS_DEST_FILES__NODE): lib/for-node/%.js: dest/%.js
	cp -f $< $@

all: $(TYPESCRIPT_DEST_FILES) $(TYPESCRIPT_DEST_FILES__NODE)

$(TYPESCRIPT_DEST_FILES): $(D)/%.js: src/%.ts
	tsc --module amd --out $@ $(TYPESCRIPT_COMMON_DEFS_FILES) $<

$(TYPESCRIPT_DEST_FILES__NODE): lib/for-node/%.js: src/%.ts
	tsc --target es5 --module commonjs --outDir lib/for-node/js $(TYPESCRIPT_COMMON_DEFS_FILES) $<

TS_CHART_DEST = $(D)/charts/dbm-solver-__int128-optimisation/chart-using-flot.js
TS_CHART2_DEST = $(D)/charts/fc-pro--4fc-intractable-deals--report/chart-using-flot.js

all: $(TS_CHART_DEST) $(TS_CHART2_DEST)

$(TS_CHART_DEST): $(D)/%.js: src/%.ts
	tsc --module amd --out $@ src/charts/dbm-solver-__int128-optimisation/typings/index.d.ts ./src/charts/dbm-solver-__int128-optimisation/jquery.flot.d.ts $<

$(TS_CHART2_DEST): $(D)/%.js: src/%.ts
	tsc --module amd --out $@ src/charts/dbm-solver-__int128-optimisation/typings/index.d.ts ./src/charts/dbm-solver-__int128-optimisation/jquery.flot.d.ts $<

$(TEST_FCS_VALID_DEST): $(patsubst $(D)/%.js,src/%.ts,$(FCS_VALID_DEST))

FC_PRO_4FC_DUMPS = $(filter charts/fc-pro--4fc-intractable-deals--report/data/%.dump.txt,$(SRC_IMAGES))
FC_PRO_4FC_TSVS = $(patsubst %.dump.txt,$(D)/%.tsv,$(FC_PRO_4FC_DUMPS))
FC_PRO_4FC_FILTERED_TSVS = $(patsubst %.dump.txt,$(D)/%.filtered.tsv,$(FC_PRO_4FC_DUMPS))

$(FC_PRO_4FC_TSVS): $(D)/%.tsv: src/%.dump.txt
	perl ../../scripts/convert-dbm-fc-solver-log-to-tsv.pl <(< "$<" perl -lapE 's#[^\t]*\t##') | perl -lanE 'print join"\t",@F[0,2]' > "$@"

$(FC_PRO_4FC_FILTERED_TSVS): %.filtered.tsv : %.tsv
	perl -lanE 'say if ((not /\A[0-9]/) or ($$F[0] % 1_000_000 == 0))' < "$<" > "$@"

$(D)/js-fc-solve/text/index.html: lib/FreecellSolver/ExtractGames.pm ../../source/USAGE.txt

$(D)/charts/fc-pro--4fc-intractable-deals--report/index.html $(D)/charts/fc-pro--4fc-deals-solvability--report/index.html: $(FC_PRO_4FC_FILTERED_TSVS) $(FC_PRO_4FC_TSVS)

all: $(FC_PRO_4FC_TSVS) $(FC_PRO_4FC_FILTERED_TSVS)

.PHONY:

# Build index.html pages for the appropriate sub-directories.
$(INDEXES): $(D)/%/index.html : src/% gen_index.pl
	perl gen_index.pl $< $@

ALL_HTACCESSES = $(D)/.htaccess

htaccesses_target: $(ALL_HTACCESSES)

$(ALL_HTACCESSES): $(D)/%.htaccess: src/%my_htaccess.conf
	cp -f $< $@

upload: all
	$(RSYNC) -a $(D)/ $(UPLOAD_URL)

# upload_temp: all
#	$(RSYNC) $(TEMP_UPLOAD_URL)

upload_local: all
	$(RSYNC) -a $(D)/ /var/www/html/shlomif/fc-solve-temp

TEST_ENV = SKIP_EMCC="$(SKIP_EMCC)"

clean:
	rm -f lib/fc-solve-for-javascript/*.bc lib/fc-solve-for-javascript/*.js $(TYPESCRIPT_DEST_FILES__NODE) $(TYPESCRIPT_DEST_FILES) $(TS_CHART_DEST)

# A temporary target to edit the active files.
edit:
	gvim -o src/js/fcs-validate.ts src/js/web-fc-solve-tests--fcs-validate.ts
