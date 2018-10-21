all: dummy

include lib/make/shlomif_common.mak
include lib/make/rules.mak
include lib/make/include.mak

PERL := perl
DOCMAKE ?= docmake

DOCBOOK5_XSL_STYLESHEETS_PATH := /usr/share/sgml/docbook/xsl-ns-stylesheets

DOCBOOK5_XSL_STYLESHEETS_XHTML_PATH := $(DOCBOOK5_XSL_STYLESHEETS_PATH)/xhtml-1_1
DOCBOOK5_XSL_STYLESHEETS_ONECHUNK_PATH := $(DOCBOOK5_XSL_STYLESHEETS_PATH)/onechunk
DOCBOOK5_XSL_STYLESHEETS_FO_PATH := $(DOCBOOK5_XSL_STYLESHEETS_PATH)/fo

DOCBOOK5_XSL_CUSTOM_XSLT_STYLESHEET := lib/sgml/shlomif-docbook/xsl-5-stylesheets/shlomif-essays-5-xhtml.xsl
DOCBOOK5_XSL_ONECHUNK_XSLT_STYLESHEET := lib/sgml/shlomif-docbook/xsl-5-stylesheets/shlomif-essays-5-xhtml-onechunk.xsl
DOCBOOK5_XSL_FO_XSLT_STYLESHEET := lib/sgml/shlomif-docbook/xsl-5-stylesheets/shlomif-essays-5-fo.xsl
DOCMAKE_PARAMS = -v
DOCMAKE_WITH_PARAMS = $(DOCMAKE) $(DOCMAKE_PARAMS)
# Toggle to generate production code with compressed and merged JS code/etc.
PROD = 0

SKIP_EMCC = 0

D = dest

TEMP_UPLOAD_URL = $${__HOMEPAGE_REMOTE_PATH}/fc-solve-temp
# TEMP_UPLOAD_URL = $${__HOMEPAGE_REMOTE_PATH}/fc-solve-t2
# TEMP_UPLOAD_URL = /var/www/html/shlomif/fc-solve-temp/
# TEMP_UPLOAD_URL = $${__HOMEPAGE_REMOTE_PATH}/1
UPLOAD_URL = $(TEMP_UPLOAD_URL)

ifeq ($(PROD),1)

	D = dest-prod
	WML_FLAGS += -DPRODUCTION=1
	UPLOAD_URL = hostgator:domains/fc-solve/public_html
	MULTI_YUI = ./bin/Run-YUI-Compressor
else
	MULTI_YUI = ./bin/cat-o

endif

DEST_JS_DIR = $(D)/js
BASE_FC_SOLVE_SOURCE_DIR := ../../source
IMAGES = $(addprefix $(D)/,$(SRC_IMAGES))
HTMLS = $(addprefix $(D)/,$(SRC_DOCS))

DOCS_PROTO = AUTHORS COPYING HACKING INSTALL NEWS README TODO USAGE
ADOC_CSS = asciidoctor.css
# ADOC_JS = asciidoc.js
DOCS_AUX_PROTO = $(ADOC_CSS)
DOCS_AUX_DIR = $(D)/docs/distro
ARC_DOCS = $(patsubst %,$(D)/%,$(DOCS_PROTO))
DOCS_AUX = $(patsubst %,$(DOCS_AUX_DIR)/%,$(DOCS_AUX_PROTO))
DOCS_HTMLS = $(patsubst %,$(D)/docs/distro/%.html,$(DOCS_PROTO))

SUBDIRS = $(addprefix $(D)/,$(SRC_DIRS))


WML_FLAGS += $(COMMON_PREPROC_FLAGS)

WML_FLAGS += --passoption=2,-X3074 \
			 -DLATEMP_THEME=sf.org1 \
	$(LATEMP_WML_FLAGS) --passoption=2,-I../lib/ --passoption=3,-I../lib/ \
	-I $${HOME}/apps/wml \
	--passoption=7,--skip=imgsize,summary \


JS_MEM_BASE = libfreecell-solver.wasm
JS_MEM_BASE__ASMJS = libfreecell-solver-asm.js.mem
LIBFREECELL_SOLVER_JS_DIR := lib/fc-solve-for-javascript
LIBFREECELL_SOLVER_ASMJS_JS_DIR = $(LIBFREECELL_SOLVER_JS_DIR)-asmjs
STAMP1 := $(LIBFREECELL_SOLVER_JS_DIR)/finish.stamp
STAMP2 := $(LIBFREECELL_SOLVER_ASMJS_JS_DIR)/finish.stamp
LIBFREECELL_SOLVER_JS = $(LIBFREECELL_SOLVER_JS_DIR)/libfreecell-solver.js
LIBFREECELL_SOLVER_ASMJS_JS = $(LIBFREECELL_SOLVER_ASMJS_JS_DIR)/libfreecell-solver-asm.js
DEST_LIBFREECELL_SOLVER_JS = $(DEST_JS_DIR)/libfreecell-solver.min.js
DEST_LIBFREECELL_SOLVER_ASMJS_JS = $(DEST_JS_DIR)/libfreecell-solver-asm.js
DEST_LIBFREECELL_SOLVER_JS_NON_MIN = $(DEST_JS_DIR)/libfreecell-solver.js
DEST_mem_dirs = $(DEST_JS_DIR) $(D)/js-fc-solve/find-deal $(D)/js-fc-solve/text $(D)/js-fc-solve/automated-tests lib/for-node/js/ .
DEST_LIBFREECELL_SOLVER_JS_MEM = $(patsubst %,%/$(JS_MEM_BASE),$(DEST_mem_dirs))
DEST_LIBFREECELL_SOLVER_JS_MEM__ASMJS = $(patsubst %,%/$(JS_MEM_BASE__ASMJS),$(DEST_mem_dirs))
DEST_QSTRING_JS = dest/js/jquery.querystring.js
BASE_BROWSERIFY_JS = big-integer.js qunit.js
dest_jsify = $(addprefix $(DEST_JS_DIR)/,$(1))
DEST_BROWSERIFY_JS = $(call dest_jsify,$(BASE_BROWSERIFY_JS))

CSS_TARGETS = $(D)/style.css $(D)/print.css $(D)/jqui-override.css $(D)/web-fc-solve.css

DEST_WEB_FC_SOLVE_UI_MIN_JS = $(DEST_JS_DIR)/web-fcs.min.js

FIND_INDEX__PYJS__SRC_BN = fc_solve_find_index_s2ints.py
FIND_INDEX__PYJS__pivot =  dist/fc_solve_find_index_s2ints.js

FIND_INDEX__PYJS__DEST_DIR = $(DEST_JS_DIR)
FIND_INDEX__PYJS__DEST = $(FIND_INDEX__PYJS__DEST_DIR)/$(FIND_INDEX__PYJS__pivot)
FIND_INDEX__PYJS__DEST2_DIR = $(D)/js-fc-solve/automated-tests
FIND_INDEX__PYJS__DEST2 = $(FIND_INDEX__PYJS__DEST2_DIR)/$(FIND_INDEX__PYJS__pivot)
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
	FIND_INDEX__PYJS__TARGETS = $(FIND_INDEX__PYJS__TGT) $(FIND_INDEX__PYJS__DEST) $(FIND_INDEX__PYJS__DEST2) $(FIND_INDEX__PYJS__NODE)
	LIBFREECELL_SOLVER_JS__NODE__TARGETS = lib/for-node/js/libfreecell-solver.min.js lib/for-node/js/libfreecell-solver-asm.js
	LIBFREECELL_SOLVER_JS__TARGETS = $(DEST_LIBFREECELL_SOLVER_JS) $(DEST_LIBFREECELL_SOLVER_JS_NON_MIN) $(DEST_LIBFREECELL_SOLVER_JS_MEM) $(DEST_LIBFREECELL_SOLVER_ASMJS_JS) $(DEST_LIBFREECELL_SOLVER_JS_MEM__ASMJS)
endif

include lib/make/deps.mak

dummy : $(D) $(SUBDIRS) $(HTMLS) $(D)/download.html $(IMAGES) $(RAW_SUBDIRS) $(ARC_DOCS) $(DOCS_AUX) $(DOCS_HTMLS)  $(DEST_QSTRING_JS) $(DEST_WEB_FC_SOLVE_UI_MIN_JS) $(CSS_TARGETS) htaccesses_target

dummy: $(LIBFREECELL_SOLVER_JS__TARGETS)

dummy: $(FIND_INDEX__PYJS__TARGETS)

dummy: $(DEST_BROWSERIFY_JS)

lib/docbook/5/xml/fcs_arch_doc.xml: ../../arch_doc/docbook/fcs_arch_doc.xml
	cp -f $< $@

OUT_PREF = lib/out-babel/js
OUT_BROWSERIFY_JS = $(patsubst %,$(OUT_PREF)/%,$(BASE_BROWSERIFY_JS))

$(DEST_BROWSERIFY_JS): $(DEST_JS_DIR)/%: $(OUT_PREF)/%
	$(MULTI_YUI) -o $@ $<

$(OUT_BROWSERIFY_JS): %:
	base="$(patsubst $(OUT_PREF)/%.js,%,$@)" ; browserify -s "$$base" -r "$$base" -o $@

STRIP_TRAIL_SPACE = perl -i -lpe 's/[ \t]+$$//'

$(FIND_INDEX__PYJS__TGT): $(BASE_FC_SOLVE_SOURCE_DIR)/board_gen/$(FIND_INDEX__PYJS__SRC_BN)
	cat $< lib/js-epilogue.py > $(FIND_INDEX__PYJS__PY)
	cd $(FIND_INDEX__PYJS__TGT_DIR) && python3 build.py
	perl -i -lpe 's:(MAX_SHIFTREDUCE_LOOPS\s*=\s*1000)(;):$${1}00$${2}:g' $(PIV)/*.js
	perl -i -lne 'print unless /String.prototype.strip = / .. /^\s*\}/' $(PIV)/*.js
	$(STRIP_TRAIL_SPACE) $$(find $(FIND_INDEX__PYJS__TGT_DIR) -regextype egrep -regex '.*\.(js|html)')
	touch $@

$(FIND_INDEX__PYJS__DEST) $(FIND_INDEX__PYJS__DEST2): %: $(FIND_INDEX__PYJS__TGT)
	mkdir -p $$(dirname "$@")
	$(MULTI_YUI) -o $@ $<

$(FIND_INDEX__PYJS__NODE): $(FIND_INDEX__PYJS__TGT)
	cp -f $< $@

SASS_STYLE = compressed
# SASS_STYLE = expanded
SASS_CMD = sass --style $(SASS_STYLE)

SASS_HEADERS = lib/sass/common-style.scss

$(CSS_TARGETS): $(D)/%.css: lib/sass/%.scss $(SASS_HEADERS)
	$(SASS_CMD) $< $@

$(D) $(SUBDIRS): % :
	@if [ ! -e $@ ] ; then \
		mkdir $@ ; \
	fi

RECENT_STABLE_VERSION = $(shell ./get-recent-stable-version.sh)

$(ARC_DOCS): $(D)/% : $(BASE_FC_SOLVE_SOURCE_DIR)/%.asciidoc
	cp -f "$<" "$@"

$(DOCS_AUX_DIR)/$(ADOC_CSS): $(DOCS_AUX_DIR)/%: $(BASE_FC_SOLVE_SOURCE_DIR)/%
	cp -f "$<" "$@"

# $(DOCS_AUX_DIR)/$(ADOC_JS): $(DOCS_AUX_DIR)/%: $(BASE_FC_SOLVE_SOURCE_DIR)/%
#	$(MULTI_YUI) -o $@ $<

$(DOCS_HTMLS): $(D)/docs/distro/% : $(BASE_FC_SOLVE_SOURCE_DIR)/%
	cp -f "$<" "$@"

PROCESS_ALL_INCLUDES = APPLY_ADS=1 ALWAYS_MIN=1 perl bin/post-incs.pl

$(HTMLS): $(D)/% : src/%.wml src/.wmlrc lib/template.wml
	$(call DEF_WML_PATH) (cd src && wml -o "$$fn" $(WML_FLAGS) -DLATEMP_FILENAME="$(patsubst src/%.wml,%,$<)" $(patsubst src/%,%,$<)) && $(PROCESS_ALL_INCLUDES) '$@'

$(IMAGES): $(D)/% : src/%
	cp -f $< $@

$(RAW_SUBDIRS): $(D)/% : src/%
	rm -fr $@
	cp -r $< $@

FC_SOLVE_SOURCE_DIR := $(PWD)/$(BASE_FC_SOLVE_SOURCE_DIR)

LIBFREECELL_SOLVER_JS_DIR__CMAKE_DIR = $(LIBFREECELL_SOLVER_JS_DIR)/CMAKE-BUILD-dir
LIBFREECELL_SOLVER_JS_DIR__CMAKE_CACHE = $(LIBFREECELL_SOLVER_JS_DIR__CMAKE_DIR)/CMakeCache.txt
LIBFREECELL_SOLVER_JS_DIR__DESTDIR = $(PWD)/$(LIBFREECELL_SOLVER_JS_DIR)/__DESTDIR
LIBFREECELL_SOLVER_JS_DIR__DESTDIR_DATA = $(LIBFREECELL_SOLVER_JS_DIR__DESTDIR)/fc-solve/share/freecell-solver/presetrc
LIBFREECELL_SOLVER_JS_DIR__DESTDIR__ASMJS = $(PWD)/$(LIBFREECELL_SOLVER_ASMJS_JS_DIR)/__DESTDIR
LIBFREECELL_SOLVER_JS_DIR__DESTDIR_DATA__ASMJS = $(LIBFREECELL_SOLVER_JS_DIR__DESTDIR__ASMJS)/fc-solve/share/freecell-solver/presetrc

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

$(LIBFREECELL_SOLVER_JS_DIR__DESTDIR_DATA__ASMJS): $(LIBFREECELL_SOLVER_JS_DIR__CMAKE_CACHE)
	( cd $(LIBFREECELL_SOLVER_JS_DIR__CMAKE_DIR) && make && make install DESTDIR=$(LIBFREECELL_SOLVER_JS_DIR__DESTDIR__ASMJS) )
	# We need that to make sure the timestamps are correct because apparently
	# the make install process updates the timestamps of CMakeCache.txt and
	# the Makefile there.
	find $(LIBFREECELL_SOLVER_JS_DIR__DESTDIR__ASMJS) -type f -print0 | xargs -0 touch

$(LIBFREECELL_SOLVER_JS): $(STAMP1)

$(STAMP1): $(LIBFREECELL_SOLVER_JS_DIR__DESTDIR_DATA)
	( cd $(LIBFREECELL_SOLVER_JS_DIR) && gmake -f $(FC_SOLVE_SOURCE_DIR)/../scripts/Makefile.to-javascript.mak SRC_DIR=$(FC_SOLVE_SOURCE_DIR) CMAKE_DIR=$(FC_SOLVE_CMAKE_DIR) && $(STRIP_TRAIL_SPACE) *.js *.html)
	touch $(STAMP1)

$(LIBFREECELL_SOLVER_ASMJS_JS): $(STAMP2)
$(LIBFREECELL_SOLVER_ASMJS_JS_DIR)/$(JS_MEM_BASE__ASMJS): $(STAMP2)

$(STAMP2): $(LIBFREECELL_SOLVER_JS_DIR__DESTDIR_DATA__ASMJS)
	( cd $(LIBFREECELL_SOLVER_ASMJS_JS_DIR) && gmake -f $(FC_SOLVE_SOURCE_DIR)/../scripts/Makefile.to-javascript.mak SRC_DIR=$(FC_SOLVE_SOURCE_DIR) CMAKE_DIR=$(FC_SOLVE_CMAKE_DIR) ASMJS=1 && $(STRIP_TRAIL_SPACE) *.js *.html)
	touch $@

clean_js:
	rm -f $(LIBFREECELL_SOLVER_JS_DIR)/*.js $(LIBFREECELL_SOLVER_JS_DIR)/*.bc


$(DEST_LIBFREECELL_SOLVER_JS): $(STAMP1)
	cp -f $(LIBFREECELL_SOLVER_JS) $@

$(DEST_LIBFREECELL_SOLVER_ASMJS_JS): $(STAMP2)
	cp -f  $(LIBFREECELL_SOLVER_ASMJS_JS) $@

$(DEST_QSTRING_JS): lib/jquery/jquery.querystring.js
	$(MULTI_YUI) -o $@ $<

WEB_RAW_JS = common-methods.js

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

dummy: $(Phoenix_JS_DEST)

$(Phoenix_JS_DEST): $(Phoenix_JS)
	cp -f $< $@

$(Phoenix_JS): $(BABEL_SRC_DIR)/$(Phoenix_JS_nonmin_BASE)
	$(MULTI_YUI) -o $@ $<

dummy: $(DEST_WEB_RAW_JS)

$(DEST_WEB_RAW_JS): $(DEST_JS_DIR)/%: lib/web-raw-js/%
	$(MULTI_YUI) -o $@ $<

WEB_FCS_UI_JS_SOURCES =  $(call dest_jsify,web-fc-solve-ui.js)

$(DEST_WEB_FC_SOLVE_UI_MIN_JS): $(WEB_FCS_UI_JS_SOURCES)
	$(MULTI_YUI) -o $@ $(WEB_FCS_UI_JS_SOURCES)

$(DEST_LIBFREECELL_SOLVER_JS_NON_MIN): $(LIBFREECELL_SOLVER_JS)
	cp -f $< $@

$(DEST_LIBFREECELL_SOLVER_JS_MEM): %: $(STAMP1)
	cp -f $(LIBFREECELL_SOLVER_JS_DIR)/$(JS_MEM_BASE) $@

$(DEST_LIBFREECELL_SOLVER_JS_MEM__ASMJS): %: $(STAMP2)
	cp -f $(LIBFREECELL_SOLVER_ASMJS_JS_DIR)/$(JS_MEM_BASE__ASMJS) $@

FCS_VALID_DEST = $(DEST_JS_DIR)/fcs-validate.js

TYPINGS =

TEST_FCS_VALID_BASENAME := web-fc-solve-tests--fcs-validate.js
TEST_FCS_VALID_DEST = $(DEST_JS_DIR)/$(TEST_FCS_VALID_BASENAME)

TYPESCRIPT_basenames = fcs-validate.js $(TEST_FCS_VALID_BASENAME) fcs-base-ui.js find-fc-deal-ui.js web-fc-solve.js web-fc-solve--expand-moves.js web-fc-solve-ui.js web-fcs-tests-strings.js web-fc-solve-tests.js
TYPESCRIPT_DEST_FILES = $(patsubst %.js,$(OUT_PREF)/%.js,$(TYPESCRIPT_basenames))
TYPESCRIPT_DEST_FILES__NODE = $(patsubst %.js,lib/for-node/js/%.js,$(TYPESCRIPT_basenames))
TYPESCRIPT_COMMON_DEFS_FILES = src/js/jq_qs.d.ts

JSES_js_basenames = jq_qs.js libfcs-wrap.js $(Phoenix_JS_nonmin_BASE) s2i-test.js web-fc-solve--expand-moves--mega-test.js
DEST_BABEL_JSES = $(call dest_jsify,$(JSES_js_basenames) $(TYPESCRIPT_basenames))
OUT_BABEL_JSES = $(patsubst $(DEST_JS_DIR)/%,$(OUT_PREF)/%,$(DEST_BABEL_JSES))

all: $(OUT_BABEL_JSES) $(DEST_BABEL_JSES)

$(patsubst %,$(OUT_PREF)/%,$(JSES_js_basenames)): $(OUT_PREF)/%.js: $(BABEL_SRC_DIR)/%.js
	babel -o $@ $<

$(DEST_BABEL_JSES): $(DEST_JS_DIR)/%.js: $(OUT_PREF)/%.js
	$(MULTI_YUI) -o $@ $<


JS_DEST_FILES__NODE = $(LIBFREECELL_SOLVER_JS__NODE__TARGETS) lib/for-node/js/libfcs-wrap.js

all: $(JS_DEST_FILES__NODE)

$(JS_DEST_FILES__NODE): lib/for-node/%.js: dest/%.js
	cp -f $< $@

all: $(TYPESCRIPT_DEST_FILES) $(TYPESCRIPT_DEST_FILES__NODE)

TYPESCRIPT_COMMON_DEPS = src/js/web-fc-solve.ts src/js/web-fcs-tests-strings.ts

src/js/web-fcs-tests-strings.ts: bin/gen-web-fc-solve-tests--texts-dictionary.pl lib/web-fcs-tests-strings/texts-lists.txt
	perl $<

$(TYPESCRIPT_DEST_FILES): $(OUT_PREF)/%.js: src/js/%.ts $(TYPESCRIPT_COMMON_DEPS)
	tsc --target es6 --moduleResolution node --module amd --outDir $$(dirname $@) $<

$(TYPESCRIPT_DEST_FILES__NODE): lib/for-node/%.js: src/%.ts $(TYPESCRIPT_COMMON_DEPS)
	tsc --target es6 --moduleResolution node --module commonjs --outDir lib/for-node/js $<

TS_CHART_DEST = $(D)/charts/dbm-solver-__int128-optimisation/chart-using-flot.js
TS_CHART2_DEST = $(D)/charts/fc-pro--4fc-intractable-deals--report/chart-using-flot.js

all: $(TS_CHART_DEST) $(TS_CHART2_DEST)

$(TS_CHART_DEST): $(D)/%.js: src/%.ts
	tsc --module amd --out $@ ./src/charts/dbm-solver-__int128-optimisation/jquery.flot.d.ts $<
	$(MULTI_YUI) -o $@ $@

$(TS_CHART2_DEST): $(D)/%.js: src/%.ts
	tsc --module amd --out $@ ./src/charts/dbm-solver-__int128-optimisation/jquery.flot.d.ts $<
	$(MULTI_YUI) -o $@ $@

$(TEST_FCS_VALID_DEST): $(patsubst $(D)/%.js,src/%.ts,$(FCS_VALID_DEST))

FC_PRO_4FC_DUMPS = $(filter charts/fc-pro--4fc-intractable-deals--report/data/%.dump.txt,$(SRC_IMAGES))
FC_PRO_4FC_TSVS = $(patsubst %.dump.txt,$(D)/%.tsv,$(FC_PRO_4FC_DUMPS))
FC_PRO_4FC_FILTERED_TSVS = $(patsubst %.dump.txt,$(D)/%.filtered.tsv,$(FC_PRO_4FC_DUMPS))

$(FC_PRO_4FC_TSVS): $(D)/%.tsv: src/%.dump.txt
	perl ../../scripts/convert-dbm-fc-solver-log-to-tsv.pl <(< "$<" perl -lapE 's#[^\t]*\t##') | perl -lanE 'print join"\t",@F[0,2]' > "$@"

$(FC_PRO_4FC_FILTERED_TSVS): %.filtered.tsv : %.tsv
	perl -lanE 'say if ((not /\A[0-9]/) or ($$F[0] % 1_000_000 == 0))' < "$<" > "$@"

$(D)/js-fc-solve/text/index.html: lib/FreecellSolver/ExtractGames.pm $(BASE_FC_SOLVE_SOURCE_DIR)/USAGE.asciidoc

$(D)/charts/fc-pro--4fc-intractable-deals--report/index.html $(D)/charts/fc-pro--4fc-deals-solvability--report/index.html: $(FC_PRO_4FC_FILTERED_TSVS) $(FC_PRO_4FC_TSVS)

T2_SVGS__BASE := $(filter %.svg,$(IMAGES))
T2_SVGS__MIN := $(T2_SVGS__BASE:%.svg=%.min.svg)
T2_SVGS__svgz := $(T2_SVGS__BASE:%.svg=%.svgz)

$(T2_SVGS__MIN): %.min.svg: %.svg
	minify --svg-decimals 3 -o $@ $<

$(T2_SVGS__svgz): %.svgz: %.min.svg
	gzip --best < $< > $@

min_svgs: $(T2_SVGS__MIN) $(T2_SVGS__svgz)

all: $(FC_PRO_4FC_TSVS) $(FC_PRO_4FC_FILTERED_TSVS)
all: min_svgs

.PHONY:

ALL_HTACCESSES = $(D)/.htaccess $(D)/js-fc-solve/automated-tests/.htaccess $(D)/michael_mann/.htaccess

GEN_SECT_NAV_MENUS = ./bin/gen-sect-nav-menus.pl
T2_CACHE_ALL_STAMP = lib/cache/STAMP.one
$(T2_CACHE_ALL_STAMP): $(GEN_SECT_NAV_MENUS) $(FACTOIDS_NAV_JSON) $(ALL_SUBSECTS_DEPS)
	perl $(GEN_SECT_NAV_MENUS) $(SRC_DOCS)
	touch $@

make-dirs: $(D) $(SUBDIRS)

sects_cache: make-dirs $(T2_CACHE_ALL_STAMP)
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
TEST_TARGETS = Tests/*.{py,t}

clean:
	rm -f $(LIBFREECELL_SOLVER_JS_DIR)/*.bc $(LIBFREECELL_SOLVER_JS_DIR)/*.js $(TYPESCRIPT_DEST_FILES__NODE) $(TYPESCRIPT_DEST_FILES) $(TS_CHART_DEST)

# A temporary target to edit the active files.
edit:
	gvim -o src/js/fcs-validate.ts src/js/web-fc-solve-tests--fcs-validate.ts

DOCBOOK5_BASE_DIR = lib/docbook/5
DOCBOOK5_ALL_IN_ONE_XHTML_DIR = $(DOCBOOK5_BASE_DIR)/essays
DOCBOOK5_SOURCES_DIR = $(DOCBOOK5_BASE_DIR)/xml
DOCBOOK5_EPUB_DIR = $(DOCBOOK5_BASE_DIR)/epub
DOCBOOK5_FO_DIR = $(DOCBOOK5_BASE_DIR)/fo
DOCBOOK5_PDF_DIR = $(DOCBOOK5_BASE_DIR)/pdf
DOCBOOK5_RTF_DIR = $(DOCBOOK5_BASE_DIR)/rtf
DOCBOOK5_FOR_OOO_XHTML_DIR = $(DOCBOOK5_BASE_DIR)/for-ooo-xhtml
DOCBOOK5_RENDERED_DIR = $(DOCBOOK5_BASE_DIR)/rendered

include lib/make/docbook/sf-homepage-docbooks-generated.mak

DOCBOOK5_EPUBS = $(patsubst %,$(DOCBOOK5_EPUB_DIR)/%.epub,$(filter-out hebrew-html-tutorial ,$(DOCBOOK5_DOCS)))

DOCBOOK5_FOS = $(patsubst %,$(DOCBOOK5_FO_DIR)/%.fo,$(DOCBOOK5_DOCS))

DOCBOOK5_FOR_OOO_XHTMLS = $(patsubst %,$(DOCBOOK5_FOR_OOO_XHTML_DIR)/%.html,$(DOCBOOK5_DOCS))

DOCBOOK5_PDFS = $(patsubst %,$(DOCBOOK5_PDF_DIR)/%.pdf,$(DOCBOOK5_DOCS))

DOCBOOK5_RTFS = $(patsubst %,$(DOCBOOK5_RTF_DIR)/%.rtf,$(DOCBOOK5_DOCS))

DOCBOOK5_INDIVIDUAL_XHTMLS = $(addprefix $(DOCBOOK5_INDIVIDUAL_XHTML_DIR)/,$(DOCBOOK5_DOCS))

DOCBOOK5_ALL_IN_ONE_XHTMLS__DIRS = $(patsubst %,$(DOCBOOK5_ALL_IN_ONE_XHTML_DIR)/%,$(DOCBOOK5_DOCS))
DOCBOOK5_ALL_IN_ONE_XHTMLS = $(patsubst %,$(DOCBOOK5_ALL_IN_ONE_XHTML_DIR)/%/all-in-one.xhtml,$(DOCBOOK5_DOCS))

dummy: docbook_targets

install_docbook5_epubs: make-dirs $(DOCBOOK5_INSTALLED_EPUBS)
install_docbook5_htmls: make-dirs $(DOCBOOK5_INSTALLED_HTMLS)

install_docbook4_pdfs: make-dirs $(DOCBOOK4_INSTALLED_PDFS)
install_docbook5_pdfs: make-dirs $(DOCBOOK5_INSTALLED_PDFS)

install_docbook4_xmls: make-dirs $(DOCBOOK4_INSTALLED_XMLS)
install_docbook5_xmls: make-dirs $(DOCBOOK5_INSTALLED_XMLS)

install_docbook4_rtfs: make-dirs  $(DOCBOOK4_INSTALLED_RTFS)
install_docbook5_rtfs: make-dirs  $(DOCBOOK5_INSTALLED_RTFS)

docbook_targets: docbook4_targets docbook5_targets \
	install_docbook5_epubs \
	install_docbook5_htmls \
	install_docbook4_xmls install_docbook_individual_xhtmls \
	install_docbook_css_dirs install_docbook5_xmls \

docbook_extended: $(DOCBOOK4_FOS) $(DOCBOOK4_PDFS) \
	$(DOCBOOK5_FOS) $(DOCBOOK5_PDFS) \
	install_docbook4_pdfs install_docbook4_rtfs \
	install_docbook5_pdfs install_docbook5_rtfs

docbook4_targets: $(DOCBOOK4_TARGETS) $(DOCBOOK4_ALL_IN_ONE_XHTMLS) $(DOCBOOK4_ALL_IN_ONE_XHTMLS_CSS)

docbook5_targets: $(DOCBOOK5_TARGETS) $(DOCBOOK5_ALL_IN_ONE_XHTMLS) $(DOCBOOK5_ALL_IN_ONE_XHTMLS_CSS) $(DOCBOOK5_RENDERED_HTMLS) $(DOCBOOK5_FOS) $(DOCBOOK5_FOR_OOO_XHTMLS)

$(DOCBOOK5_ALL_IN_ONE_XHTMLS): $(DOCBOOK5_ALL_IN_ONE_XHTML_DIR)/%/all-in-one.xhtml: $(DOCBOOK5_SOURCES_DIR)/%.xml
	# jing $(DOCBOOK5_RELAXNG) $<
	$(DOCMAKE) --stringparam "docbook.css.source=" --stringparam "root.filename=$@.temp.xml" --basepath $(DOCBOOK5_XSL_STYLESHEETS_PATH) -x $(DOCBOOK5_XSL_ONECHUNK_XSLT_STYLESHEET) xhtml5 $<
	xsltproc --output $@ ./bin/clean-up-docbook-xhtml-1.1.xslt $@.temp.xml.xhtml
	rm -f $@.temp.xml.xhtml
	$(PERL) -I./lib -MShlomif::DocBookClean -lpi -0777 -e 's/[ \t]+$$//gms; Shlomif::DocBookClean::cleanup_docbook(\$$_);' $@

$(DOCBOOK5_FO_DIR)/%.fo: $(DOCBOOK5_SOURCES_DIR)/%.xml
	$(DOCMAKE_WITH_PARAMS) --basepath $(DOCBOOK5_XSL_STYLESHEETS_PATH) -o $@ -x $(DOCBOOK5_XSL_FO_XSLT_STYLESHEET) fo $<
	$(PERL) -lpi -e 's/[ \t]+\z//' $@

$(DOCBOOK5_FOR_OOO_XHTML_DIR)/%.html: $(DOCBOOK5_ALL_IN_ONE_XHTML_DIR)/%/all-in-one.xhtml
	cat $< | $(PERL) -lne 's{(</title>)}{$${1}<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />}; print unless m{\A<\?xml}' > $@

EPUB_SCRIPT = $(DOCBOOK5_XSL_STYLESHEETS_PATH)/epub/bin/dbtoepub
EPUB_XSLT = lib/sgml/shlomif-docbook/docbook-epub-preproc.xslt
DBTOEPUB = ruby $(EPUB_SCRIPT)

$(DOCBOOK5_EPUBS): $(DOCBOOK5_EPUB_DIR)/%.epub: $(DOCBOOK5_XML_DIR)/%.xml
	$(DBTOEPUB) -s $(EPUB_XSLT) -o $@ $<

install_docbook_individual_xhtmls: make-dirs $(DOCBOOK4_INSTALLED_INDIVIDUAL_XHTMLS) $(DOCBOOK4_INSTALLED_INDIVIDUAL_XHTMLS_CSS) $(DOCBOOK5_INSTALLED_INDIVIDUAL_XHTMLS) $(DOCBOOK5_INSTALLED_INDIVIDUAL_XHTMLS_CSS)

install_docbook_css_dirs: make-dirs $(DOCBOOK4_INSTALLED_CSS_DIRS)
install_docbook_css_dirs: make-dirs $(DOCBOOK4_INSTALLED_CSS_DIRS)
