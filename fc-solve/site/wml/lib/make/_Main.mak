all: dummy

include lib/make/shlomif_common.mak
include lib/make/rules.mak
include lib/make/include.mak

# Toggle to generate production code with compressed and merged JS code/etc.
PROD = 0

SKIP_EMCC = 0

D = ./dest

TEMP_UPLOAD_URL = $${__HOMEPAGE_REMOTE_PATH}/fc-solve-temp
# TEMP_UPLOAD_URL = $${__HOMEPAGE_REMOTE_PATH}/1
UPLOAD_URL = $(TEMP_UPLOAD_URL)

ifeq ($(PROD),1)

	D = ./dest-prod
	WML_FLAGS += -DPRODUCTION=1
	UPLOAD_URL = hostgator:domains/fc-solve/public_html

endif

DEST_JS_DIR = $(D)/js
BASE_FC_SOLVE_SOURCE_DIR := ../../source
IMAGES = $(addprefix $(D)/,$(SRC_IMAGES))
HTMLS = $(addprefix $(D)/,$(SRC_DOCS))

DOCS_PROTO = AUTHORS COPYING INSTALL NEWS README TODO USAGE
ADOC_CSS = asciidoc.css
ADOC_JS = asciidoc.js
DOCS_AUX_PROTO = $(ADOC_CSS) $(ADOC_JS)
DOCS_AUX_DIR = $(D)/docs/distro
ARC_DOCS = $(patsubst %,$(D)/%,$(DOCS_PROTO))
DOCS_AUX = $(patsubst %,$(DOCS_AUX_DIR)/%,$(DOCS_AUX_PROTO))
DOCS_HTMLS = $(patsubst %,$(D)/docs/distro/%.html,$(DOCS_PROTO))

SUBDIRS = $(addprefix $(D)/,$(SRC_DIRS))


WML_FLAGS += $(COMMON_PREPROC_FLAGS)

WML_FLAGS += --passoption=2,-X3074 \
			 -DLATEMP_THEME=better-scm \
	$(LATEMP_WML_FLAGS) --passoption=3,-I../lib/ \
	-I $${HOME}/apps/wml \
	--passoption=7,--skip=summary \
	--passoption=7,--skip=imgsize

JS_MEM_BASE = libfreecell-solver.wasm
JS_MEM_BASE__ASMJS = libfreecell-solver-asm.js.mem
LIBFREECELL_SOLVER_JS_DIR = lib/fc-solve-for-javascript
LIBFREECELL_SOLVER_ASMJS_JS_DIR = lib/fc-solve-for-javascript-asmjs
LIBFREECELL_SOLVER_JS = $(LIBFREECELL_SOLVER_JS_DIR)/libfreecell-solver.js
LIBFREECELL_SOLVER_ASMJS_JS = $(LIBFREECELL_SOLVER_ASMJS_JS_DIR)/libfreecell-solver-asm.js
DEST_LIBFREECELL_SOLVER_JS = $(DEST_JS_DIR)/libfreecell-solver.min.js
DEST_LIBFREECELL_SOLVER_ASMJS_JS = $(DEST_JS_DIR)/libfreecell-solver-asm.js
DEST_LIBFREECELL_SOLVER_JS_NON_MIN = $(DEST_JS_DIR)/libfreecell-solver.js
DEST_mem_dirs = $(DEST_JS_DIR) $(D)/js-fc-solve/find-deal $(D)/js-fc-solve/text $(D)/js-fc-solve/automated-tests lib/for-node .
DEST_LIBFREECELL_SOLVER_JS_MEM = $(patsubst %,%/$(JS_MEM_BASE),$(DEST_mem_dirs))
DEST_LIBFREECELL_SOLVER_JS_MEM__ASMJS = $(patsubst %,%/$(JS_MEM_BASE__ASMJS),$(DEST_mem_dirs))
DEST_QSTRING_JS = dest/js/jquery.querystring.js
BASE_BIGINT_JS = big-integer.js qunit.js
dest_jsify = $(addprefix $(DEST_JS_DIR)/,$(1))
DEST_BIGINT_JS = $(call dest_jsify,$(BASE_BIGINT_JS))

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

dummy : $(D) $(SUBDIRS) $(HTMLS) $(D)/download.html $(IMAGES) $(RAW_SUBDIRS) $(ARC_DOCS) $(DOCS_AUX) $(DOCS_HTMLS)  $(DEST_QSTRING_JS) $(DEST_WEB_FC_SOLVE_UI_MIN_JS) $(CSS_TARGETS) htaccesses_target

dummy: $(LIBFREECELL_SOLVER_JS__TARGETS)

dummy: $(FIND_INDEX__PYJS__TARGETS)

dummy: $(DEST_BIGINT_JS)

OUT_PREF = lib/out-babel/js
OUT_BIGINT_JS = $(patsubst %,$(OUT_PREF)/%,$(BASE_BIGINT_JS))

$(DEST_BIGINT_JS): $(DEST_JS_DIR)/%: $(OUT_PREF)/%
	$(MULTI_YUI) -o $@ $<

$(OUT_BIGINT_JS): %:
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

SASS_HEADERS = lib/sass/common-style.sass

$(CSS_TARGETS): $(D)/%.css: lib/sass/%.sass $(SASS_HEADERS)
	$(SASS_CMD) $< $@

$(D) $(SUBDIRS): % :
	@if [ ! -e $@ ] ; then \
		mkdir $@ ; \
	fi

RECENT_STABLE_VERSION = $(shell ./get-recent-stable-version.sh)

$(ARC_DOCS): $(D)/% : $(BASE_FC_SOLVE_SOURCE_DIR)/%.txt
	cp -f "$<" "$@"

$(DOCS_AUX_DIR)/$(ADOC_CSS): $(DOCS_AUX_DIR)/%: $(BASE_FC_SOLVE_SOURCE_DIR)/%
	cp -f "$<" "$@"

$(DOCS_AUX_DIR)/$(ADOC_JS): $(DOCS_AUX_DIR)/%: $(BASE_FC_SOLVE_SOURCE_DIR)/%
	$(MULTI_YUI) -o $@ $<

$(DOCS_HTMLS): $(D)/docs/distro/% : $(BASE_FC_SOLVE_SOURCE_DIR)/%
	cat "$<" | perl -0777 -lapE 's#<table #<table summary="identifiers on the left, descriptions on the right" #g' > "$@"

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

$(LIBFREECELL_SOLVER_JS): $(LIBFREECELL_SOLVER_JS_DIR__DESTDIR_DATA)
	( cd $(LIBFREECELL_SOLVER_JS_DIR) && make -f $(FC_SOLVE_SOURCE_DIR)/../scripts/Makefile.to-javascript.mak SRC_DIR=$(FC_SOLVE_SOURCE_DIR) CMAKE_DIR=$(FC_SOLVE_CMAKE_DIR) && $(STRIP_TRAIL_SPACE) *.js *.html)

$(LIBFREECELL_SOLVER_ASMJS_JS): $(LIBFREECELL_SOLVER_JS_DIR__DESTDIR_DATA__ASMJS)
	( cd $(LIBFREECELL_SOLVER_ASMJS_JS_DIR) && make -f $(FC_SOLVE_SOURCE_DIR)/../scripts/Makefile.to-javascript.mak SRC_DIR=$(FC_SOLVE_SOURCE_DIR) CMAKE_DIR=$(FC_SOLVE_CMAKE_DIR) ASMJS=1 && $(STRIP_TRAIL_SPACE) *.js *.html)

clean_js:
	rm -f $(LIBFREECELL_SOLVER_JS_DIR)/*.js $(LIBFREECELL_SOLVER_JS_DIR)/*.bc

MULTI_YUI = ./bin/Run-YUI-Compressor

$(DEST_LIBFREECELL_SOLVER_JS): $(LIBFREECELL_SOLVER_JS)
	cp -f $< $@

$(DEST_LIBFREECELL_SOLVER_ASMJS_JS): $(LIBFREECELL_SOLVER_ASMJS_JS)
	cp -f $< $@

$(DEST_QSTRING_JS): lib/jquery/jquery.querystring.js
	$(MULTI_YUI) -o $@ $<

WEB_RAW_JS = common-methods.js

DEST_WEB_RAW_JS = $(call dest_jsify,$(WEB_RAW_JS))

Phoenix_CS_BASE = jquery.phoenix.coffee
Phoenix_JS_nonmin_BASE = jquery.phoenix.js
Phoenix_JS_BASE = jquery.phoenix.min.js
Phoenix_DIR = lib/jquery/jquery-phoenix
Phoenix_JS = $(Phoenix_DIR)/$(Phoenix_JS_BASE)
Phoenix_JS_DEST = $(DEST_JS_DIR)/$(Phoenix_JS_BASE)

BABEL_SRC_DIR = lib/babel/js

$(BABEL_SRC_DIR)/$(Phoenix_JS_nonmin_BASE): $(Phoenix_DIR)/src/$(Phoenix_CS_BASE)
	coffee --compile -o $(BABEL_SRC_DIR) $<

dummy: $(Phoenix_JS_DEST)

$(Phoenix_JS_DEST): $(Phoenix_JS)
	cp -f $< $@

dummy: $(DEST_WEB_RAW_JS)

$(DEST_WEB_RAW_JS): $(DEST_JS_DIR)/%: lib/web-raw-js/%
	$(MULTI_YUI) -o $@ $<

WEB_FCS_UI_JS_SOURCES =  $(call dest_jsify,web-fc-solve.js web-fc-solve-ui.js)

$(DEST_WEB_FC_SOLVE_UI_MIN_JS): $(WEB_FCS_UI_JS_SOURCES)
	$(MULTI_YUI) -o $@ $(WEB_FCS_UI_JS_SOURCES)

$(DEST_LIBFREECELL_SOLVER_JS_NON_MIN): $(LIBFREECELL_SOLVER_JS)
	cp -f $< $@

$(DEST_LIBFREECELL_SOLVER_JS_MEM): %: lib/fc-solve-for-javascript/$(JS_MEM_BASE)
	cp -f $< $@

$(DEST_LIBFREECELL_SOLVER_JS_MEM__ASMJS): %: $(LIBFREECELL_SOLVER_ASMJS_JS_DIR)/$(JS_MEM_BASE__ASMJS)
	cp -f $< $@

FCS_VALID_DEST = $(DEST_JS_DIR)/fcs-validate.js

TYPINGS =

DEST_BABEL_JSES = $(call dest_jsify,fcs-base-ui.js find-fc-deal-ui.js libfcs-wrap.js $(Phoenix_JS_nonmin_BASE) s2i-test.js web-fc-solve.js web-fc-solve-ui.js web-fc-solve--expand-moves.js web-fc-solve--expand-moves--mega-test.js web-fc-solve-tests.js)
OUT_BABEL_JSES = $(patsubst $(DEST_JS_DIR)/%,$(OUT_PREF)/%,$(DEST_BABEL_JSES))

all: $(DEST_BABEL_JSES)

$(OUT_BABEL_JSES): $(OUT_PREF)/%.js: $(BABEL_SRC_DIR)/%.js
	babel -o $@ $<

$(DEST_BABEL_JSES): $(DEST_JS_DIR)/%.js: $(OUT_PREF)/%.js
	$(MULTI_YUI) -o $@ $<

TEST_FCS_VALID_DEST = $(DEST_JS_DIR)/web-fc-solve-tests--fcs-validate.js

TYPESCRIPT_DEST_FILES = $(FCS_VALID_DEST) $(TEST_FCS_VALID_DEST)
TYPESCRIPT_DEST_FILES__NODE = $(patsubst $(D)/%.js,lib/for-node/%.js,$(TYPESCRIPT_DEST_FILES))
TYPESCRIPT_COMMON_DEFS_FILES =

JS_DEST_FILES__NODE = $(LIBFREECELL_SOLVER_JS__NODE__TARGETS) lib/for-node/js/libfcs-wrap.js lib/for-node/js/web-fc-solve.js lib/for-node/js/web-fc-solve--expand-moves.js lib/for-node/js/web-fc-solve-tests.js

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

$(D)/js-fc-solve/text/index.html: lib/FreecellSolver/ExtractGames.pm $(BASE_FC_SOLVE_SOURCE_DIR)/USAGE.txt

$(D)/charts/fc-pro--4fc-intractable-deals--report/index.html $(D)/charts/fc-pro--4fc-deals-solvability--report/index.html: $(FC_PRO_4FC_FILTERED_TSVS) $(FC_PRO_4FC_TSVS)

all: $(FC_PRO_4FC_TSVS) $(FC_PRO_4FC_FILTERED_TSVS)

.PHONY:

ALL_HTACCESSES = $(D)/.htaccess

GEN_SECT_NAV_MENUS = ./bin/gen-sect-nav-menus.pl
T2_CACHE_ALL_STAMP = lib/cache/STAMP.one
$(T2_CACHE_ALL_STAMP): $(GEN_SECT_NAV_MENUS) $(FACTOIDS_NAV_JSON) $(ALL_SUBSECTS_DEPS)
	perl $(GEN_SECT_NAV_MENUS) $(SRC_DOCS)
	touch $@

make-dirs:
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

clean:
	rm -f lib/fc-solve-for-javascript/*.bc lib/fc-solve-for-javascript/*.js $(TYPESCRIPT_DEST_FILES__NODE) $(TYPESCRIPT_DEST_FILES) $(TS_CHART_DEST)

# A temporary target to edit the active files.
edit:
	gvim -o src/js/fcs-validate.ts src/js/web-fc-solve-tests--fcs-validate.ts
