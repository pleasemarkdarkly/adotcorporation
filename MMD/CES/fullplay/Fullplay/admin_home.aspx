<%@ Page language="c#" Codebehind="admin_home.aspx.cs" AutoEventWireup="false" Inherits="Fullplay.admin_home" %>
<screen type="admin_home" admin="yes" history="add">
	<events>
		<% if (Reboot != "") { %>
		<reboot />
		<% } %>
		<% if (IsSetLocation) { %>
		<setlocation><%=NewLocationID%></setlocation>
		<set_property value="<%=DefaultVolume%>" name="default_volume" />
		<set_property value="<%=SecsUntilIdle%>" name="default_idle_timeout" />
		<% } %>
	</events>
	<layout>
		<background><%=Style["BackgroundImage"]%></background>
		<element align="left" contentid="slogan">
			<font color="<%=Style["TitleFontColor"]%>"><%=Style["TitleFont"]%></font>
			<position height="16" width="250" top="0" left="0" />
		</element>
		<element align="left" contentid="welcome">
			<font color="<%=Style["HeadingFontColor"]%>"><%=Style["HeadingFont"]%></font>
			<position height="16" width="320" top="37" left="0" />
		</element>
		<element align="left" contentid="description">
			<font color="<%=Style["DescriptionFontColor"]%>"><%=Style["DescriptionFont"]%></font>
			<position height="28" width="320" top="54" left="0" />
		</element>
		<list contentid="listcontent" select="single">
			<visible>7</visible>
			<position height="146" width="320" top="85" left="0" />
			<normal bgcolor="<%=Style["ListNormalBackgroundColor"]%>">
				<font color="<%=Style["ListNormalFontColor"]%>"><%=Style["ListNormalFont"]%></font>
			</normal>
			<selected>
				<border width="<%=Style["BorderWidth"]%>" scolor="<%=Style["BorderSelectedColor"]%>" ucolor="<%=Style["BorderNormalColor"]%>" />
			</selected>
			<highlighted bgcolor="<%=Style["ListHighlightedBackgroundColor"]%>">
				<font color="<%=Style["ListHighlightedFontColor"]%>"><%=Style["ListHighlightedFont"]%></font>
			</highlighted>
			<cellspacing width="<%=Style["SpacingSize"]%>" color="<%=Style["SpacingColor"]%>" />
		</list>
	</layout>
	<content id="slogan">
		
			<string padding="5">Admin Home</string>
		
	</content>
	<content id="welcome">
		
			<string  padding="5">Welcome Administrator!</string>
		
	</content>
	<content id="description">
		
			<string  padding="5">Pick category and press select to change settings.|This device's current location ID is <%=LocationID%> .</string>
		
	</content>
	<content id="listcontent">
		<%
	Response.Write (XML);
		%>
		<% if (AddRebootToList) { %>
		<listitem>
			
				<string  padding="5">Reset Device</string>
			
			<!--<link>/admin_home.asp?Reboot=1</link>--><link>/mmd/admin_reset.aspx</link>
		</listitem>
		<listitem>
			
				<string  padding="5">Deactivate Device</string>
			
			<link>admin_deactivate.aspx?LocationID=<%=LocationID%></link>
		</listitem>
		<listitem>
			
				<string  padding="5">Run Network Test</string>
			
			<link>admin_NetTest.aspx?LocationID=<%=LocationID%></link>
		</listitem>
		<% } %>
	</content>
</screen>
