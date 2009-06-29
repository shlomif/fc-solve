DOC = fcs_arch_doc
XML = $(DOC).xml
XHTML = $(DOC)
XHTML_INDEX = $(XHTML)/index.html
PDF = $(DOC).pdf
RTF = $(DOC).rtf
CSS = $(XHTML)/style.css

DOCMAKE = docmake -v --make 

UPLOAD_PATH = shlomif@shell.berlios.de:/home/groups/fc-solve/htdocs/arch_doc/

all: xhtml pdf rtf

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

upload:
	rsync -a -v --progress $(XHTML) $(XML) $(PDF) $(RTF) $(UPLOAD_PATH)

%.show:
	@echo "$* = $($*)"
