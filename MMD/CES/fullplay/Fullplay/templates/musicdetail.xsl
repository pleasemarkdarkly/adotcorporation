<!DOCTYPE xsl:stylesheet [ <!ENTITY nbsp "&#160;"> ]>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:StringLookup="urn:fullplay-schema-com:StringLookup"
xmlns:Style="urn:fullplay-schema-com:Style">
	
	<xsl:import href="buildMenuContent.xsl"/>

	<xsl:output method="xml" indent="yes" encoding="iso-8859-1"/>
	
	<xsl:template match="/">
		<!--begin screen layout-->
		<screen type="browse" history="yes">
	<layout>
		<background>fullplay/Fullplay_background.png</background>
		
		<!-- top bar elements -->
		<xsl:call-template name="placeMenuContent" />	     
		<!-- top bar elements end -->
				
		<element contentid="tracklistbg">
		  <position left="12" top="129" width="364" height="170"/> 
		  
		</element>
		
		<element contentid="detailbox">
		  <position left="12" top="342" width="617" height="127"/> 
		</element>
		
		<xsl:call-template name="placeDetailMenuContent" />
			
		<element contentid="similar1">
			<position left="30" top="345" width="120" height="110"/> 
		</element>

		<element contentid="similar2">
			<position left="136" top="345" width="120" height="110"/> 
		</element>

		<element contentid="similar3">
			<position left="256" top="345" width="120" height="110"/> 
		</element>

		<element contentid="similar4">
			<position left="376" top="345" width="120" height="110"/> 
		</element>	

		<element contentid="similar5">
			<position left="496" top="345" width="120" height="110"/> 
		</element>	

		
		<element contentid="albumart">
			<position left="440" top="140" width="162" height="162"/> 		
		</element>
		
        <block contentid="reviewtext" margin="0" focus="true">
            <position left="20" top="350" width="600" height="110"/> 
      	    <arrows up="fullplay/scrollup_up.png" down="fullplay/scrolldown_up.png" /> 
            <font color="0,0,0">Verdana11</font> 
            <border color="0,255,0" width="0" /> 
        </block>
               
        <list contentid="discography" select="grid" itemsize="80" focus="true" bgcolor="255,255,255" rows="1" columns="3">
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
		</list>

        		
		<list contentid="listcontent" select="single" itemheight="30" focus="true" bgcolor="255,255,255">
		<margins top="2" bottom="2" left="2" right="2" /> 
			<cellspacing width="3" /> 
			<position left="20" top="136" width="348" height="152" /> 
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

		
		<block contentid="albumtext">
			<position left="30" top="90" width="200" height="100"/> 
		  <font color="0,0,0">Verdana12bold</font> 

		</block>
		
		<block contentid="albumprice">
			<position left="260" top="95" width="200" height="100"/> 
				  <font color="0,0,0">Verdana12bold</font> 

		</block>
	
	
		<block contentid="albuminfo">
			<position left="440" top="80" width="200" height="100"/> 
			<font color="0,0,0">Verdana12bold</font> 					
		</block>
	</layout>
	
	<content id="listcontent">
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
  			<string padding="16">Speak to Me/Breathe in the Air </string> 
  			</cell>
		
		</listitem>
		
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
		  			<string padding="16">On the Run </string> 
		  			</cell>
				
		</listitem>
		
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
		  			<string padding="16">Time </string> 
		  			</cell>
				
		</listitem>
		
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
		  			<string padding="16">The Great Gig in the Sky </string> 
		  			</cell>
				
		</listitem>
		
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
		  			<string padding="16">Money</string> 
		  			</cell>
				
		</listitem>
		
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
				  			<string padding="16">Us and Them</string> 
				  			</cell>
						
		</listitem>
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
				  			<string padding="16">Any Colour You Like</string> 
				  			</cell>
						
		</listitem>
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
				  			<string padding="16">Brain Damage </string> 
				  			</cell>
						
		</listitem>
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
				  			<string padding="16">Eclipse</string> 
				  			</cell>
						
		</listitem>
				
	</content>
	
	<content id="albumart">
		<image url="fullplay/no_music.png"/>
	</content>

	<content id="albumtext">
 		<string font="Verdana16Bold">Dark Side of the Moon\n</string> 
 		<string font="Verdana12Bold">Pink Floyd</string> 
	</content>

	<content id="albumprice">
		<string font="Verdana24Bold">$666.66</string>
	</content>


	<content id="albuminfo">
		<string font="Verdana12">genre: </string>
		<string font="Verdana12Bold">Boring\n</string>
		<string font="Verdana12">release date: </string>
		<string font="Verdana12Bold">06/06/79\n</string>
		<string font="Verdana12">label: </string>
		<string font="Verdana12Bold">Old People\n</string>
	</content>
	
	<!-- top bar content -->
	     
	     
	<xsl:call-template name="buildMenuContent">	     
	     <xsl:with-param name="browse" select="'false'"/>
	     <xsl:with-param name="movies" select="'false'"/>
	     <xsl:with-param name="music" select="'true'"/>
	     <xsl:with-param name="search" select="'false'"/>
	     <xsl:with-param name="games" select="'false'"/>
	</xsl:call-template>

	<xsl:call-template name="buildDetailMenuContent">	     
		<xsl:with-param name="selectedTab" select="'1'"/>	     
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

	<content id="tracklistbg">
	  <image url="fullplay/detail_box_12_129_364_170.PNG" /> 
	</content>
	

	<!-- elements placed for similar items section-->

<!--
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
	
-->

<content id="discography">
<listitem>
  <cell align="left" width="10" /> 
<cell align="left" width="100">
  <image url="fullplay\no_music_thumb.png" /> 
  </cell>
<cell align="left" valign="top" width="180">
  <vcell height="10" /> 
<vcell valign="top" height="15">
  <string>Pink Floyd</string> 
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
  <string font="Verdana12">Gift Set</string> 
  </cell>
  </vcell>
<vcell valign="top" height="15">
<normal>
  <font>Verdana12</font> 
  </normal>
<highlighted>
  <font>Verdana12</font> 
  </highlighted>
<selected>
  <font>Verdana12</font> 
  </selected>
  <string>$16.99</string> 
  </vcell>
  </cell>
  <cell /> 
  <link>getItemDetail.m?itemid=10120748</link> 
  </listitem>
<listitem>
  <cell align="left" width="10" /> 
<cell align="left" width="100">
  <image url="fullplay\no_music_thumb.png" /> 
  </cell>
<cell align="left" valign="top" width="180">
  <vcell height="10" /> 
<vcell valign="top" height="15">
  <string>Pink Floyd</string> 
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
  <string font="Verdana12">A Collection of Great Dance Songs</string> 
  </cell>
  </vcell>
<vcell valign="top" height="15">
<normal>
  <font>Verdana12</font> 
  </normal>
<highlighted>
  <font>Verdana12</font> 
  </highlighted>
<selected>
  <font>Verdana12</font> 
  </selected>
  <string>$16.99</string> 
  </vcell>
  </cell>
  <cell /> 
  <link>getItemDetail.m?itemid=10061225</link> 
  </listitem>
<listitem>
  <cell align="left" width="10" /> 
<cell align="left" width="100">
  <image url="fullplay\no_music_thumb.png" /> 
  </cell>
<cell align="left" valign="top" width="180">
  <vcell height="10" /> 
<vcell valign="top" height="15">
  <string>Pink Floyd</string> 
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
  <string font="Verdana12">A Collection of Great Dance Songs</string> 
  </cell>
  </vcell>
<vcell valign="top" height="15">
<normal>
  <font>Verdana12</font> 
  </normal>
<highlighted>
  <font>Verdana12</font> 
  </highlighted>
<selected>
  <font>Verdana12</font> 
  </selected>
  <string>$16.99</string> 
  </vcell>
  </cell>
  <cell /> 
  <link>getItemDetail.m?itemid=10322442</link> 
  </listitem>
<listitem>
  <cell align="left" width="10" /> 
<cell align="left" width="100">
  <image url="fullplay\no_music_thumb.png" /> 
  </cell>
<cell align="left" valign="top" width="180">
  <vcell height="10" /> 
<vcell valign="top" height="15">
  <string>Pink Floyd</string> 
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
  <string font="Verdana12">Meddle</string> 
  </cell>
  </vcell>
<vcell valign="top" height="15">
<normal>
  <font>Verdana12</font> 
  </normal>
<highlighted>
  <font>Verdana12</font> 
  </highlighted>
<selected>
  <font>Verdana12</font> 
  </selected>
  <string>$16.99</string> 
  </vcell>
  </cell>
  <cell /> 
  <link>getItemDetail.m?itemid=10116453</link> 
  </listitem>
<listitem>
  <cell align="left" width="10" /> 
<cell align="left" width="100">
  <image url="fullplay\no_music_thumb.png" /> 
  </cell>
<cell align="left" valign="top" width="180">
  <vcell height="10" /> 
<vcell valign="top" height="15">
  <string>Pink Floyd</string> 
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
  <string font="Verdana12">Meddle</string> 
  </cell>
  </vcell>
<vcell valign="top" height="15">
<normal>
  <font>Verdana12</font> 
  </normal>
<highlighted>
  <font>Verdana12</font> 
  </highlighted>
<selected>
  <font>Verdana12</font> 
  </selected>
  <string>$16.99</string> 
  </vcell>
  </cell>
  <cell /> 
  <link>getItemDetail.m?itemid=10120751</link> 
  </listitem>
<listitem>
  <cell align="left" width="10" /> 
<cell align="left" width="100">
  <image url="fullplay\no_music_thumb.png" /> 
  </cell>
<cell align="left" valign="top" width="180">
  <vcell height="10" /> 
<vcell valign="top" height="15">
  <string>Pink Floyd</string> 
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
  <string font="Verdana12">Obscured by Clouds</string> 
  </cell>
  </vcell>
<vcell valign="top" height="15">
<normal>
  <font>Verdana12</font> 
  </normal>
<highlighted>
  <font>Verdana12</font> 
  </highlighted>
<selected>
  <font>Verdana12</font> 
  </selected>
  <string>$16.99</string> 
  </vcell>
  </cell>
  <cell /> 
  <link>getItemDetail.m?itemid=10091921</link> 
  </listitem>
<listitem>
  <cell align="left" width="10" /> 
<cell align="left" width="100">
  <image url="fullplay\no_music_thumb.png" /> 
  </cell>
<cell align="left" valign="top" width="180">
  <vcell height="10" /> 
<vcell valign="top" height="15">
  <string>Pink Floyd</string> 
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
  <string font="Verdana12">The Piper at the Gates of Dawn</string> 
  </cell>
  </vcell>
<vcell valign="top" height="15">
<normal>
  <font>Verdana12</font> 
  </normal>
<highlighted>
  <font>Verdana12</font> 
  </highlighted>
<selected>
  <font>Verdana12</font> 
  </selected>
  <string>$16.99</string> 
  </vcell>
  </cell>
  <cell /> 
  <link>getItemDetail.m?itemid=10095261</link> 
  </listitem>
<listitem>
  <cell align="left" width="10" /> 
<cell align="left" width="100">
  <image url="fullplay\no_music_thumb.png" /> 
  </cell>
<cell align="left" valign="top" width="180">
  <vcell height="10" /> 
<vcell valign="top" height="15">
  <string>Pink Floyd</string> 
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
  <string font="Verdana12">The Piper at the Gates of Dawn</string> 
  </cell>
  </vcell>
<vcell valign="top" height="15">
<normal>
  <font>Verdana12</font> 
  </normal>
<highlighted>
  <font>Verdana12</font> 
  </highlighted>
<selected>
  <font>Verdana12</font> 
  </selected>
  <string>$16.99</string> 
  </vcell>
  </cell>
  <cell /> 
  <link>getItemDetail.m?itemid=10363326</link> 
  </listitem>
<listitem>
  <cell align="left" width="10" /> 
<cell align="left" width="100">
  <image url="fullplay\no_music_thumb.png" /> 
  </cell>
<cell align="left" valign="top" width="180">
  <vcell height="10" /> 
<vcell valign="top" height="15">
  <string>Pink Floyd</string> 
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
  <string font="Verdana12">A Saucerful of Secrets</string> 
  </cell>
  </vcell>
<vcell valign="top" height="15">
<normal>
  <font>Verdana12</font> 
  </normal>
<highlighted>
  <font>Verdana12</font> 
  </highlighted>
<selected>
  <font>Verdana12</font> 
  </selected>
  <string>$16.99</string> 
  </vcell>
  </cell>
  <cell /> 
  <link>getItemDetail.m?itemid=10095262</link> 
  </listitem>
<listitem>
  <cell align="left" width="10" /> 
<cell align="left" width="100">
  <image url="fullplay\no_music_thumb.png" /> 
  </cell>
<cell align="left" valign="top" width="180">
  <vcell height="10" /> 
<vcell valign="top" height="15">
  <string>Pink Floyd</string> 
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
  <string font="Verdana12">Ummagumma</string> 
  </cell>
  </vcell>
<vcell valign="top" height="15">
<normal>
  <font>Verdana12</font> 
  </normal>
<highlighted>
  <font>Verdana12</font> 
  </highlighted>
<selected>
  <font>Verdana12</font> 
  </selected>
  <string>$16.99</string> 
  </vcell>
  </cell>
  <cell /> 
  <link>getItemDetail.m?itemid=10095371</link> 
  </listitem>
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
				<image url="fullplay/Tab_disc_high_154_309_133_33.png" />
			</xsl:when>
			<xsl:otherwise>
				<image url="fullplay/Tab_disc_up_154_309_133_33.png" surl="fullplay/Tab_disc_down_154_309_133_33.png"/>
			</xsl:otherwise>
		</xsl:choose>
	</content>
	<content id="tab3">
		<xsl:choose>
			<xsl:when test="$selectedTab='3'">
				<image url="fullplay/Tab_review_high_287_309_104_33.png" />
			</xsl:when>
			<xsl:otherwise>
				<image url="fullplay/Tab_review_up_287_309_104_33.png" surl="fullplay/Tab_review_down_287_309_104_33.png" />
			</xsl:otherwise>
		</xsl:choose>
	</content>

</xsl:template>

<xsl:template name="placeDetailMenuContent">
	 <element contentid="tab1">
	 	<position left="12" top="309" width="142" height="33"/> 
	 </element>
	 <element contentid="tab2">
	 	<position left="154" top="309" width="133" height="33"/> 
	 </element>
	 <element contentid="tab3">
	 	<position left="287" top="309" width="104" height="33"/> 
	 </element>	
</xsl:template>


</xsl:stylesheet>
