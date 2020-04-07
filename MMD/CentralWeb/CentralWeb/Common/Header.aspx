<%@ Page language="c#" Codebehind="Header.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Common.Header" %>

<% Server.Execute("/CentralWeb/Common/HTMLHeader.aspx"); %>


<% if(CSWContext.ShowNavPane) { %>
<body marginheight="0" marginwidth="0" topmargin="0" leftmargin="0" background="/CentralWeb/img/circuitboard6.gif" link="#ffffff" vlink="#cccccc">
<table border=0 width=100% cellpadding=0 cellspacing=0>
<tr style="height: 0px"><td width="792" style="height: 0px"></td>
<td width="100%" style="height: 0px"></td>
</tr>
<tr><td colspan=2>

<table width="100%" height="22" border="0" cellpadding="0" cellspacing="0">
<tr>
    <td width="21" height="22"><img src="/CentralWeb/img/blue_left.gif" width="21" height="22" alt="" border="0"></td>
        
    <td width="100%" height="22" valign="middle" align="right" bgcolor="#000000"> 
      &nbsp;&nbsp;</td>
    <td width="20" height="22"><img src="/CentralWeb/img/blue_right.gif" width="20" height="22" alt=""></td>
</tr>

</table>


<table width="100%" height="60" border="0" cellpadding="0" cellspacing="0" bgcolor="#ffffff">
  <tr> 
    <td width="21" height="60"><img src="/CentralWeb/img/blue_left.gif" width=21 height=60 alt="" border="0"></td>
    <td width="373" height="60"><a href="../index.html"><img src="/CentralWeb/img/logo_fullplay_button.gif" width="373" height="60" border="0"></a></td>
    <td height="60">
      <table width="100%" border="0" cellspacing="0" cellpadding="0">
        <tr>
          <td><img src="/CentralWeb/img/Media_M_C.gif" width="375" height="60"></td>
          <td width="100%"><img src="/CentralWeb/img/Media_M_C_spacer.gif" width="100%" height="60"></td>
        </tr>
      </table>
    </td>
    <td width="20" height="60"><img src="/CentralWeb/img/blue_right.gif" width=20 height=60 alt="" border="0"></td>
  </tr>
</table>

<table width="100%" height="22" border="0" cellpadding="0" cellspacing="0">
<tr>
    <td width="21" height="22"><img src="/CentralWeb/img/blue_left.gif" width="21" height="22" alt="" border="0"></td>
        
    <td width="100%" height="22" valign="middle" align="right" bgcolor="#000000"> 
      &nbsp;&nbsp;</td>
    <td width="20" height="22"><img src="/CentralWeb/img/blue_right.gif" width="20" height="22" alt=""></td>
</tr>
<tr>
<td width="21" height="15"><img src="/CentralWeb/img/blue_left.gif" width="21" height="15" alt="" border="0"></td>
    <td width="749" height="15" valign="middle" align="right" bgcolor="#ffffff"><img src="/CentralWeb/img/clear.gif" width=749 height=15 alt="" border="0"></td>
<td width="20" height="15"><img src="/CentralWeb/img/blue_right.gif" width="20" height="15" alt=""></td>
</tr>
</table>

<table width="100%" border="0" cellpadding="0" cellspacing="0" bgcolor="#ffffff">
	<tr>
		<td width="21" background="/CentralWeb/img/blue_left.gif"><img src="/CentralWeb/img/clear.gif" width="21" alt="" border="0"></td>
		<td width="15"><img src="/CentralWeb/img/clear.gif" width="15" height="15"></td>
		<td width="100%" align="left" valign="top">


<table  bgcolor="#ffffff" border=0 cellpadding=0 cellspacing=0>
<% } else { %>
<body marginheight="0" marginwidth="0" topmargin="0" leftmargin="0" bgcolor="#ffffff" link="#ffffff" vlink="#cccccc">
<table  bgcolor="#ffffff" border=0 cellpadding=0 cellspacing=0>

<% } %>
<tr style="height: 0px"><td style="height: 0px"><div style="width: 150px; height: 0px"></div></td><td width="100%" style="height: 0px"></td></tr>
<tr><td width=100% colspan="2">

<span class="PageTitle"><%=CSWContext.PageTitle%></span><br>
&nbsp;<br>

</td></tr>
<tr>

<td width="150">&nbsp;</td>
<% if(CSWContext.ShowNavPane) { %>
<td width="100%">
<% } else { %>
<td>
<% } %>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>
</td>
</tr>


<tr>

<%
	string sNavPaneStyle = "display: " + ((CSWContext.ShowNavPane) ? "block" : "none");
%>

<td id="NavPane" valign=top align=left width="150" style="<%=sNavPaneStyle%>">

<% Server.Execute("/CentralWeb/Common/NavTree.aspx"); %>



</td>
<td width="100%" valign=top>


