
all: dummy

include rules.mak
include include.mak

WITH_DEVEL_VERSION = 1

DEVEL_VER_USE_CACHE = 1

# Toggle to generate production code with compressed and merged JS code/etc.
PROD = 0

RSYNC = rsync --progress --verbose --rsh=ssh -a --inplace

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

DOCS_PROTO = AUTHORS INSTALL NEWS README TODO USAGE
ARC_DOCS = $(patsubst %,$(D)/%,$(DOCS_PROTO))
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
	-I $${HOME}/apps/wml

JS_MEM_BASE = libfreecell-solver.js.mem
LIBFREECELL_SOLVER_JS_DIR = lib/fc-solve-for-javascript
LIBFREECELL_SOLVER_JS = $(LIBFREECELL_SOLVER_JS_DIR)/libfreecell-solver.js
DEST_LIBFREECELL_SOLVER_JS = $(D)/js/libfreecell-solver.min.js
DEST_LIBFREECELL_SOLVER_JS_NON_MIN = $(D)/js/libfreecell-solver.js
DEST_LIBFREECELL_SOLVER_JS_MEM = $(patsubst %,%/$(JS_MEM_BASE),$(D)/js $(D)/js-fc-solve/text $(D)/js-fc-solve/automated-tests)
DEST_QSTRING_JS = dest/js/jquery.querystring.js

CSS_TARGETS = $(D)/style.css $(D)/print.css $(D)/jqui-override.css $(D)/web-fc-solve.css

DEST_WEB_FC_SOLVE_UI_MIN_JS = $(D)/js/web-fcs.min.js

dummy : $(D) $(SUBDIRS) $(HTMLS) $(D)/download.html $(IMAGES) $(RAW_SUBDIRS) $(ARC_DOCS) $(INDEXES) $(DOCS_HTMLS) $(DEST_LIBFREECELL_SOLVER_JS) $(DEST_LIBFREECELL_SOLVER_JS_NON_MIN) $(DEST_QSTRING_JS) $(DEST_WEB_FC_SOLVE_UI_MIN_JS) $(CSS_TARGETS) htaccesses_target

dummy: $(DEST_LIBFREECELL_SOLVER_JS_MEM)

SASS_STYLE = compressed
# SASS_STYLE = expanded
SASS_CMD = sass --style $(SASS_STYLE)

SASS_HEADERS = lib/sass/common-style.sass

$(CSS_TARGETS): $(D)/%.css: lib/sass/%.sass $(SASS_HEADERS)
	$(SASS_CMD) $< $@

$(D) $(SUBDIRS) :: % :
	@if [ ! -e $@ ] ; then \
		mkdir $@ ; \
	fi

RECENT_STABLE_VERSION = $(shell ./get-recent-stable-version.sh)

$(ARC_DOCS) :: $(D)/% : ../../source/%.txt
	cp -f "$<" "$@"

$(DOCS_HTMLS) :: $(D)/docs/distro/% : ../../source/%
	cp -f "$<" "$@"

$(HTMLS) :: $(D)/% : src/%.wml src/.wmlrc template.wml $(INCLUDES)
	WML_LATEMP_PATH="$$(perl -MFile::Spec -e 'print File::Spec->rel2abs(shift)' '$@')" ; \
	(cd src && wml -o "$${WML_LATEMP_PATH}" $(WML_FLAGS) -DLATEMP_FILENAME="$(patsubst src/%.wml,%,$<)" $(patsubst src/%,%,$<))

$(IMAGES) :: $(D)/% : src/%
	cp -f $< $@

$(RAW_SUBDIRS) :: $(D)/% : src/%
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
	( cd $(LIBFREECELL_SOLVER_JS_DIR__CMAKE_DIR) && cmake -DCMAKE_INSTALL_PREFIX=/fc-solve $(FC_SOLVE_SOURCE_DIR) )

$(LIBFREECELL_SOLVER_JS_DIR__DESTDIR_DATA): $(LIBFREECELL_SOLVER_JS_DIR__CMAKE_CACHE)
	( cd $(LIBFREECELL_SOLVER_JS_DIR__CMAKE_DIR) && make && make install DESTDIR=$(LIBFREECELL_SOLVER_JS_DIR__DESTDIR) )
	# We need that to make sure the timestamps are correct because apparently
	# the make install process updates the timestamps of CMakeCache.txt and
	# the Makefile there.
	find $(LIBFREECELL_SOLVER_JS_DIR__DESTDIR) -type f -print0 | xargs -0 touch

$(LIBFREECELL_SOLVER_JS): $(LIBFREECELL_SOLVER_JS_DIR__DESTDIR_DATA)
	( cd $(LIBFREECELL_SOLVER_JS_DIR) && make -f $(FC_SOLVE_SOURCE_DIR)/Makefile.to-javascript.mak SRC_DIR=$(FC_SOLVE_SOURCE_DIR) CMAKE_DIR=$(FC_SOLVE_CMAKE_DIR) && perl -i -lape 's/[ \t]+$$//' *.js *.html)

clean_js:
	rm -f $(LIBFREECELL_SOLVER_JS_DIR)/*.js $(LIBFREECELL_SOLVER_JS_DIR)/*.bc

JSMIN = ./scripts/jsmin

$(JSMIN): lib/jsmin/jsmin.c
	gcc -O3 -o $@ $<

MULTI_YUI = ./scripts/Run-YUI-Compressor

$(DEST_LIBFREECELL_SOLVER_JS): $(LIBFREECELL_SOLVER_JS) $(JSMIN)
	$(JSMIN) < $(LIBFREECELL_SOLVER_JS) > $(DEST_LIBFREECELL_SOLVER_JS)

$(DEST_QSTRING_JS): lib/jquery/jquery.querystring.js
	$(MULTI_YUI) -o $@ $<


WEB_FCS_UI_JS_SOURCES = src/js/ms-rand.js src/js/gen-ms-fc-board.js src/js/web-fc-solve--expand-moves.js src/js/web-fc-solve.js src/js/web-fc-solve-ui.js

$(DEST_WEB_FC_SOLVE_UI_MIN_JS): $(WEB_FCS_UI_JS_SOURCES)
	$(MULTI_YUI) -o $@ $(WEB_FCS_UI_JS_SOURCES)

$(DEST_LIBFREECELL_SOLVER_JS_NON_MIN): $(LIBFREECELL_SOLVER_JS)
	cp -f $< $@

$(DEST_LIBFREECELL_SOLVER_JS_MEM): %: lib/fc-solve-for-javascript/libfreecell-solver.js.mem
	cp -f $< $@

.PHONY:

# Build index.html pages for the appropriate sub-directories.
$(INDEXES) :: $(D)/%/index.html : src/% gen_index.pl
	perl gen_index.pl $< $@

ALL_HTACCESSES = $(D)/.htaccess

htaccesses_target: $(ALL_HTACCESSES)

$(ALL_HTACCESSES): $(D)/%.htaccess: src/%my_htaccess.conf
	cp -f $< $@

upload: all
	$(RSYNC) $(D)/ $(UPLOAD_URL)

# upload_temp: all
#	$(RSYNC) $(TEMP_UPLOAD_URL)

upload_local: all
	$(RSYNC) $(D)/ /var/www/html/shlomif/fc-solve-temp

test: all
	prove Tests/*.t

runtest: all
	runprove Tests/*.t

clean:
	rm -f lib/fc-solve-for-javascript/*.bc lib/fc-solve-for-javascript/*.js

%.show:
	@echo "$* = $($*)"
