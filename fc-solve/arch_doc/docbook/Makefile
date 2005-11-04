DOC = fcs_arch_doc

# DB2STH_SCRIPTS_FLAGS = -c /etc/sgml/catalog
DB2STH_SCRIPTS_ALL_FLAGS = -c /etc/sgml/catalog 

DB2STH_SCRIPTS_PRINT_FLAGS = $(DB2STH_SCRIPTS_ALL_FLAGS) -d './mystyle.dsl\#print'

DB2STH_SCRIPTS_HTML_FLAGS = $(DB2STH_SCRIPTS_ALL_FLAGS) -d './mystyle.dsl\#html'

MAIN_SOURCE = $(DOC).xml

COMMON_SOURCES = $(MAIN_SOURCE) Makefile code_overview.xml \
				 coding_conventions.xml techniques.xml scm.xml terms.xml

DSSSL_SPECIFIC_SOURCES = mystyle.dsl jadetex.cfg 
DSSSL_SOURCES =  $(COMMON_SOURCES) $(DSSSL_SPECIFIC_SOURCES)
XSL_SPECIFIC_SOURCES = tej.xsl
OTHER_SOURCES = style.css
XSL_SOURCES = $(COMMON_SOURCES) $(XSL_SPECIFIC_SOURCES)
ALL_SOURCES = $(COMMON_SOURCES) $(DSSSL_SPECIFIC_SOURCES) $(XSL_SPECIFIC_SOURCES) $(OTHER_SOURCES)

RSYNC = rsync --progress --verbose --rsh=ssh

SRC_ARCHIVE = $(DOC)-docbook-source.zip


all: html

arc : $(SRC_ARCHIVE)
	
$(SRC_ARCHIVE) : $(DSSSL_SOURCES) Makefile
	zip -u $@ $(DSSSL_SOURCES) Makefile

HTML_XSL_TARGET = $(DOC)

html: html-xsl

HTML_DSSSL_TARGET = $(DOC)-dsssl

html-dsssl: $(HTML_DSSSL_TARGET)

HTML_XSL_TARGET_FILE_IN_DIR = $(HTML_XSL_TARGET)/index.html

html-xsl: $(HTML_XSL_TARGET_FILE_IN_DIR) $(HTML_XSL_TARGET)/style.css

$(HTML_XSL_TARGET_FILE_IN_DIR): $(XSL_SOURCES)
	xmlto -m tej.xsl -o $(HTML_XSL_TARGET) xhtml $<
	
$(HTML_XSL_TARGET)/style.css: style.css
	cp -f $< $@

$(HTML_DSSSL_TARGET): $(DSSSL_SOURCES)
	db2html $(DB2STH_SCRIPTS_HTML_FLAGS) $(MAIN_SOURCE)

pdf: $(DOC).pdf

$(DOC).pdf: $(DSSSL_SOURCES) jadetex.cfg
	db2pdf $(DB2STH_SCRIPTS_PRINT_FLAGS) $(MAIN_SOURCE)

rtf: $(DOC).rtf

$(DOC).rtf: $(DSSSL_SOURCES)
	db2rtf $(DB2STH_SCRIPTS_PRINT_FLAGS) $(MAIN_SOURCE)

upload: html pdf arc rtf
	$(RSYNC) -r $(DOC) $(DOC).pdf $(DOC).rtf shlomif@shell.berlios.de:/home/groups/fc-solve/htdocs/arch_doc/

