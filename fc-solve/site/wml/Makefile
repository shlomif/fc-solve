
WITH_DEVEL_VERSION = 1

DEVEL_VER_USE_CACHE = 1

HOSTNAME = $(shell hostname)

ifeq ($(HOSTNAME),vipe.technion.ac.il)
	VIPE = 1
endif

ifdef BERLIOS
	ifdef VIPE
		D = $(HOME)/progs/fcs/berlios-site
	else
		D = /var/www/html/fc-solve-berlios
	endif
	WML_FLAGS = -DBERLIOS=BERLIOS
else
	ifdef VIPE
		D = $(HOME)/public_html/freecell-solver/
	else
		D = /var/www/html/fc-solve
	endif
	WML_FLAGS = 
endif
#D = /home/httpd/html/ip-noise

IMAGES_PRE1 = $(shell cd src && (ls *.tar.gz *.zip *.patch *.css *.png))
IMAGES = $(addprefix $(D)/,$(IMAGES_PRE1))

# WML_FLAGS = -DBERLIOS=BERLIOS

HTMLS = $(D)/index.html $(D)/links.html $(D)/don_woods.html $(D)/features.html $(D)/old-news.html $(D)/book.html

ARC_DOCS = $(D)/README $(D)/USAGE $(D)/ChangeLog $(D)/TODO $(D)/AUTHORS

INCLUDES_PROTO = std/logo.wml
INCLUDES = $(addprefix src/,$(INCLUDES_PROTO))

SUBDIRS_WITH_INDEXES = $(D)/win32_build $(D)/win32_build/bootstrap $(D)/win32_build/dynamic $(D)/win32_build/static

SUBDIRS = $(SUBDIRS_WITH_INDEXES) $(D)/docs $(D)/arch_doc

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
	cd src && wml $(WML_FLAGS) -DFILENAME="$(notdir $@)" $(notdir $<) > $@
	
else
	RECENT_DEVEL_VERSION = ""
	DEVEL_ARC = 
endif
WML_FLAGS += --passoption=2,-X -DWITH_DEVEL_VERSION=$(WITH_DEVEL_VERSION) -DRECENT_DEVEL_VERSION=$(RECENT_DEVEL_VERSION)

dummy : $(SUBDIRS) $(HTMLS) $(D)/download.html $(IMAGES) $(RAW_SUBDIRS) $(ARC_DOCS) $(INDEXES) $(DEVEL_ARC)

$(SUBDIRS) :: % : 
	@if [ ! -e $@ ] ; then \
		mkdir $@ ; \
	fi

RECENT_STABLE_VERSION = $(shell ./get-recent-stable-version.sh)

$(ARC_DOCS) :: $(D)/% : src/freecell-solver-$(RECENT_STABLE_VERSION).tar.gz
	tar -xOzf $< freecell-solver-$(RECENT_STABLE_VERSION)/$(notdir $@) > $@

src/win32_build/dynamic/fcs.zip: src/freecell-solver-$(RECENT_STABLE_VERSION).tar.gz
	if [ ! -d t ] ; then mkdir t ; fi
	cd t && \
	tar -xzvf ../$<; \
	cd free* &&       \
	zip -r ../../$@ * ; \
	cd ../../ && \
	rm -fr t

$(HTMLS) :: $(D)/% : src/%.wml src/.wmlrc template.wml $(INCLUDES)
	cd src && wml $(WML_FLAGS) -DFILENAME="$(notdir $@)" $(notdir $<) > $@

$(IMAGES) :: $(D)/% : src/%
	cp -f $< $@

$(RAW_SUBDIRS) :: $(D)/% : src/%
	rm -fr $@
	cp -r $< $@

.PHONY: 

# Build index.html pages for the appropriate sub-directories.
$(INDEXES) :: $(D)/%/index.html : src/% gen_index.pl
	perl gen_index.pl $< $@


