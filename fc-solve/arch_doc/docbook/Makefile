DOCBOOK5_XSL_STYLESHEETS_PATH := /usr/share/sgml/docbook/xsl-ns-stylesheets

DOC = fcs_arch_doc
XML = $(DOC).xml
XHTML = $(DOC)
XHTML_INDEX = $(XHTML)/index.html
EPUB = $(DOC).epub
PDF = $(DOC).pdf
RTF = $(DOC).rtf
CSS = $(XHTML)/style.css

EPUB_SCRIPT = $(DOCBOOK5_XSL_STYLESHEETS_PATH)/epub/bin/dbtoepub
EPUB_XSLT = lib/sgml/shlomif-docbook/docbook-epub-preproc.xslt
DBTOEPUB = ruby $(EPUB_SCRIPT)

DOCMAKE = docmake -v --make

UPLOAD_PATH = hostgator:domains/fc-solve/public_html/arch_doc/

all: xhtml pdf rtf epub

xhtml: $(XHTML_INDEX) $(CSS)

$(XHTML_INDEX): $(XML)
	$(DOCMAKE) \
		--stringparam "docmake.output.format=xhtml" \
		--stringparam "docmake.output.work_in_progress=" \
		-x shlomif-essays-xhtml.xsl	\
		-o $(XHTML) \
		xhtml $<

$(CSS): style.css $(XHTML_INDEX)
	cp -f $< $@

pdf: $(PDF)

$(PDF): $(XML)
	$(DOCMAKE) \
		--stringparam "docmake.output.format=fo" \
		-x shlomif-essays-fo.xsl \
		-o $@ \
		pdf $<

rtf: $(RTF)

$(RTF): $(XML)
	$(DOCMAKE) \
		--stringparam "docmake.output.format=fo" \
		-x shlomif-essays-fo.xsl \
		-o $@ \
		rtf $<

epub: $(EPUB)

$(EPUB): $(XML)
	$(DBTOEPUB) -s $(EPUB_XSLT) -o $@ $<

upload:
	rsync -a -v --progress $(XHTML) $(XML) $(PDF) $(RTF) $(UPLOAD_PATH)

%.show:
	@echo "$* = $($*)"
