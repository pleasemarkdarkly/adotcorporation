<!DOCTYPE xsl:stylesheet [ <!ENTITY nbsp "&#160;"> ]>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
xmlns:StringLookup="urn:fullplay-schema-com:StringLookup" xmlns:Ext="xslext"
xmlns:Style="urn:fullplay-schema-com:Style">
	<xsl:output method="xml" indent="yes" encoding="iso-8859-1"/>
	
	<xsl:template name="buildMenuContent">
		<xsl:param name="browse" select='false'/>
		<xsl:param name="music" select='false'/>
		<xsl:param name="movies" select='false'/>
		<xsl:param name="games" select='false'/>
		<xsl:param name="search" select='false'/>
		
			<content id="browse">
			<xsl:choose>
				<xsl:when test="$browse='true'"> <image url="fullplay/tab_browse_high_42_48_164_40.PNG" /> </xsl:when>
				<xsl:otherwise> 
				<image url="fullplay/tab_browse_up_42_48_164_40.PNG" surl="fullplay/tab_browse_down_42_48_164_40.PNG"/>
				<link>getGenres.m?mediaid=1</link>
			 </xsl:otherwise>
			</xsl:choose>
			</content>
			
		
			<content id="music">
			<xsl:choose>
				<xsl:when test="$music='true'"><image url="fullplay/tab_music_high_42_0_106_48.png" /><link>getGenres.m?mediaid=1</link></xsl:when>
				<xsl:otherwise>  
					<image url="fullplay/tab_music_up_42_0_106_48.png" surl="fullplay/tab_music_down_42_0_106_48.png"/>  
					<link>getGenres.m?mediaid=1</link>
				</xsl:otherwise>
			</xsl:choose>			  			  
			</content>
		
			<content id="movies">
			<xsl:choose>
				<xsl:when test="$movies='true'"> <image url="fullplay/tab_movies_high_148_0_109_48.png" /><link>getGenres.m?mediaid=13</link> </xsl:when>
				<xsl:otherwise> 
					<image url="fullplay/tab_movies_up_148_0_109_48.png" surl="fullplay/tab_movies_down_148_0_109_48.png" /> 
					<link>getGenres.m?mediaid=13</link>
				</xsl:otherwise>
			</xsl:choose>			  
			</content>
		
			<content id="games">
			<xsl:choose>
				<xsl:when test="$games='true'"> <image url="fullplay/tab_games_high_257_0_118_48.PNG" /> <link>getGenres.m?mediaid=10</link> </xsl:when>
				<xsl:otherwise> 
					<image url="fullplay/tab_games_up_257_0_118_48.PNG" surl="fullplay/tab_games_down_257_0_118_48.PNG"/>  
					<link>getGenres.m?mediaid=10</link>
				</xsl:otherwise>
			</xsl:choose>				  
			</content>
		
			<content id="search">
			<xsl:choose>
				<xsl:when test="$search='true'"> <image url="fullplay/tab_search_high_206_48_169_40.PNG" />  </xsl:when>
				<xsl:otherwise> 
					<image url="fullplay/tab_search_up_206_48_169_40.PNG" surl="fullplay/tab_search_down_206_48_169_40.PNG"/> 
				</xsl:otherwise>
			</xsl:choose>	
			  <link>Search.m</link>
			</content>
		
			<content id="back">
			  <image url="fullplay/Button_back_0_0_42_50.PNG" /> 
			</content>
	
	</xsl:template>

	<xsl:template name="placeMenuContent">
		 <element contentid="search">
		  <position left="206" top="48" width="169" height="40"/> 
		  </element>
		 <element contentid="browse">
		  <position left="42" top="48" width="164" height="40" /> 
		  </element>
		 <element contentid="games">
		  <position left="257" top="0" width="118" height="48" /> 
		  </element>
		 <element contentid="movies">
		  <position left="148" top="0" width="109" height="48" /> 
		  </element>
		 <element contentid="music">
		  <position left="42" top="0" width="109" height="48"/> 
		</element>
		 <element contentid="back">
		  <position left="0" top="0" width="42" height="50"/> 
		</element>
	</xsl:template>

</xsl:stylesheet>
