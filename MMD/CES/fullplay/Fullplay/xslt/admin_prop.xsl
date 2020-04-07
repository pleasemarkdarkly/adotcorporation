<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" >

<xsl:param name="EventID" select="4"/>

<xsl:template match="mmd-property">
	<content>
			<xsl:attribute name="id">listcontent</xsl:attribute>
	<xsl:choose>
		<xsl:when test="@multiselect!='True'">
			<xsl:attribute name="link">admin_setprop.aspx?EventID=<xsl:value-of select="$EventID"/>&amp;PropID=<xsl:value-of select="@id"/></xsl:attribute>
			<xsl:apply-templates select="mmd-propvalue[@selected = 'True']" />
			<xsl:apply-templates select="mmd-propvalue[@selected != 'True']" />
		</xsl:when>
		<xsl:otherwise>
			<xsl:apply-templates select="mmd-propvalue" />
		</xsl:otherwise>
		</xsl:choose>
	</content>
</xsl:template>
	
<xsl:template match="mmd-property[@multiselect != 'True']/mmd-propvalue">
	<xsl:choose>
		<xsl:when test="@selected='True'">
			<listitem selected="true">
				<string  padding="4"><xsl:value-of select="@name"/></string>
				<link>admin_setprop.aspx?EventID=<xsl:value-of select="$EventID"/>&amp;PropID=<xsl:value-of select="../@id"/>&amp;Values=<xsl:value-of select="@id"/></link>
			</listitem>
		</xsl:when>
		<xsl:otherwise>
			<listitem>
				<string padding="4"><xsl:value-of select="@name"/></string>
				<link>admin_setprop.aspx?EventID=<xsl:value-of select="$EventID"/>&amp;PropID=<xsl:value-of select="../@id"/>&amp;Values=<xsl:value-of select="@id"/></link>
		</listitem>
		</xsl:otherwise>
	</xsl:choose>
</xsl:template>

<xsl:template match="mmd-property[@multiselect = 'True']/mmd-propvalue">
	<listitem>
		<xsl:attribute name="value"><xsl:value-of select="@id"/></xsl:attribute>
		<xsl:attribute name="selected"><xsl:value-of select="@selected"/></xsl:attribute>
		<string padding="4"><xsl:value-of select="@name"/></string>
	</listitem>
</xsl:template>


<xsl:template match="mmd-propvalue">
	<listitem>
		<string><xsl:value-of select="../@name"/></string> 
		<string><xsl:value-of select="@name"/></string> 
		<link>admin_setprop.aspx?EventID=<xsl:value-of select="$EventID"/>&amp;PropID=<xsl:value-of select="../@id"/>&amp;Values=<xsl:value-of select="@id"/></link>
	</listitem>
</xsl:template>

</xsl:stylesheet>
