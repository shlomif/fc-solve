<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml" version='1.0'>
    <xsl:param name="use.id.as.filename">1</xsl:param>
    <xsl:param name="html.stylesheet">style.css</xsl:param>
    <xsl:param name="itemizedlist.propagates.style">1</xsl:param>
    <xsl:param name="chunker.output.doctype-public">-//W3C//DTD XHTML 1.0 Transitional//EN</xsl:param>
    <xsl:param name="chunker.output.doctype-system">http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd</xsl:param>
    <!-- Parameters for Generating Strict Output. See:
    http://www.sagehill.net/docbookxsl/OtherOutputForms.html#StrictXhtmlValid
    -->
    <xsl:param name="css.decoration">0</xsl:param>
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


      <xsl:if test="$css.decoration != 0">
        <xsl:attribute name="type">
          <xsl:call-template name="list.itemsymbol"/>
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

    <div class="center ads_top">
    <script type="text/javascript">
google_ad_client = "pub-2480595666283917";
google_ad_width = 468;
google_ad_height = 60;
google_ad_format = "468x60_as";
google_ad_type = "text";
google_ad_channel ="";
google_color_border = "336699";
google_color_text = "000000";
google_color_bg = "FFFFFF";
google_color_link = "0000FF";
google_color_url = "008000";
</script>
<script type="text/javascript"
  src="http://pagead2.googlesyndication.com/pagead/show_ads.js">
</script>
    </div>
    <xsl:if test="string($docmake.output.work_in_progress)">
    <div class="center warning">
        <p>
            <b>Note:</b> This document is work-in-progress. Please don't 
            publish it on news sites, or otherwise link to it in public
            without the author's permission. Private linking is acceptable.
        </p>
    </div>
    </xsl:if>
</xsl:template>

</xsl:stylesheet>
