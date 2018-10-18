DOCBOOK5_XSL_STYLESHEETS_PATH := /usr/share/sgml/docbook/xsl-ns-stylesheets

DOC = fcs_arch_doc
XML = $(DOC).xml
XHTML = $(DOC)
XHTML_all_in_one = $(DOC)-all-on-one-page
XHTML_INDEX = $(XHTML)/index.html
XHTML_all_in_one_INDEX = $(XHTML_all_in_one)/index.xhtml
EPUB = $(DOC).epub
PDF = $(DOC).pdf
RTF = $(DOC).rtf
CSS = $(XHTML)/style.css

EPUB_SCRIPT = $(DOCBOOK5_XSL_STYLESHEETS_PATH)/epub/bin/dbtoepub
EPUB_XSLT = lib/sgml/shlomif-docbook/docbook-epub-preproc.xslt
DBTOEPUB = ruby $(EPUB_SCRIPT)

DOCMAKE = docmake -v --make

UPLOAD_PATH = hostgator:domains/fc-solve/public_html/arch_doc/

all: xhtml pdf rtf all_in_one epub

all_in_one: $(XHTML_all_in_one_INDEX)

xhtml: $(XHTML_INDEX) $(CSS)

$(XHTML_all_in_one_INDEX): $(XML)
	$(DOCMAKE) \
		--trailing-slash=0 \
		--stringparam "docmake.output.format=xhtml" \
		--stringparam "docmake.output.work_in_progress=" \
		-x shlomif-essays-5-xhtml-onechunk.xsl	\
		-o $(XHTML_all_in_one)/notneeded.x \
		xhtml $<

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

upload: all
	rsync -a -v --progress $(EPUB) $(XHTML) $(XHTML_all_in_one) $(XML) $(PDF) $(RTF) $(UPLOAD_PATH)

%.show:
	@echo "$* = $($*)"
