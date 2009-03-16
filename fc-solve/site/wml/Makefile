
WITH_DEVEL_VERSION = 1

DEVEL_VER_USE_CACHE = 1

RSYNC = rsync --progress --verbose --rsh=ssh -r

D = ./dest
WML_FLAGS = -DBERLIOS=BERLIOS

#D = /home/httpd/html/ip-noise

IMAGES_PRE1 = $(shell cd src && (ls *.tar.gz *.zip *.patch *.css pysol/*.tar.gz pysol/*.diff docs/*.txt images/*.png images/*.gif js/*.js))
IMAGES = $(addprefix $(D)/,$(IMAGES_PRE1))

# WML_FLAGS = -DBERLIOS=BERLIOS

HTMLS = $(D)/index.html $(D)/links.html $(D)/don_woods.html $(D)/features.html $(D)/old-news.html $(D)/book.html $(D)/docs/index.html $(D)/arch_doc/index.html $(D)/to-do.html $(D)/pysol/index.html $(D)/current-status.html $(D)/getting-credit.html $(D)/verify-code/index.html

TTMLS = $(D)/style.css

ARC_DOCS = $(D)/README $(D)/USAGE $(D)/ChangeLog $(D)/TODO $(D)/AUTHORS

INCLUDES_PROTO = std/logo.wml
INCLUDES = $(addprefix src/,$(INCLUDES_PROTO))

# Remming out because it confuses the validator and no longer needed because
# the web-server now supports indexes.
# SUBDIRS_WITH_INDEXES = $(WIN32_BUILD_SUBDIRS)
#
WIN32_BUILD_SUBDIRS = $(D)/win32_build $(D)/win32_build/bootstrap $(D)/win32_build/dynamic $(D)/win32_build/static

SUBDIRS = $(D) $(WIN32_BUILD_SUBDIRS) $(D)/docs $(D)/arch_doc $(D)/js $(D)/images $(D)/pysol $(D)/verify-code/

INDEXES = $(addsuffix /index.html,$(SUBDIRS_WITH_INDEXES))

IMAGES += $(addprefix $(D)/win32_build/,bootstrap/curl.exe bootstrap/build.bat static/zip.exe static/unzip.exe dynamic/fcs.zip)

all: dummy

ifneq ($(WITH_DEVEL_VERSION),0)

ifeq ($(DEVEL_VER_USE_CACHE),0)
RECENT_DEVEL_VERSION = $(shell ./get-recent-devel-version.sh)
else
RECENT_DEVEL_VERSION = $(shell cat Source-Devel/fc-solve/source/ver.txt)
endif

RECENT_DEVEL_VERSION = 2.11.10

$(D)/download.html :: $(D)/% : src/%.wml src/.wmlrc template.wml $(INCLUDES)
	WML_LATEMP_PATH="$$(perl -MFile::Spec -e 'print File::Spec->rel2abs(shift)' '$@')" ; \
	(cd src && wml -o "$${WML_LATEMP_PATH}" $(WML_FLAGS) -DLATEMP_FILENAME="$(patsubst src/%.wml,%,$<)" $(patsubst src/%,%,$<))
	
else
	RECENT_DEVEL_VERSION = ""
	DEVEL_ARC = 
endif

COMMON_PREPROC_FLAGS = -I $$HOME/conf/wml/Latemp/lib
LATEMP_WML_FLAGS =$(shell latemp-config --wml-flags) 

TTML_FLAGS += $(COMMON_PREPROC_FLAGS)
WML_FLAGS += $(COMMON_PREPROC_FLAGS)

WML_FLAGS += --passoption=2,-X3074 \
	-DWITH_DEVEL_VERSION=$(WITH_DEVEL_VERSION) \
	-DRECENT_DEVEL_VERSION=$(RECENT_DEVEL_VERSION) \
	-DLATEMP_SERVER=fc-solve -DLATEMP_THEME=better-scm \
	$(LATEMP_WML_FLAGS) --passoption=3,-I../lib/ \
	-I $${HOME}/apps/wml

dummy : $(SUBDIRS) $(HTMLS) $(D)/download.html $(TTMLS) $(IMAGES) $(RAW_SUBDIRS) $(ARC_DOCS) $(INDEXES)

$(SUBDIRS) :: % : 
	@if [ ! -e $@ ] ; then \
		mkdir $@ ; \
	fi

RECENT_STABLE_VERSION = $(shell ./get-recent-stable-version.sh)

STABLE_VERSION_DIR = $(shell bash scripts/get-stable-version-dir.sh)

$(ARC_DOCS) :: $(D)/% : $(STABLE_VERSION_DIR)/%
	cp -f "$<" "$@"

src/win32_build/dynamic/fcs.zip: $(STABLE_VERSION_DIR)
	bash scripts/get-fcs-zip.bash

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

upload:
	cd $(D) && $(RSYNC) * shlomif@shell.berlios.de:/home/groups/fc-solve/htdocs/
