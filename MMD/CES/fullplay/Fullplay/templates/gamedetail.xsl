<!DOCTYPE xsl:stylesheet [ <!ENTITY nbsp "&#160;"> ]>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:StringLookup="urn:fullplay-schema-com:StringLookup"
xmlns:Style="urn:fullplay-schema-com:Style" xmlns:Ext="xslext">
	
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
				
		<element contentid="synopsisbg">
		  <position left="12" top="172" width="363" height="135"/> 
		</element>
		
		<block contentid="synopsistext" focus="true">
			<position left="25" top="180" width="345" height="120"/>
			<arrows up="fullplay/scrollup_up.png" down="fullplay/scrolldown_up.png" />
			<font color="0,0,0">Verdana12</font>
		</block>
		
		<element contentid="synopsistab">
			<position left="12" top="139" width="170" height="33"/>
		</element>
		
		<element contentid="detailbox">
		  <position left="12" top="342" width="617" height="127"/> 
		</element>
		
		<xsl:call-template name="placeDetailMenuContent" />
		
		<element contentid="gameart">
			<position left="440" top="140" width="162" height="162"/> 		
		</element>
		
        <!--<block contentid="reviewtext" margin="0" focus="true">
            <position left="20" top="350" width="600" height="110"/> 
      	    <arrows up="fullplay/scrollup_up.png" down="fullplay/scrolldown_up.png" /> 
            <font color="0,0,0">Verdana11</font> 
            <border color="0,255,0" width="0" /> 
        </block>-->
        
        <element contentid="similar1">
			<position left="35" top="350" width="120" height="110"/> 
		</element>

		<element contentid="similar2">
			<position left="155" top="350" width="120" height="110"/> 
		</element>

		<element contentid="similar3">
			<position left="275" top="350" width="120" height="110"/> 
		</element>

		<element contentid="similar4">
			<position left="395" top="350" width="120" height="110"/> 
		</element>	

		<element contentid="similar5">
			<position left="515" top="350" width="120" height="110"/> 
		</element>
               
        <!--<list contentid="discography" select="grid" itemsize="80" focus="true" bgcolor="255,255,255" rows="1" columns="3">
		  <cellspacing width="1" /> 
		  <border color="160,160,160" width="2" /> 
		  <position left="20" top="350" width="600" height="110"/> 
		         	    <arrows up="fullplay/scrollup_up.png" down="fullplay/scrolldown_up.png" /> 
		         	    <normal bgcolor="255,255,255">
		  <border color="204,204,204" width="0" rounded="true" /> 
		  <font color="0,0,0">Verdana11Bold</font> 
		  </normal>
		   <highlighted bgcolor="255,255,255">
		  <border color="160,160,160" width="0" /> 
		  </highlighted>
		</list>-->
		
		<block contentid="gametext">
			<position left="18" top="90" width="200" height="100"/> 
		  <font color="0,0,0">Verdana12bold</font> 

		</block>
		
		<block contentid="gameprice" align="right">
			<position left="180" top="140" width="190" height="100"/> 
				  <font color="0,0,0">Verdana12bold</font> 

		</block>
	
	
		<block contentid="gameinfo">
			<position left="440" top="80" width="200" height="100"/> 
			<font color="0,0,0">Verdana12bold</font> 					
		</block>
	</layout>
	
	<content id="synopsistext">
		<string><xsl:value-of select="Item/item/synopsis"/></string>
	</content>
	
	<content id="gameart">
		<image>
			<xsl:attribute name="url">
				<xsl:value-of select="Item/item/coverartlargeurl"/>
			</xsl:attribute>
		</image>
	</content>

	<content id="gametext">
 		<string font="Verdana16Bold"><xsl:value-of select="Item/item/title"/><xsl:text>\n</xsl:text></string> 
 		<string font="Verdana12Bold"><xsl:value-of select="Item/item/platform"/></string> 
	</content>

	<content id="gameprice">
		<string font="Verdana24Bold">$19.99</string>
	</content>


	<content id="gameinfo">
		<string font="Verdana12">genre: </string>
		<string font="Verdana12Bold"><xsl:value-of select="Item/item/genre"/><xsl:text>\n</xsl:text></string>
		<string font="Verdana12">release date: </string>
		<string font="Verdana12Bold"><xsl:value-of select="Item/item/releasedate"/><xsl:text>\n</xsl:text></string>
		<string font="Verdana12">publisher: </string>
		<string font="Verdana12Bold"><xsl:value-of select="Item/item/label"/><xsl:text>\n</xsl:text></string>
	</content>
	
	<!-- top bar content -->
	     
	     
	<xsl:call-template name="buildMenuContent">	     
	     <xsl:with-param name="browse" select="'false'"/>
	     <xsl:with-param name="movies" select="'false'"/>
	     <xsl:with-param name="music" select="'false'"/>
	     <xsl:with-param name="search" select="'false'"/>
	     <xsl:with-param name="games" select="'true'"/>
	</xsl:call-template>

	<xsl:call-template name="buildDetailMenuContent">	     
		<xsl:with-param name="selectedTab" select="'3'"/>	     
	</xsl:call-template>
	<!-- top bar content end-->
	
	
	<content id="topsell">
	  <image url="fullplay/button_top_sell_375_123_47_236.PNG" /> 
	</content>

	<content id="newrel">
	  <image url="fullplay/button_new_rel_375_181_47_236.PNG" /> 
	</content>

	<content id="detailbox">
	  <image url="fullplay/details_2box_12_342_617_127.PNG" /> 
	</content>

	<content id="synopsisbg">
	  <image url="fullplay/detail_box_12_172_363_135.PNG" /> 
	</content>
	
	<content id="synopsistab">
		<image url="fullplay/Tab_game_syn_up_12_139_170_33.png" />
	</content>
	

	<!-- elements placed for similar items section-->

	<content id="similar1">
		<image url="list/074646390023.jpg" /> 
	</content>

	<content id="similar2">
		<image url="list/074646390023.jpg" /> 
	</content>

	<content id="similar3">
		<image url="list/074646390023.jpg" /> 
	</content>

	<content id="similar4">
		<image url="list/074646390023.jpg" /> 
	</content>	

	<content id="similar5">
		<image url="list/074646390023.jpg" /> 
	</content>	


<!-- element for review text section-->

<!--

<content id="reviewtext">
  <string font="Verdana11Bold" padding="5">\nBig Text\n</string> 
    <string> test test test test test test test test test \n
     test test test test test test test test test \n
      test test test test test test test test test \n
       test test test test test test test test test \n test test test test test test test test test \n
        test test test test test test test test test \n
         test test test test test test test test test \n
          test test test test test test test test test \n
           test test test test test test test test test \n
            test test test test test test test test test \n
             test test test test test test test test test \n
              test test test test test test test test test \n
               test test test test test test test test test \n
               </string>

  </content>

-->

</screen>
</xsl:template>


<!--builds bottom product detail menu -->

<xsl:template name="buildDetailMenuContent">
	<xsl:param name="selectedTab" select='1'/>
	<content id="tab1">
		<xsl:choose>
			<xsl:when test="$selectedTab='1'">
				<image url="fullplay/tab_sim_12_309_142_33.png" />				
			</xsl:when>
			<xsl:otherwise>
				<image url="fullplay/tab_sim_up_12_309_142_33.png" surl="fullplay/tab_sim_down_12_309_142_33.png"/>				
			</xsl:otherwise>
		</xsl:choose>
	</content>
	<content id="tab2">
		<xsl:choose>
			<xsl:when test="$selectedTab='2'">
				<image url="fullplay/tab_review_high_153_309_105_33.png" />
			</xsl:when>
			<xsl:otherwise>
				<image url="fullplay/tab_review_up_153_309_105_33.png" surl="fullplay/tab_review_down_153_309_105_33.png"/>
			</xsl:otherwise>
		</xsl:choose>
	</content>

</xsl:template>

<xsl:template name="placeDetailMenuContent">
	 <element contentid="tab1">
	 	<position left="12" top="309" width="142" height="33"/> 
	 </element>
	 <element contentid="tab2">
	 	<position left="153" top="309" width="105" height="33"/> 
	 </element>
</xsl:template>


</xsl:stylesheet>
