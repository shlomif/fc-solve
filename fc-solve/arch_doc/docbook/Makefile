DOC=fcs_arch_doc

DB2STH_SCRIPTS_FLAGS = -c /etc/sgml/catalog

MAIN_SOURCE = $(DOC).xml
SOURCES = $(MAIN_SOURCE) coding_conventions.xml code_overview.xml techniques.xml

RSYNC = rsync --progress --verbose --rsh=ssh

SRC_ARCHIVE = $(DOC)-docbook-source.zip


all: html

arc : $(SRC_ARCHIVE)
	
$(SRC_ARCHIVE) : $(SOURCES) Makefile
	zip -u $@ $(SOURCES) Makefile

html: $(DOC)

$(DOC): $(SOURCES)
	db2html $(DB2STH_SCRIPTS_FLAGS) $(MAIN_SOURCE)

pdf: $(DOC).pdf

$(DOC).pdf: $(SOURCES)
	db2pdf $(DB2STH_SCRIPTS_FLAGS) $(MAIN_SOURCE)

rtf: $(DOC).rtf

$(DOC).rtf: $(SOURCES)
	db2rtf $(DB2STH_SCRIPTS_FLAGS) $(MAIN_SOURCE)

upload: html pdf arc rtf
	$(RSYNC) -r $(DOC) $(DOC).pdf $(DOC).rtf $(SRC_ARCHIVE) shlomif@shell.berlios.de:/home/groups/fc-solve/htdocs/arch_doc/


