<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:fo="http://www.w3.org/1999/XSL/Format">

<xsl:template match="mmd-adminevents/mmd-adminevent">
	<listitem>
		<string  padding="5"><xsl:value-of select="@name"/></string>
		<xsl:choose>
		<xsl:when test="@id = 0">
			<link>getHome.m?Logout=1</link>
		</xsl:when>
		<xsl:otherwise>
			<link>admin_event.aspx?EventID=<xsl:value-of select="@id"/></link>
		</xsl:otherwise>
		</xsl:choose>
	</listitem>
</xsl:template>

</xsl:stylesheet>