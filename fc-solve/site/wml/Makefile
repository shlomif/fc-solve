
D = /var/www/html/fc-solve
#D = /home/httpd/html/ip-noise

IMAGES_PRE1 = $(shell cd src && (ls *.tar.gz *.zip))
IMAGES = $(addprefix $(D)/,$(IMAGES_PRE1))


SUBDIRS = $(D)/download $(D)/docs $(D)/docs/roadmaps $(D)/docs/Users_Guide $(D)/mailing-list

HTMLS = $(D)/index.html $(D)/download.html $(D)/links.html $(D)/don_woods.html

all : $(SUBDIRS) $(HTMLS) $(IMAGES) $(RAW_SUBDIRS)

$(SUBDIRS) :: % : 
	@if [ ! -e $@ ] ; then \
		mkdir $@ ; \
	fi
	

$(HTMLS) :: $(D)/% : src/%.wml .wmlrc template.wml
	wml $< > $@

$(IMAGES) :: $(D)/% : src/%
	cp -f $< $@

$(RAW_SUBDIRS) :: $(D)/% : %
	rm -fr $@
	cp -r $< $@
