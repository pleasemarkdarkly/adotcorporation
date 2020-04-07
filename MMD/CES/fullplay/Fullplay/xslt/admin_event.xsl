<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">

<xsl:param name="PropID" select="''" />

<xsl:template match="mmd-properties/mmd-property">
	<listitem>
		<cell width="150"><string  padding="5"><xsl:value-of select="@name"/></string></cell>
		<!-- last() is a fix for multiple items -->
		<cell align="right">
				<xsl:if test="@id = $PropID"><normal><font color="255,0,0"/></normal></xsl:if>
				<string padding="2"><xsl:for-each select="mmd-propvalue"><xsl:value-of select="@name"/>
			<xsl:if test="last() != position()"><xsl:text>, </xsl:text></xsl:if></xsl:for-each></string>
		</cell>
		<link>admin_prop.aspx?PropID=<xsl:value-of select="@id"/></link>
	</listitem>
</xsl:template>

</xsl:stylesheet>