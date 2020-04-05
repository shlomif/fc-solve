<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
    xmlns="http://docbook.org/ns/docbook"
    xmlns:d="http://docbook.org/ns/docbook"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xlink="http://www.w3.org/1999/xlink"
    >

    <xsl:import href="http://docbook.sourceforge.net/release/xsl-ns/current/epub/docbook.xsl" />

    <xsl:output method="xml" version="1.0" encoding="UTF-8" indent="no" />

<!-- Get rid of the revhistory element -->
<xsl:template match="d:revhistory" mode="titlepage.mode" />

</xsl:stylesheet>
