<?xml version='1.0' encoding='utf-8'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml" version="1.0">

    <xsl:import href="http://docbook.sourceforge.net/release/xsl/current/xhtml5/onechunk.xsl" />

    <!-- Avoid Generating a Table-of-Contents-->
    <xsl:param name="generate.toc"></xsl:param>

<!-- Disable the title="" attribute in sections. -->
<xsl:template name="generate.html.title">
</xsl:template>

</xsl:stylesheet>
