<?xml version='1.0' encoding='utf-8'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml" version='1.0'>
    <xsl:param name="use.id.as.filename">1</xsl:param>
    <xsl:param name="html.stylesheet">style.css</xsl:param>
    <xsl:param name="itemizedlist.propagates.style">1</xsl:param>
    <!-- Parameters for Generating Strict Output. See:
    http://www.sagehill.net/docbookxsl/OtherOutputForms.html#StrictXhtmlValid
    -->
    <xsl:param name="ulink.target"></xsl:param>
    <xsl:param name="use.viewport">0</xsl:param>
    <!-- End of Strict Params -->
    <xsl:param name="toc.section.depth">10</xsl:param>
    <xsl:param name="generate.section.toc.level">10</xsl:param>
    <!-- Enable fop extensions - see:
    http://www.sagehill.net/docbookxsl/InstallingAnFO.html
    http://www.mail-archive.com/fop-users%40xmlgraphics.apache.org/msg06170.html
    -->
    <xsl:param name="fop1.extensions">1</xsl:param>

<xsl:template name="html_itemized_list">
    <div xmlns="http://www.w3.org/1999/xhtml" class="{name(.)}">
    <xsl:call-template name="anchor"/>
    <xsl:if test="title">
      <xsl:call-template name="formal.object.heading"/>
    </xsl:if>

    <xsl:apply-templates select="*[not(self::listitem or self::title
        or self::titleabbrev)]"/>

    <ul>
         <xsl:if test="@role">
             <xsl:attribute name="class">
                 <xsl:value-of select="@role"/>
             </xsl:attribute>
         </xsl:if>


      <xsl:if test="@spacing='compact'">
        <xsl:attribute name="compact">
          <xsl:value-of select="@spacing"/>
        </xsl:attribute>
      </xsl:if>
      <xsl:apply-templates select="listitem"/>
    </ul>
  </div>
</xsl:template>

<!-- Insert some AdSense Ads -->
<xsl:template name="user.header.navigation">
    <!--
    <div class="site_nav_menu">
        <ul>
            <li><a href="{$docmake.output.path_to_root}">Home</a></li>
            <li><a href="{$docmake.output.path_to_root}humour/">Humour</a></li>
            <li><a href="{$docmake.output.path_to_root}philosophy/">Articles and Essays</a></li>
            <li><a href="{$docmake.output.path_to_root}puzzles/">Puzzles</a></li>
            <li><a href="{$docmake.output.path_to_root}art/">Computer Art</a></li>
        </ul>
    </div>
    -->
    <div class="center ads_top"></div>
    <xsl:if test="string($docmake.output.work_in_progress)">
    <div class="center warning">
        <p>
            <b>Note:</b> This document is work-in-progress. Please don’t
            publish it on news sites, or otherwise link to it in public
            without the author’s permission. Private linking is acceptable.
        </p>
    </div>
    </xsl:if>
</xsl:template>

<!-- Disable the title="" attribute in sections. -->
<xsl:template name="generate.html.title">
</xsl:template>

</xsl:stylesheet>
