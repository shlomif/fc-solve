<?xml version='1.0' encoding='UTF-8'?>
<xsl:stylesheet
    exclude-result-prefixes="d"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:d="http://docbook.org/ns/docbook"
    version='1.0'
    >
    <xsl:import href="http://docbook.sourceforge.net/release/xsl-ns/current/xhtml5/html5-element-mods.xsl"/>

    <xsl:param name="css.decoration" select="0"></xsl:param>
    <xsl:param name="generate.id.attributes" select="0"></xsl:param>
    <xsl:param name="html.cellspacing"></xsl:param>
    <xsl:param name="html.cellpadding"></xsl:param>
    <xsl:param name="make.clean.html" select="1"></xsl:param>

    <xsl:template name="pi.dbhtml_cellpadding">
        <xsl:text/>
    </xsl:template>
    <xsl:template name="pi.dbhtml_cellspacing">
        <xsl:text/>
    </xsl:template>
    <!--
         Commented out because it does not work properly.
    <xsl:template name="anchor">
        <xsl:param name="node" select="."/>
        <xsl:param name="conditional" select="1"/>
        <xsl:variable name="id">
            <xsl:call-template name="object.id">
                <xsl:with-param name="object" select="$node"/>
            </xsl:call-template>
        </xsl:variable>

        <xsl:attribute name="id">
            <xsl:value-of select="$id" />
        </xsl:attribute>

    </xsl:template>
    -->
<xsl:template name="root.attributes">
        <xsl:attribute name="lang">
            <xsl:if test="//*/@lang">
                <xsl:value-of select="//*/@lang"/>
            </xsl:if>
            <xsl:if test="//*/@xml:lang">
                <xsl:value-of select="//*/@xml:lang"/>
            </xsl:if>
        </xsl:attribute>
</xsl:template>
</xsl:stylesheet>
