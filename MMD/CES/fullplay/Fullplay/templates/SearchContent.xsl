<!DOCTYPE xsl:stylesheet [ <!ENTITY nbsp "&#160;"> ]>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:StringLookup="urn:fullplay-schema-com:StringLookup"
xmlns:Style="urn:fullplay-schema-com:Style">
	<xsl:output method="xml" indent="yes" encoding="iso-8859-1"/>
	<xsl:template match="/*">

		<!--begin screen layout-->
<frame name="search_content">
<layout>
		<list contentid="listcontent" select="grid" itemsize="80" focus="true" bgcolor="255,255,255" rows="2" columns="3">
		  <cellspacing width="1" /> 
		  <border color="160,160,160" width="2" /> 
		  <position left="20" top="105" width="595" height="160" /> 
		  <arrows up="fullplay/scrollup_up.png" down="fullplay/scrolldown_up.png" /> 
		<normal bgcolor="255,255,255">
		  <border color="204,204,204" width="0" rounded="true" /> 
		  <font color="0,0,0">Verdana11Bold</font> 
		  </normal>
		<highlighted bgcolor="255,255,255">
		  <border color="160,160,160" width="0" /> 
		  </highlighted>
		</list>

</layout>
	<content id="listcontent">
		<xsl:apply-templates select="//Search/searchlist"/>
	</content>
</frame>
</xsl:template>

<xsl:template match="Search/searchlist">
		<xsl:for-each select="item">
		<listitem>
		<cell align="left" width="10" /> 
		<cell align="left" width="80">
			<image>
				<xsl:attribute name="url">
					<!--<xsl:value-of select="coverartsmallurl"/>-->
					<xsl:value-of select="'fullplay/no_music_thumb.png'"/>
				</xsl:attribute>
			</image>
		  </cell>
		<cell align="left" valign="top" width="100">
		  <vcell height="10" /> 
		   <vcell valign="top" height="15">
				<string><xsl:value-of select="artistinfo"/></string> 
		  </vcell>
			<vcell valign="top" height="15">
		<cell>
			<highlighted>
			  <font color="0,0,0">Verdana12</font> 
			</highlighted>
			<normal>
				<font color="0,0,0">Verdana12</font> 
			</normal>
			<selected>
				 <font color="0,0,0">Verdana12</font> 
			</selected>
				<string font="Verdana12"><xsl:value-of select="title"/></string> 
		</cell>
		  </vcell>
		  </cell>
		  <cell /> 		  
		  <link>getItemDetail.m?itemid=<xsl:value-of select="./@id"/></link>
	</listitem>
</xsl:for-each>
</xsl:template>
</xsl:stylesheet>
