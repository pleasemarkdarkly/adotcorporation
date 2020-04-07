<!DOCTYPE xsl:stylesheet [ <!ENTITY nbsp "&#160;"> ]>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:StringLookup="urn:fullplay-schema-com:StringLookup"
xmlns:Style="urn:fullplay-schema-com:Style">

<!--<xsl:import href="common/customer_content.xsl"/>-->
	<xsl:output method="xml" indent="yes" encoding="iso-8859-1"/>
	<xsl:template match="/*">
	
	
	
	
	
	<!--begin screen layout-->
		<screen type="replay" history="yes">
	<layout>
		<background>Replay_Background.png</background>
		
		
		
		<list contentid="listcontent" select="grid" itemsize="80" focus="true" bgcolor="255,255,255" rows="3" columns="3">
		  <margins top="1" bottom="1" left="30" right="5" /> 
		  <cellspacing width="1" /> 
		  <border color="160,160,160" width="2" /> 
		  <position left="32" top="10" width="574" height="245" /> 
		  <arrows up="up_arrow.png" down="down_arrow.png" /> 
		<normal bgcolor="255,255,255">
		  <border color="204,204,204" width="0" rounded="true" /> 
		  <font color="0,0,0">Verdana11Bold</font> 
		  </normal>
		<highlighted bgcolor="255,255,255">
		  <border color="160,160,160" width="0" /> 
		  </highlighted>
		</list>

		<keyboard>
			
			<background>
				<image url = "keyb.gif" surl = "keyb_sel.gif"/>					
			</background>
			
			<text margin="5" align="left" bgcolor="51,51,51" maxchars="20">
			  <position left="42" top="260" width="247" height="36" /> 
			  <font color="255,255,255">Verdana12</font> 
			</text>
			
			<normal>
				<font color="255,255,255">Verdana12</font> 			
			</normal>
			
			<super>
				<font color="255,255,0">Verdana12</font> 
			</super>
			
			<keylist>
				<key>
					<position left="10" top="300" width="50" height="50"/>
					<action primary="Q" shift="1">1</action>
				</key>
				<key>
					<position left="65" top="300" width="50" height="50"/>
					<action primary="W" shift="2">1</action>
				</key>
				<key>
					<position left="120" top="300" width="50" height="50"/>
					<action primary="E" shift="3">1</action>
				</key>
				<key>
					<position left="175" top="300" width="50" height="50"/>
					<action primary="R" shift="">1</action>
				</key>
				<key>
					<position left="230" top="300" width="50" height="50"/>
					<action primary="T" shift="">1</action>
				</key>
				<key>
					<position left="285" top="300" width="50" height="50"/>
					<action primary="Y" shift="">1</action>
				</key>
				<key>
					<position left="340" top="300" width="50" height="50"/>
					<action primary="U" shift="">1</action>
				</key>
				<key>
					<position left="395" top="300" width="50" height="50"/>
					<action primary="I" shift="">1</action>
				</key>
				<key>
					<position left="450" top="300" width="50" height="50"/>
					<action primary="O" shift="">1</action>
				</key>
				<key>
					<position left="505" top="300" width="50" height="50"/>
					<action primary="P" shift="">1</action>
				</key>
				<key>
					<position left="560" top="300" width="50" height="50"/>
					<action primary="DEL" shift="">3</action>
				</key>
				<key>
					<position left="10" top="355" width="50" height="50"/>
					<action primary="A" shift="4">1</action>
				</key>
				<key>
					<position left="65" top="355" width="50" height="50"/>
					<action primary="S" shift="5">1</action>
				</key>
				<key>
					<position left="120" top="355" width="50" height="50"/>
					<action primary="D" shift="6">1</action>
				</key>
				<key>
					<position left="175" top="355" width="50" height="50"/>
					<action primary="F" shift="">1</action>
				</key>
				<key>
					<position left="230" top="355" width="50" height="50"/>
					<action primary="G" shift="">1</action>
				</key>
				<key>
					<position left="285" top="355" width="50" height="50"/>
					<action primary="H" shift="">1</action>
				</key>
				<key>
					<position left="340" top="355" width="50" height="50"/>
					<action primary="J" shift="">1</action>
				</key>
				<key>
					<position left="395" top="355" width="50" height="50"/>
					<action primary="K" shift="">1</action>
				</key>
				<key>
					<position left="450" top="355" width="50" height="50"/>
					<action primary="L" shift="">1</action>
				</key>
				<key>
					<position left="505" top="355" width="50" height="50"/>
					<action primary=";" shift="">1</action>
				</key>
				<key>
					<position left="560" top="355" width="50" height="50"/>
					<action primary="ENTER" shift="">2</action>
				</key>
				<key>
					<position left="10" top="410" width="50" height="50"/>
					<action primary="Z" shift="7">1</action>
				</key>
				<key>
					<position left="65" top="410" width="50" height="50"/>
					<action primary="X" shift="8">1</action>
				</key>
				<key>
					<position left="120" top="410" width="50" height="50"/>
					<action primary="C" shift="9">1</action>
				</key>
				<key>
					<position left="175" top="410" width="50" height="50"/>
					<action primary="V" shift="0">1</action>
				</key>
				<key>
					<position left="230" top="410" width="50" height="50"/>
					<action primary="B" shift="">1</action>
				</key>
				<key>
					<position left="285" top="410" width="50" height="50"/>
					<action primary="N" shift="">1</action>
				</key>
				<key>
					<position left="340" top="410" width="50" height="50"/>
					<action primary="M" shift="">1</action>
				</key>
				<key>
					<position left="395" top="410" width="50" height="50"/>
					<action primary="SPACE" shift="">4</action>
				</key>
				<key>
					<position left="450" top="410" width="50" height="50"/>
					<action primary="SHIFT" shift="">5</action>
				</key>
			</keylist>
			
		</keyboard>
	</layout>
	
	
			
	<content id="listcontent">
		
		<listitem>
		<cell align="left" width="10" /> 
		<cell align="left" width="100">
		  <image url="list/606949315825.jpg" /> 
		  </cell>
		<cell align="left" valign="top" width="180">
		  <vcell height="10" /> 
		<vcell valign="top" height="15">
		  <string>No Doubt</string> 
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
		  <string font="Verdana12">Rock Steady</string> 
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
		  <link>getItemDetail.mource=1itemid=brokenbrokenbroken</link> 
		  </listitem>
<listitem>
		<cell align="left" width="10" /> 
		<cell align="left" width="100">
		  <image url="list/606949315825.jpg" /> 
		  </cell>
		<cell align="left" valign="top" width="180">
		  <vcell height="10" /> 
		<vcell valign="top" height="15">
		  <string>No Doubt</string> 
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
		  <string font="Verdana12">Rock Steady</string> 
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
		  <link>getItemDetail.mource=1itemid=brokenbrokenbroken</link> 
		  </listitem>
		  <listitem>
		  		<cell align="left" width="10" /> 
		  		<cell align="left" width="100">
		  		  <image url="list/606949315825.jpg" /> 
		  		  </cell>
		  		<cell align="left" valign="top" width="180">
		  		  <vcell height="10" /> 
		  		<vcell valign="top" height="15">
		  		  <string>No Doubt</string> 
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
		  		  <string font="Verdana12">Rock Steady</string> 
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
		  		  <link>getItemDetail.mource=1itemid=brokenbrokenbroken</link> 
		  </listitem>
		  <listitem>
		  		<cell align="left" width="10" /> 
		  		<cell align="left" width="100">
		  		  <image url="list/606949315825.jpg" /> 
		  		  </cell>
		  		<cell align="left" valign="top" width="180">
		  		  <vcell height="10" /> 
		  		<vcell valign="top" height="15">
		  		  <string>No Doubt</string> 
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
		  		  <string font="Verdana12">Rock Steady</string> 
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
		  		  <link>getItemDetail.mource=1itemid=brokenbrokenbroken</link> 
		  </listitem>
		  <listitem>
		  		<cell align="left" width="10" /> 
		  		<cell align="left" width="100">
		  		  <image url="list/606949315825.jpg" /> 
		  		  </cell>
		  		<cell align="left" valign="top" width="180">
		  		  <vcell height="10" /> 
		  		<vcell valign="top" height="15">
		  		  <string>No Doubt</string> 
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
		  		  <string font="Verdana12">Rock Steady</string> 
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
		  		  <link>getItemDetail.mource=1itemid=brokenbrokenbroken</link> 
		  </listitem>
		  <listitem>
		  		<cell align="left" width="10" /> 
		  		<cell align="left" width="100">
		  		  <image url="list/606949315825.jpg" /> 
		  		  </cell>
		  		<cell align="left" valign="top" width="180">
		  		  <vcell height="10" /> 
		  		<vcell valign="top" height="15">
		  		  <string>No Doubt</string> 
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
		  		  <string font="Verdana12">Rock Steady</string> 
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
		  		  <link>getItemDetail.mource=1itemid=brokenbrokenbroken</link> 
		  </listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">A</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">B</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">C</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">D</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">E</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">F</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">G</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">H</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">I</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">J</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">K</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">L</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">M</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">N</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">O</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">P</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">Q</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">R</string> 
				  </cell>			 
		</listitem>
		<listitem>
				<cell align="left" width="100">
					 <string font="Verdana12">S</string> 
				  </cell>			 
		</listitem>
	</content>

	
</screen>
<!--end screen layout-->


	</xsl:template>

</xsl:stylesheet>
