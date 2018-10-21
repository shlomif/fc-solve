<?xml version='1.0' encoding='utf-8'?>
<xsl:stylesheet
     version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:import href="http://docbook.sourceforge.net/release/xsl/current/fo/docbook.xsl" />
    <xsl:import href="shlomif-essays.xsl" />

<!--colored and hyphenated links -->
<xsl:template match="ulink">
    <fo:basic-link external-destination="{@url}"
         xsl:use-attribute-sets="xref.properties"
         text-decoration="underline"
         color="blue">
         <xsl:choose>
         <xsl:when test="count(child::node())=0">
         <xsl:value-of select="@url"/>
         </xsl:when>
         <xsl:otherwise>
         <xsl:apply-templates/>
         </xsl:otherwise>
         </xsl:choose>
    </fo:basic-link>
</xsl:template>
</xsl:stylesheet>
