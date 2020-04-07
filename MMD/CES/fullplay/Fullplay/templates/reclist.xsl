<!DOCTYPE xsl:stylesheet [ <!ENTITY nbsp "&#160;"> ]>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:StringLookup="urn:fullplay-schema-com:StringLookup"
xmlns:Style="urn:fullplay-schema-com:Style">

	<xsl:import href="buildMenuContent.xsl"/>

	<xsl:output method="xml" indent="yes" encoding="iso-8859-1"/>
	<xsl:template match="/*">

	<!--begin screen layout-->
	<screen type="browse" history="yes">
	<layout>
		<background>fullplay/Fullplay_background.png</background>
		
		<!-- top bar elements -->
		<xsl:call-template name="placeMenuContent" />	
		<!-- top bar elements end -->

		<xsl:call-template name="placeRecListMenuContent" />	

		<element contentid="reclistbg">
		  <position left="13" top="123" width="617" height="346"/> 
		</element>

		<element contentid="topsell">
		  <position left="12" top="129" width="364" height="170"/> 
		</element>

		<element contentid="newrel">
		  <position left="12" top="129" width="364" height="170"/> 
		</element>

		<element contentid="onsale">
		  <position left="12" top="129" width="364" height="170"/> 
		</element>
		
		<list contentid="listcontent" select="single" itemheight="80" focus="true" bgcolor="255,255,255">
			<margins top="0" bottom="0" left="0" right="5" /> 
			<cellspacing width="5" /> 
			<position left="23" top="131" width="600" height="330" /> 
			<arrows up="fullplay/scrollup_up.png" down="fullplay/scrolldown_up.png" />
			<normal bgcolor="255,255,255">
				<border color="204,204,204" width="1" rounded="true" /> 
				<font color="0,0,0">Verdana12Bold</font> 
			</normal>
			<highlighted bgcolor="204,204,204">
				<border color="160,160,160" width="1" rounded="true" /> 
				<font color="0,0,0">Verdana12Bold</font> 
			</highlighted>
		</list>
	</layout>
	
	<!-- top bar content -->
    <xsl:choose>
		<xsl:when test="ProductID='4'">
			<xsl:call-template name="buildMenuContent">	     
				<xsl:with-param name="browse" select="'true'"/>
				<xsl:with-param name="search" select="'false'"/>
				<xsl:with-param name="movies" select="'false'"/>
				<xsl:with-param name="music" select="'false'"/>
				<xsl:with-param name="games" select="'true'"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:when test="ProductID='1'">
			<xsl:call-template name="buildMenuContent">	     
				<xsl:with-param name="browse" select="'true'"/>
				<xsl:with-param name="search" select="'false'"/>
				<xsl:with-param name="movies" select="'false'"/>
				<xsl:with-param name="music" select="'true'"/>	     
				<xsl:with-param name="games" select="'false'"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:when test="ProductID='3'">
			<xsl:call-template name="buildMenuContent">	     
				<xsl:with-param name="browse" select="'true'"/>
				<xsl:with-param name="search" select="'false'"/>
				<xsl:with-param name="movies" select="'true'"/>
				<xsl:with-param name="music" select="'false'"/>
				<xsl:with-param name="games" select="'false'"/>
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
			<xsl:call-template name="buildMenuContent">	     
				<xsl:with-param name="browse" select="'true'"/>
			    <xsl:with-param name="search" select="'false'"/>
				<xsl:with-param name="movies" select="'false'"/>
			    <xsl:with-param name="music" select="'true'"/>	     
			    <xsl:with-param name="games" select="'false'"/>
			</xsl:call-template>
		</xsl:otherwise>
	</xsl:choose>

	<!-- top bar content end-->
	<xsl:choose>
		<xsl:when test="ReclistID='1'">
		<xsl:call-template name="buildRecListMenuContent">	     
			 <xsl:with-param name="selectedTab" select="'1'"/>
			<xsl:with-param name="producttype" select="ProductID"/>
		</xsl:call-template>
		</xsl:when>
		<xsl:when test="ReclistID='3'">
		<xsl:call-template name="buildRecListMenuContent">	     
			 <xsl:with-param name="selectedTab" select="'2'"/>
				<xsl:with-param name="producttype" select="ProductID"/>
		</xsl:call-template>
		</xsl:when>
		<xsl:when test="ReclistID='81'">
		<xsl:call-template name="buildRecListMenuContent">	     
			 <xsl:with-param name="selectedTab" select="'3'"/>
				<xsl:with-param name="producttype" select="ProductID"/>
		</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
		<xsl:call-template name="buildRecListMenuContent">	     
			 <xsl:with-param name="selectedTab" select="'1'"/>
			<xsl:with-param name="producttype" select="ProductID"/>
		</xsl:call-template>
		</xsl:otherwise>
</xsl:choose>
	
	<content id="listcontent">

	<xsl:apply-templates select="Reclist/itemlist"/>
	
	</content>
	
	<content id="reclistbg">
	  <image url="fullplay/reclist_box_13_123_617_346.PNG" /> 
	</content>

</screen>
</xsl:template>

<xsl:template match="Reclist/itemlist">
	<xsl:for-each select="//item">
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
		  	<string padding="16"> </string>
			<image>
				<xsl:attribute name="url">
					<!--<xsl:value-of select="$imageURL"/>-->
						<xsl:value-of select="'fullplay/no_music_thumb.png'"/>
				</xsl:attribute>
			</image>
			<string padding="16"><xsl:value-of select="title"/></string> 
  			<!--<string padding="16"><xsl:value-of select="$text"/></string> -->
  		</cell>
		<link>getItemDetail.m?itemid=<xsl:value-of select="@idItem"/></link>
		</listitem>
</xsl:for-each>
</xsl:template>
<!--builds reclist menu-->
<xsl:template name="buildRecListMenuContent">
	<xsl:param name="selectedTab" select='1'/>
	<xsl:param name="producttype" select="1"/>
	<content id="tab1">
		<xsl:choose>
			<xsl:when test="$selectedTab='1'">
				<image url="fullplay/tab_top_seller_high_13_90_162_33.PNG" />
			</xsl:when>
			<xsl:otherwise>
			<image url="fullplay/tab_top_seller_up_13_90_162_33.PNG" surl="fullplay/tab_top_seller_down_13_90_162_33.PNG"/>
			</xsl:otherwise>
		</xsl:choose>
		<link>getReclist.m?idproducttype=<xsl:value-of select="$producttype"/>&amp;idreclisttype=1</link>
	</content>
	<content id="tab2">
		<xsl:choose>
			<xsl:when test="$selectedTab='2'">
				<image url="fullplay/tab_new_rel_high_174_90_189_33.PNG" />
			</xsl:when>
			<xsl:otherwise>
				<image url="fullplay/tab_new_rel_up_174_90_189_33.PNG" surl="fullplay/tab_new_rel_down_174_90_189_33.PNG"/>
			</xsl:otherwise>
		</xsl:choose>
		<link>getReclist.m?idproducttype=<xsl:value-of select="$producttype"/>&amp;idreclisttype=3</link>
	</content>
	<content id="tab3">
		<xsl:choose>
			<xsl:when test="$selectedTab='3'">
				<image url="fullplay/tab_on_sale_high_364_90_135_33.PNG" />
			</xsl:when>
			<xsl:otherwise>
				<image url="fullplay/tab_on_sale_up_364_90_135_33.PNG" surl="fullplay/tab_on_sale_down_364_90_135_33.PNG"/>
			</xsl:otherwise>
		</xsl:choose>
		<link>getReclist.m?idproducttype=<xsl:value-of select="$producttype"/>&amp;idreclisttype=81</link>
	</content>

</xsl:template>

<xsl:template name="placeRecListMenuContent">
	 <element contentid="tab1">
	 	<position left="13" top="90" width="162" height="33"/> 
	 </element>
	 <element contentid="tab2">
	 	<position left="175" top="90" width="189" height="33"/> 
	 </element>
	 <element contentid="tab3">
	 	<position left="364" top="90" width="135" height="33"/> 
	 </element>
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
		  	<string padding="16"> </string>
			<image>
				<xsl:attribute name="url">
					<!--<xsl:value-of select="$imageURL"/>-->
						<xsl:value-of select="'fullplay/no_music_thumb.png'"/>
				</xsl:attribute>
			</image>

  			<string padding="16"><xsl:value-of select="$text"/></string> 
  		</cell>
		</listitem>
</xsl:template>
</xsl:stylesheet>
