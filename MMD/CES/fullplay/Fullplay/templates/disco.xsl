<!DOCTYPE xsl:stylesheet [ <!ENTITY nbsp "&#160;"> ]>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:StringLookup="urn:fullplay-schema-com:StringLookup"
xmlns:Style="urn:fullplay-schema-com:Style" xmlns:Ext="xslext">
	<xsl:import href="buildMenuContent.xsl"/>
	<xsl:output method="xml" indent="yes" encoding="iso-8859-1"/>
	<xsl:template match="/*">
			<!--begin screen layout-->
<frame name="search_content">
<layout>
		<list contentid="albumlist" select="single" itemheight="65" focus="true" bgcolor="255,255,255">
			<margins top="3" bottom="0" left="4" right="7" /> 
			<cellspacing width="13" /> 
			<position left="366" top="139" width="256" height="322" /> 
			<arrows up="fullplay/scrollup_up.png" down="fullplay/scrolldown_up.png" />
			<normal bgcolor="255,255,255">
				<font color="0,0,0">Verdana12Bold</font> 
			</normal>
			<highlighted bgcolor="204,204,204">
				<font color="0,0,0">Verdana12Bold</font> 
			</highlighted>
		</list>

</layout>
	
	<content id="albumlist">
		<!-- 
			each listitem is built via template. so to automate this further
			you should just have to tweak the buildAlbumListItem template a bit
			so that it uses match / for-each magic
		-->
		<xsl:apply-templates select="Disco/discography"/>
		<!--
		<xsl:call-template name="buildAlbumListItem">	     
		     <xsl:with-param name="text" select="'ARGGHHH'"/>
		     <xsl:with-param name="imageURL" select="'fullplay\no_music_thumb.png'"/>
		</xsl:call-template>
		<xsl:call-template name="buildAlbumListItem">	     
		     <xsl:with-param name="text" select="'ARGGHHH'"/>
		     <xsl:with-param name="imageURL" select="'fullplay\no_music_thumb.png'"/>
		</xsl:call-template>
		<xsl:call-template name="buildAlbumListItem">	     
		     <xsl:with-param name="text" select="'ARGGHHH'"/>
		     <xsl:with-param name="imageURL" select="'fullplay\no_music_thumb.png'"/>
		</xsl:call-template>
		<xsl:call-template name="buildAlbumListItem">	     
		     <xsl:with-param name="text" select="'ARGGHHH'"/>
		     <xsl:with-param name="imageURL" select="'fullplay\no_music_thumb.png'"/>
		</xsl:call-template>
		<xsl:call-template name="buildAlbumListItem">	     
		     <xsl:with-param name="text" select="'ARGGHHH'"/>
		     <xsl:with-param name="imageURL" select="'fullplay\no_music_thumb.png'"/>
		</xsl:call-template>
		<xsl:call-template name="buildAlbumListItem">	     
		     <xsl:with-param name="text" select="'ARGGHHH'"/>
		     <xsl:with-param name="imageURL" select="'fullplay\no_music_thumb.png'"/>
		</xsl:call-template>
		-->

	</content>
	
</frame>
</xsl:template>

<xsl:template match="Disco/discography">
<xsl:for-each select="item">
<listitem>
		<cell align="left">
			<normal>
				<font color="0,0,0">Verdana12Bold</font> 
		 	</normal>
		 	<highlighted>
		  		<font color="0,0,0">Verdana12Bold</font> 
		  	</highlighted>
		 	<selected>
		  		<font color="0,0,0">Verdana12Bold</font> 
		  	</selected>			  
			<image>
				<xsl:attribute name="url">
					<!--<xsl:value-of select="coverartsmallurl"/>-->
					<xsl:value-of select="'fullplay/no_music_thumb.png'"/>
				</xsl:attribute>
			</image>
  			<string padding="10"><xsl:value-of select="sTitle"/></string> 
  		</cell>
		<link>getItemDetail.m?itemid=<xsl:value-of select="@id"/></link>
		</listitem>


</xsl:for-each>
</xsl:template>

<xsl:template name="buildAlbumListItem">
	<xsl:param name="text" select="false"/>
	<xsl:param name="imageURL" select="false"/>

		<listitem>
		<cell align="left">
			<normal>
				<font color="0,0,0">Verdana12Bold</font> 
		 	</normal>
		 	<highlighted>
		  		<font color="0,0,0">Verdana12Bold</font> 
		  	</highlighted>
		 	<selected>
		  		<font color="0,0,0">Verdana12Bold</font> 
		  	</selected>			  
			<image>
				<xsl:attribute name="url">
					<!--<xsl:value-of select="$imageURL"/>-->
					<xsl:value-of select="'fullplay/no_music_thumb.png'"/>
				</xsl:attribute>
			</image>

  			<string padding="10"><xsl:value-of select="$text"/></string> 
  		</cell>
		</listitem>
</xsl:template>
</xsl:stylesheet>