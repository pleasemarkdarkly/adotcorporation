<!DOCTYPE xsl:stylesheet [ <!ENTITY nbsp "&#160;"> ]>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:StringLookup="urn:fullplay-schema-com:StringLookup"
xmlns:Style="urn:fullplay-schema-com:Style" xmlns:Ext="xslext">
	<xsl:import href="buildMenuContent.xsl"/>
	<xsl:output method="xml" indent="yes" encoding="iso-8859-1"/>
	<xsl:template match="/*">
		<!--begin screen layout-->
		<screen type="replay" history="yes">
	<layout>
		<background>fullplay/Fullplay_background.png</background>

		<!-- top bar elements -->
		<xsl:call-template name="placeMenuContent" />	
		<!-- top bar elements end -->
	
		 <element contentid="topsell">
		  <position left="375" top="122" width="236" height="47"/> 
		</element>
		 <element contentid="newrel">
		  <position left="375" top="182" width="236" height="47"/> 
		</element>
		 <element contentid="onsale">
		  <position left="375" top="241" width="236" height="47"/> 
		</element>
		 <element contentid="browsebox">
		  <position left="13" top="122" width="342" height="347"/> 
		</element>
		<element contentid="featureart">		
			<position left="450" top="340" width="65" height="65"/> 	
		</element>
		<block contentid="featuretext">
			<position left="377" top="445" width="230" height="50"/> 
		</block>
		<block contentid="featureheader">
			<position left="377" top="295" width="230" height="40"/> 
		</block>

		<list contentid="listcontent" select="single" itemheight="30" focus="true" bgcolor="255,255,255">
		<margins top="6" bottom="4" left="6" right="7" /> 
			<cellspacing width="7" /> 
			<position left="19" top="127" width="330" height="335" /> 
			<arrows up="fullplay/scrollup_up.png" down="fullplay/scrolldown_up.png" />
			<normal bgcolor="255,255,255">
			<border color="204,204,204" width="1" rounded="true" /> 
			<font color="0,0,0">Verdana12Bold</font> 
		</normal>
			<highlighted bgcolor="204,204,204">
			<border color="160,160,160" width="1" /> 
			<font color="0,0,0">Verdana12Bold</font> 
			</highlighted>
		</list>

	</layout>


	<!-- top bar content -->
     
	<xsl:call-template name="buildMenuContent">	     
	     <xsl:with-param name="browse" select="'true'"/>
	     <xsl:with-param name="movies" select="'false'"/>
	     <xsl:with-param name="music" select="'true'"/>
	     <xsl:with-param name="search" select="'false'"/>
	     <xsl:with-param name="games" select="'false'"/>
	</xsl:call-template>

	<!-- top bar content end-->
	

	<content id="topsell">
	  <image url="fullplay/button_topsell_w_375_122_236_47.PNG" surl="fullplay/button_topsell_y_375_122_236_47.PNG"/> 
	<link>getReclist.m?idproducttype=1&amp;idreclisttype=1</link>
</content>

	<content id="newrel">
	  <image url="fullplay/button_newrel_w_375_182_236_47.PNG" surl="fullplay/button_newrel_y_375_182_236_47.PNG"/> 
	<link>getReclist.m?idproducttype=1&amp;idreclisttype=3</link>	
</content>

	<content id="onsale">
	  <image url="fullplay/button_onsale_w_375_241_236_47.PNG" surl="fullplay/button_onsale_y_375_241_236_47.PNG"/> 
	<link>getReclist.m?idproducttype=1&amp;idreclisttype=81</link>	
</content>

	<content id="browsebox">
	  <image url="fullplay/Browse_music_box_13_122_342_347.PNG" /> 
	</content>

	<content id="featureart">
		<image url="fullplay/no_music_thumb.png"/>
	</content>
	
	<content id="featuretext">
 		<string font="Verdana12bold">Pink Floyd - </string> 
 		<string font="Verdana12">Dark Side of the Moon</string> 
	</content>

	<content id="featureheader">
		<string font="Verdana12bold">feature album</string>
	</content>

	<content id="listcontent">
		<!-- 
			each listitem is build via template. so to automate this further
			you should just have to tweak the buildListItem template a bit
			so that it uses a match of some sort
		-->

<xsl:apply-templates select="Genres/genres"/>

	</content>
</screen>
</xsl:template>

<!-- generates a listitem for given genre name -->
<xsl:template match="Genres/genres">
	<xsl:for-each select="genre">
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
  			<string padding="16"><xsl:value-of select="./text()"/></string> 
  		</cell>
		<xsl:choose>
			<xsl:when test="@idMediaType='1'">
				<link>getArtists.m?idmediatype=<xsl:value-of select="@idMediaType"/>&amp;idgenre=<xsl:value-of select="@idGenre"/>&amp;index=1</link>
			</xsl:when>
			<xsl:when test="@idMediaType='10'">
				<link>getGames.m?sGenre=<xsl:value-of select="./text()"/></link>
			</xsl:when>
			<xsl:when test="@idMediaType='13'">
				<link>getMovies.m?idgenre=<xsl:value-of select="@idGenre"/>&amp;idmediatype=<xsl:value-of select="@idMediaType"/></link>
			</xsl:when>
		</xsl:choose>
		</listitem>
	</xsl:for-each>
</xsl:template>
		

<xsl:template name="buildListItem">
		<xsl:param name="genre" select="false"/>

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
  			<string padding="16">
				<xsl:value-of select="$genre"/>
			</string> 
  		</cell>
		</listitem>
</xsl:template>


</xsl:stylesheet>
