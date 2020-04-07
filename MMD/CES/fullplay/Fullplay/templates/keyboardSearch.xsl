<!DOCTYPE xsl:stylesheet [ <!ENTITY nbsp "&#160;"> ]>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:StringLookup="urn:fullplay-schema-com:StringLookup"
xmlns:Style="urn:fullplay-schema-com:Style">
	<xsl:import href="buildMenuContent.xsl"/>
	<xsl:output method="xml" indent="yes" encoding="iso-8859-1"/>
	<xsl:template match="/">
		<!--begin screen layout-->
		<screen type="replay" history="yes">
	<layout>

			
		<background>fullplay/Fullplay_background.png</background>			
		
		<!-- top bar elements -->
		<xsl:call-template name="placeMenuContent" />	     
		<!-- top bar elements end -->

		<block contentid="searchstr">
			<position left="40" top="278" width="100" height="39" />
			<arrows up="fullplay/scrollup_up.png" down="fullplay/scrolldown_up.png" /> 
			<font color="0,0,0">Trebuchet16Bold</font> 
            		<border color="0,255,0" width="0" /> 
		</block>

		
		<element contentid="searchtextbox">
			<position left="122" top="272" width="423" height="39" />
		</element>
			
		<element contentid="searchresbg">
			<position left="13" top="97" width="617" height="176" /> 
		</element>
	
		<list contentid="listcontent" select="grid" itemsize="80" focus="true" bgcolor="255,255,255" rows="2" columns="3">
		  <cellspacing width="1" /> 
		  <border color="160,160,160" width="2" /> 
		  <position left="20" top="105" width="600" height="160" /> 
		  <arrows up="fullplay/scrollup_up.png" down="fullplay/scrolldown_up.png" /> 
		<normal bgcolor="255,255,255">
		  <border color="204,204,204" width="0" rounded="true" /> 
		  <font color="0,0,0">Verdana11Bold</font> 
		  </normal>
		<highlighted bgcolor="255,255,255">
		  <border color="160,160,160" width="0" /> 
		  </highlighted>
		</list>

		<keyboard>
		
			<text margin="5" align="left" bgcolor="255,255,255" maxchars="20">
			  <position left="130" top="278" width="400" height="24" /> 
			  <font color="0,0,0">Verdana16Bold</font> 
			</text>
			
			
			<checkbox id="0">
				<position left="378" top="66" width="85" height="31" />
				<images unselected="fullplay/tab_artist_up_378_66_85_31.png" selected="fullplay/tab_artist_high_378_66_85_31.png"/>
			</checkbox>
					
			<checkbox id="1">
				 <position left="463" top="66" width="83" height="31" />
				 <images unselected="fullplay/Tab_album_up_463_66_83_31.png" selected="fullplay/Tab_album_high_463_66_83_31.png"/>
			</checkbox>

			<checkbox id="2">
				 <position left="546" top="66" width="83" height="31" />
				 <images unselected="fullplay/Tab_song_up_546_66_83_31.png" selected="fullplay/Tab_song_high_546_66_83_31.png"/>
			</checkbox>
			
			
			<normal>
				<font color="0,0,0">Trebuchet16Bold</font> 			
			</normal>
			
			<super>
				<font color="255,255,255">Trebuchet16Bold</font> 
			</super>
			
			<keylist>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="10" top="317" width="53" height="51"/>
				<action primary="Q" shift="1">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="63" top="317" width="53" height="51"/>
				<action primary="W" shift="2">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="116" top="317" width="53" height="51"/>
				<action primary="E" shift="3">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="169" top="317" width="53" height="51"/>
				<action primary="R" shift="4">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="222" top="317" width="53" height="51"/>
				<action primary="T" shift="5">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="275" top="317" width="53" height="51"/>
				<action primary="Y" shift="6">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="328" top="317" width="53" height="51"/>
				<action primary="U" shift="7">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="381" top="317" width="53" height="51"/>
				<action primary="I" shift="8">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="434" top="317" width="53" height="51"/>
				<action primary="O" shift="9">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="487" top="317" width="53" height="51"/>
				<action primary="P" shift="0">1</action>
			</key>
			<key>
			<image url = "fullplay/button_big_upstate_93_51.png" surl = "fullplay/button_big_downstate_93_51.png"/>
				<position left="540" top="317" width="91" height="51"/>
				<action primary="DEL" shift="">3</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="35" top="368" width="53" height="51"/>
				<action primary="A" shift="">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="88" top="368" width="53" height="51"/>
				<action primary="S" shift="">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="141" top="368" width="53" height="51"/>
				<action primary="D" shift="">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="194" top="368" width="53" height="51"/>
				<action primary="F" shift="">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="247" top="368" width="53" height="51"/>
				<action primary="G" shift="">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="300" top="368" width="53" height="51"/>
				<action primary="H" shift="">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="353" top="368" width="53" height="51"/>
				<action primary="J" shift="">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="406" top="368" width="53" height="51"/>
				<action primary="K" shift="">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="459" top="368" width="53" height="51"/>
				<action primary="L" shift="">1</action>
			</key>
			<key>
			<image url = "fullplay/button_big_upstate_93_51.png" surl = "fullplay/button_big_downstate_93_51.png"/>
				<position left="512" top="368" width="91" height="51"/>
				<action primary="ENTER" shift="">2</action>
			</key>
			<key>
			<image url = "fullplay/button_big_upstate_93_51.png" surl = "fullplay/button_big_downstate_93_51.png"/>
				<position left="45" top="419" width="91" height="51"/>
				<action primary="SHIFT" shift="">5</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="136" top="419" width="53" height="51"/>
				<action primary="Z" shift="">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="189" top="419" width="53" height="51"/>
				<action primary="X" shift="">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="242" top="419" width="53" height="51"/>
				<action primary="C" shift="">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="295" top="419" width="53" height="51"/>
				<action primary="V" shift="">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="348" top="419" width="53" height="51"/>
				<action primary="B" shift="">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="401" top="419" width="53" height="51"/>
				<action primary="N" shift="">1</action>
			</key>
			<key>
			<image url = "fullplay/button_small_upstate_53_51.png" surl = "fullplay/button_small_downstate_53_51.png"/>
				<position left="454" top="419" width="53" height="51"/>
				<action primary="M" shift="">1</action>
			</key>
			<key>
			<image url = "fullplay/button_big_upstate_93_51.png" surl = "fullplay/button_big_downstate_93_51.png"/>
				<position left="507" top="419" width="91" height="51"/>
				<action primary="SPACE" shift="">4</action>
</key>

			</keylist>
			
		</keyboard>
	</layout>

	<xsl:call-template name="buildMenuContent">	     
	     <xsl:with-param name="browse" select="'false'"/>
	     <xsl:with-param name="movies" select="'false'"/>
	     <xsl:with-param name="music" select="'true'"/>
	     <xsl:with-param name="search" select="'true'"/>
	     <xsl:with-param name="games" select="'false'"/>
	</xsl:call-template>

	<content id="searchstr">
	<string>Search:</string>
	</content>

	<content id="searchtextbox">
	  <image url="fullplay/Search_textbox_122_272_423_39.png" />
	</content>
	<content id="searchresbg">
	  <image url="fullplay/search_box_12_97_617_176.png" />
	</content>
	<content id="listcontent">
		<xsl:apply-templates select="//Search/searchlist"/>
	</content>
</screen>
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
