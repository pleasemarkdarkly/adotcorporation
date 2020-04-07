<%@ Page language="c#" Codebehind="WeekDateRange.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Reports.WeekDateRange" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" > 

<html>
  <head>
    <title>WeekDateRange</title>
    <meta name="GENERATOR" Content="Microsoft Visual Studio 7.0">
    <meta name="CODE_LANGUAGE" Content="C#">
    <meta name=vs_defaultClientScript content="JavaScript">
    <meta name=vs_targetSchema content="http://schemas.microsoft.com/intellisense/ie5">
  </head>
  <body MS_POSITIONING="GridLayout">

<!-- BEGIN IFRAME AND SCRIPT BLOCK FOR CALENDAR - CHANGE SRC ATTRIBUTES AS REQUIRED -->
<IFRAME STYLE="display:none;position:absolute;height: 500px;z-index=100" ID="CalFrame" 
	MARGINHEIGHT=0 MARGINWIDTH=0 NORESIZE FRAMEBORDER=0 SCROLLING=NO SRC="/CentralWeb/Common/Calendar/calendar.htm"></IFRAME>
<SCRIPT LANGUAGE="javascript" SRC="/CentralWeb/Common/Calendar/calendar.js"></SCRIPT>
<SCRIPT FOR=document EVENT="onclick()">
<!--
document.all.CalFrame.style.display="none";
//-->
</SCRIPT>
<!-- END -->
	
    <form name=frmMain id="WeekDateRange" method="post" action="WeekDateRange.aspx">
		<input type="hidden" name="post" value="1" >

		<table>
		<tr><td>Start Date</td>
		<td><INPUT class=text3 maxLength=12 name=date1 size=9 value="9/01/02">
		<A href="javascript:ShowCalendar(document.frmMain.dimg1,document.frmMain.date1,null, null, null)" 
			onclick=event.cancelBubble=true;>
		<IMG align=top border=0 height=21 id=dimg1 src="/CentralWeb/Common/Calendar/calendar.gif" style="POSITION: relative" width=34></A>
		</td>
		</tr>
		<tr><td>End Date</td>
		<td><INPUT class=text3 maxLength=12 name=date2 size=9 value="10/01/02">
		<A href="javascript:ShowCalendar(document.frmMain.dimg2,document.frmMain.date2,null, null, null)" 
			onclick=event.cancelBubble=true;>
		<IMG align=top border=0 height=21 id=dimg2 src="/CentralWeb/Common/Calendar/calendar.gif" style="POSITION: relative" width=34></A>
		</td>
		</tr>
		<tr><td>Reports</td><td><%=GetReports()%></td></tr>
		<tr><td><input type=submit value="Submit"></td></tr>
		</table>		
     </form>
	
<hr>
<% WriteResults(); %>
  </body>
</html>
