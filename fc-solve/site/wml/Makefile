
D = /var/www/html/fc-solve
#D = /home/httpd/html/ip-noise

IMAGES = $(D)/style.css                                             \
         $(D)/docs/Arbitrator_Architecture.txt                      \
         $(D)/docs/Architecture.txt                                 \
		 $(D)/docs/Final2.ppt                                       \
		 $(D)/docs/IP-Noise-Gen-Final.doc                           \
		 $(D)/docs/IP-Noise-Gen-Final.pdf                           \
         $(D)/docs/IP-Noise-Generator-Mid-Term.doc                  \
         $(D)/docs/ip_noise.spec.zip                                \
         $(D)/docs/Mid-Term.ppt                                     \
         $(D)/docs/Protocol.txt                                     \
         $(D)/docs/Syntax_Description.txt                           \
         $(D)/docs/roadmaps/2001-08-22.txt                          \
         $(D)/docs/roadmaps/2001-09-11.txt                          \
         $(D)/docs/Users_Guide/index.html                           \
         $(D)/docs/Users_Guide/markov_chain1.png                    \
         $(D)/download/IP-Noise-C-Arbitrator-0.1.1.tar.gz           \
         $(D)/download/IP-Noise-C-Arbitrator-0.1.2.tar.gz           \
         $(D)/download/IP-Noise-C-Arbitrator-0.1.3.tar.gz           \
         $(D)/download/IP-Noise-C-Arbitrator-0.1.6.tar.gz           \
         $(D)/download/IP-Noise-C-Arbitrator-0.1.7.tar.gz           \
         $(D)/download/IP-Noise-C-Arbitrator-0.1.8.tar.gz           \
         $(D)/download/IP-Noise-Compiler-2001-08-28-rev1.tar.gz     \
         $(D)/download/IP-Noise-Compiler-0.1.2.tar.gz               \
         $(D)/download/IP-Noise-Compiler-0.1.3.tar.gz               \
         $(D)/download/IP-Noise-Compiler-0.1.4.tar.gz               \
         $(D)/download/IP-Noise-Compiler-0.1.6.tar.gz               \
         $(D)/download/IP-Noise-Compiler-0.1.7.tar.gz               \
         $(D)/download/IP-Noise-Engine-perl-0.1.0.tar.gz            \
         $(D)/download/IP-Noise-Engine-perl-0.1.1.tar.gz            \
         $(D)/download/IP-Noise-Engine-perl-0.1.2.tar.gz            \
         $(D)/download/IP-Noise-Engine-perl-0.1.4.tar.gz            \
         $(D)/download/IP-Noise-Engine-perl-0.1.5.tar.gz            \
         $(D)/download/IP-Noise-Perl-Arbitrator-0.1.2.tar.gz        \
         $(D)/download/IP-Noise-Perl-Arbitrator-0.1.3.tar.gz        \
         $(D)/download/IP-Noise-Perl-Arbitrator-0.1.4.tar.gz        \
         $(D)/download/IP-Noise-Perl-Arbitrator-0.1.5.tar.gz        \
         $(D)/download/IP-Noise-perl-filters-0.1.0.tar.gz           \
         


SUBDIRS = $(D)/download $(D)/docs $(D)/docs/roadmaps $(D)/docs/Users_Guide $(D)/mailing-list

HTMLS = $(D)/index.html $(D)/berlios.html $(D)/download/index.html $(D)/links.html \
	$(D)/docs/index.html $(D)/mailing-list/index.html

RAW_SUBDIRS = $(D)/docs/Mid-Term-html $(D)/docs/Final-html $(D)/docs/Final-Report-HTML

all : $(SUBDIRS) $(HTMLS) $(IMAGES) $(RAW_SUBDIRS)

$(SUBDIRS) :: % : 
	@if [ ! -e $@ ] ; then \
		mkdir $@ ; \
	fi
	

$(HTMLS) :: $(D)/% : %.wml .wmlrc template.wml
	wml $< > $@

$(IMAGES) :: $(D)/% : %
	cp -f $< $@

$(RAW_SUBDIRS) :: $(D)/% : %
	rm -fr $@
	cp -r $< $@
