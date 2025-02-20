LATEMP_ROOT_SOURCE_DIR := .
LATEMP_ABS_ROOT_SOURCE_DIR := $(realpath $(LATEMP_ROOT_SOURCE_DIR))

all: real_all

include lib/make/shlomif_common.mak
include lib/make/sf-filefind-rules-mak.mak
include lib/make/include.mak

# Toggle to generate production code with compressed and merged JS code/etc.
PROD = 0
LOCAL_BROWSER_TESTS = 0

SKIP_EMCC ?= 0

D = dest
TESTING_ENV__URL_SUFFIX := fc-solve-temp

BASE_LOCAL_UPLOAD_PREFIX := $(HOME)/Arcs/temp/var/www/html/shlomif

TEMP_UPLOAD_URL = $${__HOMEPAGE_REMOTE_PATH}/$(TESTING_ENV__URL_SUFFIX)
# TEMP_UPLOAD_URL = $${__HOMEPAGE_REMOTE_PATH}/fc-solve-t2
# TEMP_UPLOAD_URL = $${__HOMEPAGE_REMOTE_PATH}/1

TEMP_UPLOAD_URL_LOCAL = $(BASE_LOCAL_UPLOAD_PREFIX)/$(TESTING_ENV__URL_SUFFIX)/
UPLOAD_URL = $(TEMP_UPLOAD_URL)

STAGING_URL_SUFFIX := fc-solve-staging

ifeq ($(PROD),1)

    TEST_SITE_URL_SUFFIX = $(STAGING_URL_SUFFIX)
    D = dest-prod
    UPLOAD_URL = hostgator:domains/fc-solve/public_html
    STAGING_UPLOAD_URL = $${__HOMEPAGE_REMOTE_PATH}/$(STAGING_URL_SUFFIX)
    BETA_UPLOAD_URL = $${__HOMEPAGE_REMOTE_PATH}/fc-solve-animated-sol--prod
    MULTI_YUI = terser --compress --comments "false"
    SASS_DEBUG_FLAGS =

else

    TEST_SITE_URL_SUFFIX = $(TESTING_ENV__URL_SUFFIX)
    MULTI_YUI = ./bin/cat-o
    BETA_UPLOAD_URL = $${__HOMEPAGE_REMOTE_PATH}/fc-solve-animated-sol--debug2
    SASS_DEBUG_FLAGS = --sourcemap-embed

endif

ifeq ($(LOCAL_BROWSER_TESTS),0)

    BROWSER_TESTS_URL__BASE = https://www.shlomifish.org/

else

    BROWSER_TESTS_URL__BASE = http://127.0.0.1:2400/
    STAGING_UPLOAD_URL = $(BASE_LOCAL_UPLOAD_PREFIX)/$(STAGING_URL_SUFFIX)

endif

FAQ_PIVOT := $(D)/faq-indiv-nodes/which_prog_lang.xhtml

$(FAQ_PIVOT): $(D)/faq.html
	python3 lib/split-html/faq_splitter_prog.py --dest-dir "$(D)" && touch "$@"

real_all: $(FAQ_PIVOT)

POST_DEST = $(D)
DEST_JS_DIR = $(D)/js
BASE_FC_SOLVE_SOURCE_DIR := ../../source
IMAGES = $(addprefix $(D)/,$(SRC_IMAGES))
HTMLS = $(addprefix $(D)/,$(SRC_DOCS))

FC_SOLVE_SOURCE_DIR := $(PWD)/$(BASE_FC_SOLVE_SOURCE_DIR)

DOCS_PROTO = AUTHORS COPYING HACKING INSTALL NEWS README TODO USAGE
ADOC_CSS = asciidoctor.css
# ADOC_JS = asciidoc.js
DOCS_AUX_PROTO = $(ADOC_CSS)
DOCS_AUX_DIR = $(D)/docs/distro
ARC_DOCS = $(patsubst %,$(D)/%,$(DOCS_PROTO))
DOCS_AUX = $(patsubst %,$(DOCS_AUX_DIR)/%,$(DOCS_AUX_PROTO))
DOCS_HTMLS = $(patsubst %,$(D)/docs/distro/%.html,$(DOCS_PROTO))

SUBDIRS = $(addprefix $(D)/,$(SRC_DIRS))

Solver_Dest_Dir := $(D)/js-fc-solve/text
DEST_mem_dirs = $(DEST_JS_DIR) $(D)/js-fc-solve/find-deal $(Solver_Dest_Dir) $(D)/js-fc-solve/automated-tests lib/for-node/js/

include lib/make/generated/emcc_libs.mak

DEST_QSTRING_JS = $(D)/js/jquery.querystring.js
BASE_BROWSERIFY_JS = big-integer.js flatted.js qunit.js
dest_jsify = $(addprefix $(DEST_JS_DIR)/,$(1))
DEST_BROWSERIFY_JS = $(call dest_jsify,$(BASE_BROWSERIFY_JS))
BASE_Solitairey_JS = application.js auto-stack-clear.js auto-turnover.js autoplay.js freecell.js iphone.js solitaire.js solver-freecell.js statistics.js yui-breakout.js
DEST_Solitairey_JS = $(call dest_jsify,$(BASE_Solitairey_JS))
BASE_lodash_Solitairey_JS = lodash.custom.min.js
DEST_lodash_Solitairey_JS = $(call dest_jsify,$(BASE_lodash_Solitairey_JS))
BASE_yui_min_Solitairey_JS = yui-all-min.js
BASE_yui_Solitairey_JS = yui-debug.js
DEST_yui_Solitairey_JS = $(call dest_jsify,$(BASE_yui_Solitairey_JS))
DEST_yui_min_Solitairey_JS = $(call dest_jsify,$(BASE_yui_min_Solitairey_JS))

DEST_WEB_FC_SOLVE_UI_MIN_JS = $(DEST_JS_DIR)/web-fcs.min.js

include lib/make/deps.mak

real_all : $(D) $(SUBDIRS) $(HTMLS) $(D)/download.html $(IMAGES) $(RAW_SUBDIRS) $(ARC_DOCS) $(DOCS_AUX) $(DOCS_HTMLS) $(DEST_QSTRING_JS) $(DEST_WEB_FC_SOLVE_UI_MIN_JS) htaccesses_target

real_all:$(DEST_BROWSERIFY_JS) $(DEST_Solitairey_JS) $(DEST_yui_Solitairey_JS) $(DEST_lodash_Solitairey_JS)

real_all: $(DEST_yui_min_Solitairey_JS)

OUT_PREF = lib/out-babel/js
out_pref_jsify = $(addprefix $(OUT_PREF)/,$(1))
OUT_BROWSERIFY_JS = $(call out_pref_jsify,$(BASE_BROWSERIFY_JS))

$(DEST_BROWSERIFY_JS): $(DEST_JS_DIR)/%: $(OUT_PREF)/%
	$(MULTI_YUI) -o $@ $<

SOLITAIREY_REPO := lib/repos/Solitairey

$(DEST_lodash_Solitairey_JS): $(DEST_JS_DIR)/%: $(SOLITAIREY_REPO)/ext/lodash/%
	$(MULTI_YUI) -o $@ $<

$(DEST_yui_Solitairey_JS): $(DEST_JS_DIR)/%: $(SOLITAIREY_REPO)/ext/yui-debug/%
	$(MULTI_YUI) -o $@ $<

$(DEST_yui_min_Solitairey_JS): $(DEST_JS_DIR)/%: $(SOLITAIREY_REPO)/ext/yui/%
	$(MULTI_YUI) -o $@ $<

$(DEST_Solitairey_JS): $(DEST_JS_DIR)/%: $(SOLITAIREY_REPO)/src/js/%
	$(MULTI_YUI) -o $@ $<

$(OUT_PREF)/big-integer.js $(OUT_PREF)/flatted.js $(OUT_PREF)/qunit.js: %:
	base="$(patsubst $(OUT_PREF)/%.js,%,$@)" ; browserify -s "$$base" -r "$$base" -o $@

STRIP_TRAIL_SPACE = $(PERL) -i -lpe 's/[ \t]+$$//'

$(D) $(SUBDIRS): % :
	@if ! test -e $@ ; then mkdir $@ ; fi

RECENT_STABLE_VERSION = $(shell ./get-recent-stable-version.sh)

$(ARC_DOCS): $(D)/% : $(BASE_FC_SOLVE_SOURCE_DIR)/%.asciidoc
	$(call COPY)

$(DOCS_AUX_DIR)/$(ADOC_CSS): $(DOCS_AUX_DIR)/%: $(BASE_FC_SOLVE_SOURCE_DIR)/%
	$(call COPY)

# $(DOCS_AUX_DIR)/$(ADOC_JS): $(DOCS_AUX_DIR)/%: $(BASE_FC_SOLVE_SOURCE_DIR)/%
#	$(MULTI_YUI) -o $@ $<

$(DOCS_HTMLS): $(D)/docs/distro/% : $(BASE_FC_SOLVE_SOURCE_DIR)/%
	$(call COPY)

PROCESS_ALL_INCLUDES = ALWAYS_MIN=1 $(PERL) bin/post-incs-v2.pl --mode=minify \
   --minifier-conf=bin/html-min-cli-config-file.conf \
   --source-dir=$1 \
   --dest-dir=$1 \
   --

# jinja_rend := bin/jinja-render.py
# jinja_bases := $(shell cat lib/make/jinja.txt)
# dest_jinjas := $(patsubst %,dest/%,$(jinja_bases))
# prod_dest_jinjas := $(patsubst %,dest-prod/%,$(jinja_bases))
#
# $(dest_jinjas): $(jinja_rend) lib/template.jinja
# 	python3 $(jinja_rend)
# 	@$(call PROCESS_ALL_INCLUDES,dest) $(jinja_bases)
# 	@$(call PROCESS_ALL_INCLUDES,dest-prod) $(jinja_bases)

tt2_rend := bin/tt-render.pl
tt2_bases := $(shell cat lib/make/tt2.txt)
dest_tt2s := $(patsubst %,dest/%,$(tt2_bases))
prod_dest_tt2s := $(patsubst %,dest-prod/%,$(tt2_bases))

$(dest_tt2s) $(prod_dest_tt2s): $(tt2_rend) lib/blocks.tt2
	$(PERL) $(tt2_rend)
	$(call PROCESS_ALL_INCLUDES,dest) $(tt2_bases)
	$(call PROCESS_ALL_INCLUDES,dest-prod) $(tt2_bases)

$(IMAGES): $(D)/% : src/%
	$(call COPY)

$(RAW_SUBDIRS): $(D)/% : src/%
	rm -fr $@
	cp -r $< $@

$(LIBFREECELL_SOLVER_JS_DIR__DESTDIR_DATA): $(LIBFREECELL_SOLVER_JS_DIR__CMAKE_CACHE)
	( cd $(LIBFREECELL_SOLVER_JS_DIR__CMAKE_DIR)/Presets && make -j4 install DESTDIR=$(LIBFREECELL_SOLVER_JS_DIR__DESTDIR) )
	# We need that to make sure the timestamps are correct because apparently
	# the make install process updates the timestamps of CMakeCache.txt and
	# the Makefile there.
	find $(LIBFREECELL_SOLVER_JS_DIR__DESTDIR) -type f -print0 | xargs -0 touch

$(DEST_QSTRING_JS): lib/jquery/jquery.querystring.js
	$(MULTI_YUI) -o $@ $<

WEB_RAW_JS = global-funcs.js

DEST_WEB_RAW_JS = $(call dest_jsify,$(WEB_RAW_JS))

Phoenix_CS_BASE = jquery.phoenix.coffee
Phoenix_JS_nonmin_BASE = jquery.phoenix.js
Phoenix_JS_BASE = jquery.phoenix.min.js
Phoenix_DIR = lib/jquery/jquery-phoenix
Phoenix_JS_DEST = $(DEST_JS_DIR)/$(Phoenix_JS_BASE)

BABEL_SRC_DIR = lib/babel/js
Phoenix_JS = $(BABEL_SRC_DIR)/$(Phoenix_JS_BASE)

$(BABEL_SRC_DIR)/$(Phoenix_JS_nonmin_BASE): $(Phoenix_DIR)/src/$(Phoenix_CS_BASE)
	coffee --compile -o $(BABEL_SRC_DIR) $<

include lib/make/jquery-ui-webpack.mak

$(Phoenix_JS_DEST): $(Phoenix_JS)
	$(call COPY)

$(Phoenix_JS): $(BABEL_SRC_DIR)/$(Phoenix_JS_nonmin_BASE)
	$(MULTI_YUI) -o $@ $<

real_all: $(DEST_WEB_RAW_JS) $(Phoenix_JS_DEST) $(JQUERYUI_JS_DESTS)

$(DEST_WEB_RAW_JS): $(DEST_JS_DIR)/%: lib/web-raw-js/%
	$(MULTI_YUI) -o $@ $<

WEB_FCS_UI_JS_SOURCES = $(call dest_jsify,web-fc-solve-ui.js)

$(DEST_WEB_FC_SOLVE_UI_MIN_JS): $(WEB_FCS_UI_JS_SOURCES)
	$(MULTI_YUI) -o $@ $(WEB_FCS_UI_JS_SOURCES)

FCS_VALID_DEST = $(DEST_JS_DIR)/fcs-validate.js

TYPESCRIPT_basenames = capitalize-cards.js chart-using-flot--4fc-intractable.js chart-using-flot--int128-opt.js expand-moves-ui.js fcs-base-ui.js fcs-chart--base.js fcs-validate.js find-fc-deal-ui.js find-fc-deal.js french-cards.js generic-tests-driver.js prange.js s2ints_js.js tests/fcs-common-constants.js tests/fcs-core.js tests/fcs-purejs-rand.js tests/fcs-ui.js tests/fcs-validate.js toggle_sect.js toggler.js web-fc-solve--expand-moves--mega-test.js web-fc-solve--expand-moves.js web-fc-solve-ui.js web-fc-solve.js web-fcs-api-base.js web-fcs-purejs-rand.js web-fcs-tests-strings.js

include lib/make/sf-javascripts.mak

JSES_js_basenames = jq_qs.js libfcs-wrap.js libfind-deal-wrap.js $(Phoenix_JS_nonmin_BASE) s2i-test.js
DEST_BABEL_JSES = $(call dest_jsify,$(JSES_js_basenames) $(TYPESCRIPT_basenames))
OUT_BABEL_JSES = $(patsubst $(DEST_JS_DIR)/%,$(OUT_PREF)/%,$(DEST_BABEL_JSES))

$(patsubst %,$(OUT_PREF)/%,$(JSES_js_basenames)): $(OUT_PREF)/%.js: $(BABEL_SRC_DIR)/%.js
	babel -o $@ $<

$(DEST_BABEL_JSES): $(DEST_JS_DIR)/%.js: $(OUT_PREF)/%.js
	$(MULTI_YUI) -o $@ $<

JS_DEST_FILES__NODE = $(LIBFREECELL_SOLVER_JS__NODE__TARGETS) lib/for-node/js/libfcs-wrap.js lib/for-node/js/libfind-deal-wrap.js

$(JS_DEST_FILES__NODE): lib/for-node/%.js: $(D)/%.js
	$(call COPY)

TYPESCRIPT_COMMON_DEPS = src/ts/web-fc-solve.ts src/ts/web-fcs-tests-strings.ts

src/ts/web-fcs-tests-strings.ts: bin/gen-web-fc-solve-tests--texts-dictionary.pl lib/web-fcs-tests-strings/list.txt
	$(PERL) $<

$(TYPESCRIPT_DEST_FILES): $(OUT_PREF)/%.js: src/ts/%.ts $(TYPESCRIPT_COMMON_DEPS)
	$(call run_tsc,www)

$(TYPESCRIPT_DEST_FILES__NODE): lib/for-node/js/%.js: src/ts/%.ts $(TYPESCRIPT_COMMON_DEPS)
	$(call run_tsc,cmdline)

serial_run: tsc_www tsc_cmdline $(LIBFREECELL_SOLVER_JS) $(LIBFREECELL_SOLVER_ASMJS_STAMP)

TS_CHART_DEST = $(D)/charts/dbm-solver-__int128-optimisation/chart-using-flot.js
TS_CHART2_DEST = $(D)/charts/fc-pro--4fc-intractable-deals--report/chart-using-flot.js

ts_chart_common1 = ./src/charts/dbm-solver-__int128-optimisation/jquery.flot.d.ts

$(TS_CHART_DEST) $(TS_CHART2_DEST): $(D)/%.js: src/%.ts
	tsc --module amd --out $@ $(ts_chart_common1) $<
	$(MULTI_YUI) -o $@ $@

FC_PRO_4FC_DUMPS = $(filter charts/fc-pro--4fc-intractable-deals--report/data/%.dump.txt,$(SRC_IMAGES))
FC_PRO_4FC_TSVS = $(patsubst %.dump.txt,$(D)/%.tsv,$(FC_PRO_4FC_DUMPS))
FC_PRO_4FC_FILTERED_TSVS = $(patsubst %.dump.txt,$(D)/%.filtered.tsv,$(FC_PRO_4FC_DUMPS))

$(FC_PRO_4FC_TSVS): $(D)/%.tsv: src/%.dump.txt
	$(PERL) ../../scripts/convert-dbm-fc-solver-log-to-reduced-tsv.pl "$<" > "$@"

$(FC_PRO_4FC_FILTERED_TSVS): %.filtered.tsv : %.tsv
	$(PERL) -lan bin/filter-tsv.pl < "$<" > "$@"

$(Solver_Dest_Dir)/index.html: lib/FreecellSolver/ExtractGames.pm $(BASE_FC_SOLVE_SOURCE_DIR)/USAGE.asciidoc

FC_pro_all_TSVS := $(FC_PRO_4FC_FILTERED_TSVS) $(FC_PRO_4FC_TSVS)

chart-tsvs: $(FC_pro_all_TSVS)

$(D)/charts/fc-pro--4fc-intractable-deals--report/index.html $(D)/charts/fc-pro--4fc-deals-solvability--report/index.html: $(FC_pro_all_TSVS)

T2_SVGS__BASE := $(filter %.svg,$(IMAGES))
T2_SVGS__MIN := $(T2_SVGS__BASE:%.svg=%.min.svg)
T2_SVGS__svgz := $(T2_SVGS__BASE:%.svg=%.svgz)

$(T2_SVGS__MIN): %.min.svg: %.svg
	minify --svg-precision 5 -o $@ $<

$(T2_SVGS__svgz): %.svgz: %.min.svg
	gzip --best -n < $< > $@

min_svgs: $(T2_SVGS__MIN) $(T2_SVGS__svgz)

all_deps: $(FC_pro_all_TSVS)
all_deps: $(JQUI_webpack_dest)

real_all: all_deps min_svgs

.PHONY: chart-tsvs

ALL_HTACCESSES = $(D)/.htaccess $(D)/js-fc-solve/automated-tests/.htaccess $(D)/michael_mann/.htaccess

GEN_SECT_NAV_MENUS = ./bin/gen-sect-nav-menus.pl
T2_CACHE_ALL_STAMP = lib/cache/STAMP.one
$(T2_CACHE_ALL_STAMP): $(GEN_SECT_NAV_MENUS) $(FACTOIDS_NAV_JSON) $(ALL_SUBSECTS_DEPS)
	$(PERL) $(GEN_SECT_NAV_MENUS) $(SRC_DOCS) $(jinja_bases) $(tt2_bases)
	touch $@

make-dirs: $(D) $(SUBDIRS)

sects_cache: make-dirs $(T2_CACHE_ALL_STAMP)
htaccesses_target: $(ALL_HTACCESSES)

$(ALL_HTACCESSES): $(D)/%.htaccess: src/%my_htaccess.conf
	$(call COPY)

upload: all
	$(RSYNC) -a -l $(D)/ $(UPLOAD_URL)
	# $(RSYNC) -a -l --exclude='**/.htaccess' $(D)/ $(TEMP_UPLOAD_URL_LOCAL)
	$(RSYNC) -a -l $(D)/ $(TEMP_UPLOAD_URL_LOCAL)

upload_staging: all
	if test "$(PROD)" != "1"; then echo "use PROD=1\!" ; exit 0 ; else $(RSYNC) -a -l $(D)/ $(STAGING_UPLOAD_URL)/ ; fi

upload_beta: all
	$(RSYNC) -a -l $(D)/ $(BETA_UPLOAD_URL)

upload_temp: all
	$(RSYNC) -a -l $(D)/ $(TEMP_UPLOAD_URL)

upload_local: all
	$(RSYNC) -a $(D)/ $(BASE_LOCAL_UPLOAD_PREFIX)/$(TESTING_ENV__URL_SUFFIX)

TEST_ENV = PYTHONPATH="$${PYTHONPATH}:$(LATEMP_ABS_ROOT_SOURCE_DIR)/Tests/lib" SKIP_EMCC="$(SKIP_EMCC)"
TEST_TARGETS = Tests/*.{py,t}

clean:
	rm -f $(LIBFREECELL_SOLVER_JS_DIR)/*.{bc,js} $(LIBFREECELL_SOLVER_WASM_STAMP) $(LIBFREECELL_SOLVER_ASMJS_STAMP) $(TYPESCRIPT_DEST_FILES__NODE) $(TYPESCRIPT_DEST_FILES) $(TS_CHART_DEST)

# A temporary target to edit the active files.
edit:
	gvim -o src/ts/fcs-validate.ts src/ts/web-fc-solve-tests--fcs-validate.ts

include lib/make/sf-css.mak

real_all: $(SRC_CSS_TARGETS)

SCSS_TARGETS = $(patsubst %.css,%.scss,$(SRC_CSS_TARGETS))

$(SCSS_TARGETS): $(D)/%.scss: lib/sass/%.scss
	$(call COPY)

real_all: $(SCSS_TARGETS)

include lib/make/docbook/sf-docbook-common.mak

real_all: docbook_targets

$(DOCBOOK5_SOURCES_DIR)/fcs_arch_doc.xml: ../../arch_doc/docbook/fcs_arch_doc.xml
	$(call COPY)

$(DOCBOOK5_SOURCES_DIR)/fcs-book.xml: ../../docs/Freecell-Solver--Evolution-of-a-C-Program/text/fcs-book.xml
	$(call COPY)

fastrender: all_deps $(dest_tt2s)

DBTOEPUB := ZIPOPT="-X" $(DBTOEPUB)

$(DEST_JS_DIR)/yui-unpack: $(SOLITAIREY_REPO)/ext/yui-unpack
	rsync -a $</ $@
	rm -fr $@/yui/{api,docs,releasenotes,tests}

$(D)/js/jq.js: node_modules/jquery/dist/jquery.min.js
	$(call COPY)

real_all: $(D)/js/jq.js

$(Solver_Dest_Dir)/ChromeWebStore_Badge_v2_206x58.png $(Solver_Dest_Dir)/loading.gif: $(Solver_Dest_Dir)/%: $(SOLITAIREY_REPO)/%
	$(call COPY)

$(D)/green.jpg: $(SOLITAIREY_REPO)/green.jpg
	$(call COPY)

$(D)/green.webp: $(SOLITAIREY_REPO)/green.jpg
	gm convert $< $@

$(D)/layouts $(D)/dondorf: $(D)/%: $(SOLITAIREY_REPO)/%
	rsync -a $</ $@

$(Solver_Dest_Dir)/dondorf $(Solver_Dest_Dir)/layouts $(Solver_Dest_Dir)/js: %:
	ln -sf ../../$(notdir $@) $@

real_all: \
	$(D)/dondorf $(D)/green.jpg $(D)/green.webp $(Solver_Dest_Dir)/ChromeWebStore_Badge_v2_206x58.png $(Solver_Dest_Dir)/dondorf $(Solver_Dest_Dir)/js $(Solver_Dest_Dir)/layouts $(Solver_Dest_Dir)/loading.gif $(D)/layouts

SRC_pngs__BASE := $(filter $(D)/%.png,$(IMAGES))
SRC_pngs__webps := $(SRC_pngs__BASE:%.png=%.webp)
$(SRC_pngs__webps): %.webp: %.png
	gm convert $< $@

real_all: \
	$(DEST_BABEL_JSES) $(DEST_JS_DIR)/yui-unpack $(JS_DEST_FILES__NODE) $(OUT_BABEL_JSES) $(TYPESCRIPT_DEST_FILES) $(TYPESCRIPT_DEST_FILES__NODE)

real_all: lib/cache/epub.stamp

lib/cache/epub.stamp: $(DOCBOOK5_INSTALLED_EPUBS)
	$(PERL) bin/normalize-zips.pl $^
	touch $@

real_all: $(SRC_pngs__webps)

FAVICON := $(D)/favicon.ico
PNG_FAVICON := $(D)/favicon.png
FAVICON_SOURCE := ../../source/fc-solve.ico

$(FAVICON): $(FAVICON_SOURCE)
	$(call COPY)

$(PNG_FAVICON): $(FAVICON_SOURCE)
	gm convert $< $@
	optipng -o7 $@

real_all: $(FAVICON) $(PNG_FAVICON)

MLARCHIVE_DEST_DIR_c := $(D)/mail-lists/fc-solve-discuss
MLARCHIVE_DEST_DIR_bn := archive
MLARCHIVE_DEST_DIR := $(MLARCHIVE_DEST_DIR_c)/$(MLARCHIVE_DEST_DIR_bn)
MLARCHIVE_DEST_pivot := $(MLARCHIVE_DEST_DIR)/index.html
EMPTYFOOTER := $(PWD)/lib/emptyfooter.hyp

$(MLARCHIVE_DEST_pivot): $(PWD)/lib/data/mbox/fc-solve-discuss--yahoogroups.mbox
	mkdir -p $(MLARCHIVE_DEST_DIR_c)
	( cd $(MLARCHIVE_DEST_DIR_c) && hypermail -o ihtmlfooterfile=$(EMPTYFOOTER) -o mhtmlfooterfile=$(EMPTYFOOTER) -d $(MLARCHIVE_DEST_DIR_bn) < $< )
	$(PERL) -lp -i $(PWD)/bin/hypermail-post-proc.pl $(MLARCHIVE_DEST_DIR)/{attachment.html,author.html,date.html,index.html,subject.html}
	find $(MLARCHIVE_DEST_DIR_c) -print | xargs touch -d 2021-10-12T10:20:00Z

ml_archive: $(MLARCHIVE_DEST_pivot)

real_all: ml_archive

BROWSER_TESTS_URL = $(BROWSER_TESTS_URL__BASE)$(TEST_SITE_URL_SUFFIX)

QUNIT_PUP_copy_path = node_modules/.bin/q2.js

QUNIT_PUP = qunit-puppeteer
# We used to need that due to an outdated 'puppeteer'
# in https://github.com/shlomif/qunit-puppeteer , but we
# no longer do. Otherwise qunit-puppeteer silently did nothing.
# QUNIT_PUP = node $(QUNIT_PUP_copy_path)

CHECK_URL = python3 bin/check_url.py
AUTOMATED_TESTS_URL = $(BROWSER_TESTS_URL)/js-fc-solve/automated-tests/
GUI_TESTS_URL = $(BROWSER_TESTS_URL)/js-fc-solve/text/gui-tests.xhtml

browser-tests: all
	# cp -f node_modules/.bin/qunit-puppeteer $(QUNIT_PUP_copy_path)
	$(CHECK_URL) --match "Freecell [sS]olver" --title "Front Page" --url "$(BROWSER_TESTS_URL)"
	$(CHECK_URL) --match "Freecell [sS]olver" --title "js-fc-solve automated-tests Page" --url "$(AUTOMATED_TESTS_URL)"
	$(CHECK_URL) --match "Freecell [sS]olver" --title "js-fc-solve gui-tests Page" --url "$(GUI_TESTS_URL)"
	$(QUNIT_PUP) "$(AUTOMATED_TESTS_URL)"
	$(QUNIT_PUP) "$(GUI_TESTS_URL)"
	$(QUNIT_PUP) --browser "firefox" "$(AUTOMATED_TESTS_URL)"
	$(QUNIT_PUP) --browser "firefox" "$(GUI_TESTS_URL)"

MAKE_WITH_PROD = $(MAKE) PROD=$(PROD) LOCAL_BROWSER_TESTS=$(LOCAL_BROWSER_TESTS)

ifeq ($(SKIP),1)
	SKIP_ = true
else
	SKIP_ = false
endif

smoke-tests:
	prettier --parser typescript --arrow-parens always --tab-width 4 --trailing-comma all --write src/ts/**.ts && git add -u . && touch lib/blocks.tt2 && ($(MAKE_WITH_PROD) test || $(SKIP_)) && $(MAKE_WITH_PROD) upload_local upload_staging && $(MAKE_WITH_PROD) browser-tests

clean_js:
	rm -f $(LIBFREECELL_SOLVER_JS_DIR)/*.js $(LIBFREECELL_SOLVER_JS_DIR)/*.bc
	rm -f $(LIBFIND_DEAL_JS_DIR)/*.js $(LIBFIND_DEAL_JS_DIR)/*.bc
