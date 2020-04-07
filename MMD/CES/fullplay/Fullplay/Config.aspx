<%@ Page language="c#" Codebehind="Config.aspx.cs" AutoEventWireup="false" Inherits="Fullplay.Config" %>
<%

			Response.ContentType = "text/xml";
			Response.Charset = "";
			//HACK - this encoding type may not support all of latin 1, but it works in the meantime.
			Response.ContentEncoding = System.Text.Encoding.GetEncoding("windows-1252");
			Response.Write("<?xml version=\"1.0\" encoding=\"iso-8859-1\" ?>");


%>
<config>
	<location><%=LocationID%></location>
	<%
	bool bIsInitialized = this.IsInitialized;
	
%>
	<%	if(bIsInitialized) { %>
	<volume>
		<%=DefaultVolume%>
	</volume>
	<brightness>
		<%=DefaultBrightness%>
	</brightness>
	<idleloop>
		<%=IdleLoopID%>
	</idleloop>
	<idletimeout>
		<%=SecsUntilIdle%>
	</idletimeout>
	
	<% } %>


	<appinfo>
		<entrylist virtualdir="/Fullplay">
			
			<entry id="scan" url="scan.m" />
			<entry id="log" url="storelogs.aspx" />
			<!--<entry id="explicit" url="mmd.aspx" />-->
			<entry id="touch" url="calibrate.aspx"/>
			<entry id="home" url="getHome.m" />
			<entry id="idleloop" url="idleloop.aspx" />
			<entry id="nettest" url="AdminNetTest.m" />
			<entry id="unlock" url="unlock.m" />
		</entrylist>
		<resourcelist>
			<fontlist virtualdir="/MMDResources/fonts"></fontlist>
			<imagelist virtualdir="/MMDResources/images">
				<image id="activeup" url="double_up_arrows.bmp" />
				<image id="activedown" url="double_down_arrows.bmp" />
				<image id="lastup" url="grey_up_arrows.bmp" />
				<image id="lastdown" url="grey_down_arrows.bmp" />
			</imagelist>
			<soundlist virtualdir="/MMDResources/sounds">
				<sound id="scan" url="beep.wav" />
			</soundlist>
		</resourcelist>
	</appinfo>
		
	<% if (IsInitialized && IsCalibrated) { %>
	
	<calibrations>
		<cal id="1" xScr="<%=Calibrations["xScr1"]%>" yScr="<%=Calibrations["yScr1"]%>" xCal="<%=Calibrations["xCal1"]%>" yCal="<%=Calibrations["yCal1"]%>"/>
		<cal id="2" xScr="<%=Calibrations["xScr2"]%>" yScr="<%=Calibrations["yScr2"]%>" xCal="<%=Calibrations["xCal2"]%>" yCal="<%=Calibrations["yCal2"]%>"/>
		<cal id="3" xScr="<%=Calibrations["xScr3"]%>" yScr="<%=Calibrations["yScr3"]%>" xCal="<%=Calibrations["xCal3"]%>" yCal="<%=Calibrations["yCal3"]%>"/>
		<cal id="4" xScr="<%=Calibrations["xScr4"]%>" yScr="<%=Calibrations["yScr4"]%>" xCal="<%=Calibrations["xCal4"]%>" yCal="<%=Calibrations["yCal4"]%>"/>
		<cal id="5" xScr="<%=Calibrations["xScr5"]%>" yScr="<%=Calibrations["yScr5"]%>" xCal="<%=Calibrations["xCal5"]%>" yCal="<%=Calibrations["yCal5"]%>"/>
	</calibrations>
	
	<%}%>
	<screen type="default">
		<% if(DefaultScreenURL == "") { %>
		<layout>
			<background>2-00.jpg</background>
		</layout>
		<% }%>
	</screen>
</config>