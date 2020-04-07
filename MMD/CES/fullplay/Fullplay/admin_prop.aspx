<%@ Page language="c#" Codebehind="admin_prop.aspx.cs" AutoEventWireup="false" Inherits="Fullplay.admin_prop" %>
<!-- Admin Event Screen -->
<screen type="admin_prop" admin="yes" history="add">
	<layout>
		<background><%=Style["BackgroundImage"]%></background>
		<image url="<%=Style["BackTabImage"]%>">
			<position left="0" top="8" width="55" height="22" />
		</image>
		<element contentid="welcome" align="left">
			<font color="<%=Style["HeadingFontColor"]%>"><%=Style["HeadingFont"]%></font>
			<position left="0" top="37" width="320" height="16" />
		</element>
		<element contentid="description" align="left">
			<font color="<%=Style["DescriptionFontColor"]%>"><%=Style["DescriptionFont"]%></font>
			<position left="0" top="54" width="320" height="28"/>
		</element>
		<list contentid="listcontent" <%=ListMode%>>
			<visible>7</visible>
			<position left="0" top="85" width="320" height="146"/>
			<normal bgcolor="<%=Style["ListNormalBackgroundColor"]%>">
				<font color="<%=Style["ListNormalFontColor"]%>"><%=Style["ListNormalFont"]%></font>
			</normal>
			<selected>
				<border ucolor="<%=Style["BorderNormalColor"]%>" scolor="<%=Style["BorderSelectedColor"]%>" width="<%=Style["BorderWidth"]%>"/>
			</selected>
			<highlighted bgcolor="<%=Style["ListHighlightedBackgroundColor"]%>">
				<font color="<%=Style["ListHighlightedFontColor"]%>"><%=Style["ListHighlightedFont"]%></font>
			</highlighted>
			<cellspacing color="<%=Style["SpacingColor"]%>" width="<%=Style["SpacingSize"]%>" />
		</list>
	</layout>
	<content id="welcome">
		<string padding="5"><%=Heading%></string>
	</content>
	<content id="description">
		<string  padding="5"><%=Description%></string>
	</content>	
<% Transform(); %>
</screen>


