<!DOCTYPE xsl:stylesheet [ <!ENTITY nbsp "&#160;"> ]>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:StringLookup="urn:fullplay-schema-com:StringLookup"
xmlns:Style="urn:fullplay-schema-com:Style" xmlns:Ext="xslext" >

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

		<xsl:call-template name="placeAlphabetMenuContent" />	

		
		 <element contentid="artistbrowsebox">
		  <position left="13" top="123" width="342" height="347"/> 
		</element>
		 <element contentid="albumbrowsebox">
		  <position left="349" top="123" width="280" height="347"/> 
		</element>
		<list contentid="artistlist" select="single" itemheight="30" focus="true" bgcolor="255,255,255">
			<margins top="3" bottom="0" left="4" right="7" /> 
			<cellspacing width="7" /> 
			<position left="21" top="130" width="326" height="331" /> 
			<arrows up="fullplay/scrollup_up.png" down="fullplay/scrolldown_up.png" />			
			<normal bgcolor="255,255,255">
				<border color="204,204,204" width="2" rounded="false" /> 
				<font color="0,0,0">Verdana12Bold</font> 
			</normal>
			<highlighted bgcolor="204,204,204">
				<border color="160,160,160" width="2" round="false"/> 
				<font color="0,0,0">Verdana12Bold</font> 
			</highlighted>
		</list>
		<list contentid="albumlist" select="single" itemheight="65" focus="true" bgcolor="255,255,255">
			<margins top="3" bottom="0" left="4" right="7" /> 
			<cellspacing width="13" /> 
			<position left="366" top="139" width="256" height="322" /> 
			<arrows up="fullplay/scrollup_up.png" down="fullplay/scrolldown_up.png" />
			<normal bgcolor="255,255,255">
				<font color="0,0,0">Verdana12Bold</font> 
			</normal>
			<highlighted bgcolor="255,255,255">
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
			<xsl:call-template name="buildAlphabetMenuContent">
				<xsl:with-param name="selectedTab" select="Artists/artists/@index"/>
				<xsl:with-param name="genre" select="Artists/artists/@genreid"/>
				<xsl:with-param name="mediatype" select="Artists/artists/@mediaid"/>
			</xsl:call-template>


	<content id="albumbrowsebox">
	  <image url="fullplay/Music_bro_box2_349_123_280_3.PNG" /> 
	</content>

	<content id="artistbrowsebox">
	  <image url="fullplay/Music_browse_box_13_123_337_347.PNG" /> 
	</content>

	<content id="artistlist">
		<!-- 
			each listitem is built via template. so to automate this further
			you should just have to tweak the buildArtistListItem template a bit
			so that it uses some match / for-each magic
		-->
<xsl:apply-templates select="Artists/artists"/>
		
	</content>

	<content id="albumlist">
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
				<!--<xsl:attribute name="url">
					<xsl:value-of select="$imageURL"/>
				</xsl:attribute>-->
			</image>

  		<!--	<string padding="10"><xsl:value-of select="$text"/></string> -->
  		</cell>
		</listitem>
		<!-- 
			each listitem is built via template. so to automate this further
			you should just have to tweak the buildAlbumListItem template a bit
			so that it uses match / for-each magic
		-->
<!--
		<xsl:call-template name="buildAlbumListItem">	     
		     <xsl:with-param name="text" select="'Auto Portis'"/>
		     <xsl:with-param name="imageURL" select="'fullplay\no_music_thumb.png'"/>
		</xsl:call-template>
		<xsl:call-template name="buildAlbumListItem">	     
		     <xsl:with-param name="text" select="'Auto Portis'"/>
		     <xsl:with-param name="imageURL" select="'fullplay\no_music_thumb.png'"/>
		</xsl:call-template>
		<xsl:call-template name="buildAlbumListItem">	     
		     <xsl:with-param name="text" select="'Auto Portis'"/>
		     <xsl:with-param name="imageURL" select="'fullplay\no_music_thumb.png'"/>
		</xsl:call-template>
		<xsl:call-template name="buildAlbumListItem">	     
		     <xsl:with-param name="text" select="'Auto Portis'"/>
		     <xsl:with-param name="imageURL" select="'fullplay\no_music_thumb.png'"/>
		</xsl:call-template>
		<xsl:call-template name="buildAlbumListItem">	     
		     <xsl:with-param name="text" select="'Auto Portis'"/>
		     <xsl:with-param name="imageURL" select="'fullplay\no_music_thumb.png'"/>
		</xsl:call-template>
		<xsl:call-template name="buildAlbumListItem">	     
		     <xsl:with-param name="text" select="'Auto Portis'"/>
		     <xsl:with-param name="imageURL" select="'fullplay\no_music_thumb.png'"/>
		</xsl:call-template>
-->
	</content>
</screen>
</xsl:template>

<xsl:template match="Artists/artists">

	<xsl:for-each select="artistname">
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
	<link>getDiscography.m?artistname=<xsl:value-of select="Ext:UrlEncode(./text())"/></link>
	</listitem>
	</xsl:for-each>
</xsl:template>

<!-- generates a listitem for given artist name -->
<xsl:template name="buildArtistListItem">
	<xsl:param name="artist" select="false"/>

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
		<string padding="16"><xsl:value-of select="$artist"/></string> 
	</cell>
	</listitem>

</xsl:template>

<!-- generates a listitem for given album image and text -->
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


<!--builds alphabet menu-->
<xsl:template name="buildAlphabetMenuContent">
	<xsl:param name="selectedTab" select='1'/>
	<xsl:param name="genre" select="1"/>
	<content id="tab1">
		<xsl:choose>
			<xsl:when test="$selectedTab='1'">
				<image url="fullplay/tab_af_high_13_90_70_33.PNG" />
				<link>getArtists.m?idmediatype=1&amp;idgenre=<xsl:value-of select="$genre"/>&amp;index=<xsl:value-of select="1"/></link>
			</xsl:when>
			<xsl:otherwise>
				<image url="fullplay/tab_af_up_13_90_70_33.PNG" surl="fullplay/tab_af_down_13_90_70_33.PNG"/>
				<link>getArtists.m?idmediatype=1&amp;idgenre=<xsl:value-of select="$genre"/>&amp;index=<xsl:value-of select="1"/></link>
			</xsl:otherwise>
		</xsl:choose>
	</content>
	<content id="tab2">
		<xsl:choose>
			<xsl:when test="$selectedTab='2'">
				<image url="fullplay/tab_gl_high_83_90_68_33.PNG" />
				<link>getArtists.m?idmediatype=1&amp;idgenre=<xsl:value-of select="$genre"/>&amp;index=<xsl:value-of select="2"/></link>
			</xsl:when>
			<xsl:otherwise>
				<image url="fullplay/tab_gl_up_83_90_68_33.PNG" surl="fullplay/tab_gl_down_83_90_68_33.PNG"/>
				<link>getArtists.m?idmediatype=1&amp;idgenre=<xsl:value-of select="$genre"/>&amp;index=<xsl:value-of select="2"/></link>
			</xsl:otherwise>
		</xsl:choose>
	</content>
	<content id="tab3">
		<xsl:choose>
			<xsl:when test="$selectedTab='3'">
				<image url="fullplay/tab_mr_high_151_90_74_33.PNG" />
				<link>getArtists.m?idmediatype=1&amp;idgenre=<xsl:value-of select="$genre"/>&amp;index=<xsl:value-of select="3"/></link>
			</xsl:when>
			<xsl:otherwise>
				<image url="fullplay/tab_mr_up_151_90_74_33.PNG" surl="fullplay/tab_mr_down_151_90_74_33.PNG"/>
				<link>getArtists.m?idmediatype=1&amp;idgenre=<xsl:value-of select="$genre"/>&amp;index=<xsl:value-of select="3"/></link>
			</xsl:otherwise>
		</xsl:choose>
	</content>
	<content id="tab4">
		<xsl:choose>
			<xsl:when test="$selectedTab='4'">
				<image url="fullplay/tab_sv_high_224_90_69_33.PNG" />
				<link>getArtists.m?idmediatype=1&amp;idgenre=<xsl:value-of select="$genre"/>&amp;index=<xsl:value-of select="4"/></link>
			</xsl:when>
			<xsl:otherwise>
				<image url="fullplay/tab_sv_up_224_90_69_33.PNG" surl="fullplay/tab_sv_down_224_90_69_33.PNG"/>
				<link>getArtists.m?idmediatype=1&amp;idgenre=<xsl:value-of select="$genre"/>&amp;index=<xsl:value-of select="4"/></link>
			</xsl:otherwise>
		</xsl:choose>
	</content>
	<content id="tab5">
		<xsl:choose>
			<xsl:when test="$selectedTab='5'">
				<image url="fullplay/tab_wz_high_294_90_82_33.PNG" />
				<link>getArtists.m?idmediatype=1&amp;idgenre=<xsl:value-of select="$genre"/>&amp;index=<xsl:value-of select="5"/></link>
			</xsl:when>
			<xsl:otherwise>
				<image url="fullplay/tab_wz_up_294_90_82_33.PNG" surl="fullplay/tab_wz_down_294_90_82_33.PNG"/>
				<link>getArtists.m?idmediatype=1&amp;idgenre=<xsl:value-of select="$genre"/>&amp;index=<xsl:value-of select="5"/></link>
			</xsl:otherwise>
		</xsl:choose>
	</content>

</xsl:template>

<xsl:template name="placeAlphabetMenuContent">
	 <element contentid="tab1">
	 	<position left="13" top="90" width="70" height="33"/> 
	 </element>
	 <element contentid="tab2">
	 	<position left="83" top="90" width="68" height="33"/> 
	 </element>
	 <element contentid="tab3">
	 	<position left="151" top="90" width="74" height="33"/> 
	 </element>
	 <element contentid="tab4">
	 	<position left="225" top="90" width="69" height="33"/> 
	 </element>
	 <element contentid="tab5">
	 	<position left="294" top="90" width="82" height="33"/> 
	 </element>
</xsl:template>

</xsl:stylesheet>
