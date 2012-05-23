
WITH_DEVEL_VERSION = 1

DEVEL_VER_USE_CACHE = 1

RSYNC = rsync --progress --verbose --rsh=ssh -a --inplace

D = ./dest
WML_FLAGS = -DBERLIOS=BERLIOS

#D = /home/httpd/html/ip-noise

IMAGES_PRE1 = $(shell cd src && (ls *.tar.gz *.zip *.patch *.css pysol/*.tar.gz pysol/*.diff docs/*.txt images/*.png images/*.gif js/*.js downloads/*/*.xz))
IMAGES = $(addprefix $(D)/,$(IMAGES_PRE1))

# WML_FLAGS = -DBERLIOS=BERLIOS

HTMLS = $(D)/code-of-conduct/index.html $(D)/index.html $(D)/links.html $(D)/download.html $(D)/support.html $(D)/don_woods.html $(D)/features.html $(D)/old-news.html $(D)/book.html $(D)/docs/index.html $(D)/docs/Well-and-Wall.html $(D)/arch_doc/index.html $(D)/to-do.html $(D)/pysol/index.html $(D)/current-status.html $(D)/getting-credit.html $(D)/verify-code/index.html

TTMLS = $(D)/style.css

DOCS_PROTO = AUTHORS INSTALL NEWS README TODO USAGE   
ARC_DOCS = $(patsubst %,$(D)/%,$(DOCS_PROTO))
DOCS_HTMLS = $(patsubst %,$(D)/docs/distro/%.html,$(DOCS_PROTO))

INCLUDES_PROTO = std/logo.wml
INCLUDES = $(addprefix src/,$(INCLUDES_PROTO))

# Remming out because it confuses the validator and no longer needed because
# the web-server now supports indexes.
# SUBDIRS_WITH_INDEXES = $(WIN32_BUILD_SUBDIRS)
#
SUBDIRS = $(D) $(D)/code-of-conduct $(D)/docs $(D)/docs/distro $(D)/downloads $(D)/downloads/dbm-fc-solver $(D)/arch_doc $(D)/js $(D)/images $(D)/pysol $(D)/verify-code/

INDEXES = $(addsuffix /index.html,$(SUBDIRS_WITH_INDEXES))

all: dummy

COMMON_PREPROC_FLAGS = -I $$HOME/conf/wml/Latemp/lib -I../lib
LATEMP_WML_FLAGS =$(shell latemp-config --wml-flags) 

TTML_FLAGS += $(COMMON_PREPROC_FLAGS)
WML_FLAGS += $(COMMON_PREPROC_FLAGS)

WML_FLAGS += --passoption=2,-X3074 \
	-DLATEMP_SERVER=fc-solve -DLATEMP_THEME=better-scm \
	$(LATEMP_WML_FLAGS) --passoption=3,-I../lib/ \
	-I $${HOME}/apps/wml

dummy : $(SUBDIRS) $(HTMLS) $(D)/download.html $(TTMLS) $(IMAGES) $(RAW_SUBDIRS) $(ARC_DOCS) $(INDEXES) $(DOCS_HTMLS)

$(SUBDIRS) :: % : 
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

$(TTMLS) :: $(D)/% : src/%.ttml
	ttml -o $@ $(TTML_FLAGS) -DLATEMP_FILENAME=$(patsubst $(D)/%.ttml,%,$@) $<	

.PHONY: 

# Build index.html pages for the appropriate sub-directories.
$(INDEXES) :: $(D)/%/index.html : src/% gen_index.pl
	perl gen_index.pl $< $@

upload: all
	$(RSYNC) $(D)/ hostgator:domains/fc-solve/public_html

upload_temp: all
	$(RSYNC) $(D)/ $${__HOMEPAGE_REMOTE_PATH}/fc-solve-temp
