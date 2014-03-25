
all: dummy

include rules.mak
include include.mak

WITH_DEVEL_VERSION = 1

DEVEL_VER_USE_CACHE = 1

RSYNC = rsync --progress --verbose --rsh=ssh -a --inplace

D = ./dest
WML_FLAGS = -DBERLIOS=BERLIOS

#D = /home/httpd/html/ip-noise

JSMIN = scripts/jsmin

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

LIBFREECELL_SOLVER_JS_DIR = lib/fc-solve-for-javascript
LIBFREECELL_SOLVER_JS = $(LIBFREECELL_SOLVER_JS_DIR)/libfreecell-solver.js
DEST_LIBFREECELL_SOLVER_JS = $(D)/js/libfreecell-solver.min.js
DEST_LIBFREECELL_SOLVER_JS_NON_MIN = $(D)/js/libfreecell-solver.js
DEST_QSTRING_JS = dest/js/jquery.querystring.js

CSS_TARGETS = $(D)/style.css $(D)/print.css $(D)/jqui-override.css $(D)/web-fc-solve.css

dummy : $(D) $(SUBDIRS) $(HTMLS) $(D)/download.html $(IMAGES) $(RAW_SUBDIRS) $(ARC_DOCS) $(INDEXES) $(DOCS_HTMLS) $(DEST_LIBFREECELL_SOLVER_JS) $(DEST_LIBFREECELL_SOLVER_JS_NON_MIN) $(DEST_QSTRING_JS) $(CSS_TARGETS) htaccesses_target

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

$(ARC_DOCS) :: $(D)/% : ../../source/%
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
	( cd $(LIBFREECELL_SOLVER_JS_DIR) && make -f $(FC_SOLVE_SOURCE_DIR)/Makefile.to-javascript.mak SRC_DIR=$(FC_SOLVE_SOURCE_DIR) )

$(JSMIN): lib/jsmin/jsmin.c
	gcc -o $@ -O2 -Wall $<

clean_js:
	rm -f $(LIBFREECELL_SOLVER_JS_DIR)/*.js $(LIBFREECELL_SOLVER_JS_DIR)/*.bc

$(DEST_LIBFREECELL_SOLVER_JS): $(LIBFREECELL_SOLVER_JS) $(JSMIN)
	$(JSMIN) < $(LIBFREECELL_SOLVER_JS) > $(DEST_LIBFREECELL_SOLVER_JS)

$(DEST_QSTRING_JS): lib/jquery/jquery.querystring.js $(JSMIN)
	$(JSMIN) < $< > $@

$(DEST_LIBFREECELL_SOLVER_JS_NON_MIN): $(LIBFREECELL_SOLVER_JS)
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
	$(RSYNC) $(D)/ hostgator:domains/fc-solve/public_html

upload_temp: all
	$(RSYNC) $(D)/ $${__HOMEPAGE_REMOTE_PATH}/fc-solve-temp

test: all
	prove Tests/*.t

runtest: all
	runprove Tests/*.t

clean:
	rm -f lib/fc-solve-for-javascript/*.bc lib/fc-solve-for-javascript/*.js
