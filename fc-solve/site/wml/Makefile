

ifdef BERLIOS
	D = /var/www/html/fc-solve-berlios
	WML_FLAGS = -DBERLIOS=BERLIOS
else
	D = /var/www/html/fc-solve
	WML_FLAGS = 
endif
#D = /home/httpd/html/ip-noise

IMAGES_PRE1 = $(shell cd src && (ls *.tar.gz *.zip *.patch))
IMAGES = $(addprefix $(D)/,$(IMAGES_PRE1))

# WML_FLAGS = -DBERLIOS=BERLIOS


HTMLS = $(D)/index.html $(D)/download.html $(D)/links.html $(D)/don_woods.html $(D)/features.html $(D)/old-news.html

all : $(SUBDIRS) $(HTMLS) $(IMAGES) $(RAW_SUBDIRS)

$(SUBDIRS) :: % : 
	@if [ ! -e $@ ] ; then \
		mkdir $@ ; \
	fi
	

$(HTMLS) :: $(D)/% : src/%.wml .wmlrc template.wml Makefile
	wml $(WML_FLAGS) $< > $@

$(IMAGES) :: $(D)/% : src/%
	cp -f $< $@

$(RAW_SUBDIRS) :: $(D)/% : %
	rm -fr $@
	cp -r $< $@
