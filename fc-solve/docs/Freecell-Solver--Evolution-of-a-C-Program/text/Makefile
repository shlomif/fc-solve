BASE = FCS_book

DEST_DIR = $(BASE)

XML_SOURCES = fcs-book.xml rules.xml first_arch.xml states_collection.xml

all: $(DEST_DIR)/index.html $(DEST_DIR)/book.css

html: $(BASE).zip

$(DEST_DIR)/index.html: $(XML_SOURCES)
	xmlto html -o FCS_book -m glossterm.xsl -m ./filename.xsl -m  ./admon.xsl \
		-m ./css.xsl  \
		./fcs-book.xml

$(DEST_DIR)/book.css : book.css
	cp -f $< $@

$(BASE).zip: $(DEST_DIR)/index.html
	rm -f $@
	zip -r $@ $(DEST_DIR)

#fcs-book.pdf: $(XML_SOURCES)
#	xmlto pdf -v --extensions -m glossterm.xsl $<

pdf: fcs-book.pdf

fcs-book.pdf: $(XML_SOURCES)
	docmake -o $@ pdf $<
