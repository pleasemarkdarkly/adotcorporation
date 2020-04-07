<%@ Page language="c#" Codebehind="admin_reset.aspx.cs" AutoEventWireup="false" Inherits="Fullplay.admin_reset" %>
<screen type="admin_reset" admin="yes">
	<events>
		<buttons />
		<scan />
		<reboot />
	</events>
	<layout>
		<background><%=Style["BackgroundImage"]%></background>
		<element contentid="message" align="left" bgcolor="0,51,153">
			<font color="<%=Style["SubHeadingFontColor"]%>"><%=Style["SubHeadingFont"]%></font>
			<position left="0" top="60" width="320" height="20" />
		</element>
		<element contentid="description" align="center" bgcolor="255,153,0">
			<font color="<%=Style["DescriptionFontColor"]%>" ><%=Style["DescriptionFont"]%></font>
			<position left="0" top="82" width="320" height="20"/>
		</element>
		<element contentid="instructions" align="center" bgcolor="<%=Style["ListNormalBackgroundColor"]%>">
			<font color="<%=Style["ListNormalFontColor"]%>"><%=Style["ListNormalFont"]%></font>
			<position left="0" top="102" width="320" height="122"/>
		</element>
	</layout>
	<content id="message">
		<string  padding="5">MMD Reset</string>
	</content>
	<content id="description">
		<string>Rebooting, please wait.</string>
	</content>
	<content id="instructions">
		<string>Please wait while this device reboots...</string>
	</content>
</screen>