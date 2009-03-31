DOC = fcs_arch_doc
XML = $(DOC).xml
XHTML = $(DOC)
XHTML_INDEX = $(XHTML)/index.html
PDF = $(DOC).pdf

UPLOAD_PATH = shlomif@shell.berlios.de:/home/groups/fc-solve/htdocs/arch_doc/

all: xhtml pdf

xhtml: $(XHTML_INDEX)

$(XHTML_INDEX): $(XML)
	docmake -v \
		--stringparam "docmake.output.format=xhtml" \
		--stringparam "docmake.output.work_in_progress=" \
		-x shlomif-essays-xhtml.xsl	\
		-o $(XHTML) \
		xhtml $<

pdf: $(PDF)

$(PDF): $(XML)
	docmake -v \
		--stringparam "docmake.output.format=fo" \
		-x shlomif-essays-fo.xsl \
		-o $@ \
		pdf $<

upload:
	rsync -a -v --progress $(XHTML) $(XML) $(PDF) $(UPLOAD_PATH)

%.show:
	@echo "$* = $($*)"
