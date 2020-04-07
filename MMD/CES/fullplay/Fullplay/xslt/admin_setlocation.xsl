<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" >

<xsl:template match="/">
	<xsl:for-each select="/info/locations/location">
		<listitem>
		<string padding="5"><xsl:value-of select="@id"/></string>
		<link>admin_home.aspx?SetLocation=<xsl:value-of select="@id"/><xsl:for-each select="/info/mac"><xsl:value-of select="@address"/></xsl:for-each>
		</link>
	</listitem>
	</xsl:for-each>
</xsl:template>
</xsl:stylesheet>