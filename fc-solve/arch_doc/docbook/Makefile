DOC = fcs_arch_doc
XML = $(DOC).xml
XHTML = $(DOC)
XHTML_INDEX = $(XHTML)/index.html

UPLOAD_PATH = shlomif@shell.berlios.de:/home/groups/fc-solve/htdocs/arch_doc/

all: xhtml

xhtml: $(XHTML_INDEX)

$(XHTML_INDEX): $(XML)
	docmake \
		--stringparam "docmake.output.format=xhtml" \
		--stringparam "docmake.output.work_in_progress=" \
		-x shlomif-essays-xhtml.xsl	\
		-o $(XHTML) \
		xhtml $<

upload:
	rsync -a -v --progress $(XHTML) $(XML) $(UPLOAD_PATH)

%.show:
	@echo "$* = $($*)"
