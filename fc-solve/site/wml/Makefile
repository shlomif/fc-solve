
D = /var/www/html/fc-solve
#D = /home/httpd/html/ip-noise

IMAGES_PRE1 = $(shell cd src && (ls *.tar.gz *.zip *.patch))
IMAGES = $(addprefix $(D)/,$(IMAGES_PRE1))

#WML_FLAGS = -DBERLIOS
WML_FLAGS = 

SUBDIRS = $(D)/download $(D)/docs $(D)/docs/roadmaps $(D)/docs/Users_Guide $(D)/mailing-list

HTMLS = $(D)/index.html $(D)/download.html $(D)/links.html $(D)/don_woods.html $(D)/features.html

all : $(SUBDIRS) $(HTMLS) $(IMAGES) $(RAW_SUBDIRS)

$(SUBDIRS) :: % : 
	@if [ ! -e $@ ] ; then \
		mkdir $@ ; \
	fi
	

$(HTMLS) :: $(D)/% : src/%.wml .wmlrc template.wml
	wml $(WML_FLAGS) $< > $@

$(IMAGES) :: $(D)/% : src/%
	cp -f $< $@

$(RAW_SUBDIRS) :: $(D)/% : %
	rm -fr $@
	cp -r $< $@
