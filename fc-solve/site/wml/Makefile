
WITH_DEVEL_VERSION = 1

DEVEL_VER_USE_CACHE = 1

HOSTNAME = $(shell hostname)

ifeq ($(HOSTNAME),vipe.technion.ac.il)
	VIPE = 1
endif

ifdef BERLIOS
    D = ./dest-berlios
	WML_FLAGS = -DBERLIOS=BERLIOS
else
	D = ./dest
	WML_FLAGS = 
endif
#D = /home/httpd/html/ip-noise

IMAGES_PRE1 = $(shell cd src && (ls *.tar.gz *.zip *.patch *.css pysol/*.tar.gz pysol/*.diff docs/*.txt images/*.png images/*.gif))
IMAGES = $(addprefix $(D)/,$(IMAGES_PRE1))

# WML_FLAGS = -DBERLIOS=BERLIOS

HTMLS = $(D)/index.html $(D)/links.html $(D)/don_woods.html $(D)/features.html $(D)/old-news.html $(D)/book.html $(D)/docs/index.html $(D)/arch_doc/index.html $(D)/to-do.html $(D)/pysol/index.html $(D)/current-status.html $(D)/getting-credit.html

TTMLS = $(D)/style.css

ARC_DOCS = $(D)/README $(D)/USAGE $(D)/ChangeLog $(D)/TODO $(D)/AUTHORS

INCLUDES_PROTO = std/logo.wml
INCLUDES = $(addprefix src/,$(INCLUDES_PROTO))

SUBDIRS_WITH_INDEXES = $(D)/win32_build $(D)/win32_build/bootstrap $(D)/win32_build/dynamic $(D)/win32_build/static

SUBDIRS = $(D) $(SUBDIRS_WITH_INDEXES) $(D)/docs $(D)/arch_doc $(D)/pysol $(D)/images

INDEXES = $(addsuffix /index.html,$(SUBDIRS_WITH_INDEXES))

IMAGES += $(addprefix $(D)/win32_build/,bootstrap/curl.exe bootstrap/build.bat static/zip.exe static/unzip.exe dynamic/fcs.zip)

all: dummy

ifneq ($(WITH_DEVEL_VERSION),0)

ifeq ($(DEVEL_VER_USE_CACHE),0)
RECENT_DEVEL_VERSION = $(shell ./get-recent-devel-version.sh)
else
RECENT_DEVEL_VERSION = $(shell cat t/fc-solve/source/ver.txt)
endif

DEVEL_ARC = $(D)/freecell-solver-$(RECENT_DEVEL_VERSION).tar.gz

$(DEVEL_ARC) : 
	./put-recent-devel-version.sh $(DEVEL_ARC)

$(D)/download.html :: $(D)/% : src/%.wml src/.wmlrc template.wml $(INCLUDES) $(DEVEL_ARC)
	(cd src && wml $(WML_FLAGS) -DLATEMP_FILENAME="$(notdir $@)" $(notdir $<)) > $@
	
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

dummy : $(SUBDIRS) $(HTMLS) $(D)/download.html $(TTMLS) $(IMAGES) $(RAW_SUBDIRS) $(ARC_DOCS) $(INDEXES) $(DEVEL_ARC) 

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
	(cd src && wml $(WML_FLAGS) -DLATEMP_FILENAME="$(patsubst src/%.wml,%,$<)" $(patsubst src/%,%,$<)) > $@

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


