DOC=fcs_arch_doc

DB2STH_SCRIPTS_FLAGS = -c /etc/sgml/catalog

MAIN_SOURCE = $(DOC).xml
SOURCES = $(MAIN_SOURCE)

all: html
	
html: $(DOC)

$(DOC): $(SOURCES)
	db2html $(DB2STH_SCRIPTS_FLAGS) $(MAIN_SOURCE)

pdf: $(DOC).pdf

$(DOC).pdf: $(SOURCES)
	db2pdf $(DB2STH_SCRIPTS_FLAGS) $(MAIN_SOURCE)

