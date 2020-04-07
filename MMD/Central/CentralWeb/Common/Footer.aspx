<%@ Page language="c#" Codebehind="Footer.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Common.Footer" %>
<%@ Import NameSpace="System.Reflection" %>
<%@ Import NameSpace="MMD.Central.CentralLib" %>
<%@ Import NameSpace="MMD.Central.CentralWeb" %>


</td>
</tr>
</table>
<!-- Begin Screen Size Rulers -->

<!--
<a class="ShortCut" href="javascript:ToggleDisplay('NavPane')">Toggle Nav</a>

&nbsp;<a class="ShortCut" href="javascript:ToggleBVT()">BVT</a>

<div style="position:absolute; z-index:-100; top:0px; left:0px; width: 640px; height: 480px; border: dashed pink 1px; text-align: right; color: pink; font-size: 8pt"><div style="height: 465px"></div>640 x 480</div>
<div style="position:absolute; z-index:-100; top:0px; left:0px; width: 800px; height: 600px; border: dashed pink 1px; text-align: right; color: pink; font-size: 8pt"><div style="height: 585px"></div>800 x 600</div>
<div style="position:absolute; z-index:-100; top:0px; left:0px; width: 1024px; height: 768px; border: dashed pink 1px; text-align: right; color: pink; font-size: 8pt"><div style="height: 753px"></div>1024 x 768</div>
<div style="position:absolute; z-index:-100; top:0px; left:0px; width: 1280px; height: 1024px; border: dashed pink 1px; text-align: right; color: pink; font-size: 8pt"><div style="height: 1009px"></div>1280 x 1024</div>
<div style="position:absolute; z-index:-100; top:0px; left:0px; width: 1400px; height: 1000px; border: dashed pink 1px; text-align: right; color: pink; font-size: 8pt"><div style="height: 985px"></div>1400 x 1000</div>
<div style="position:absolute; z-index:-100; top:0px; left:0px; width: 1600px; height: 1200px; border: dashed pink 1px; text-align: right; color: pink; font-size: 8pt"><div style="height: 1185px"></div>1600 x 1200</div>
-->
<!-- End Screen Size Rulers -->
<%
	object o = new Hook();
%>

	<table width="100%" style="border-top: dashed black 1px" bgcolor="#ffffff" cellpadding=0 cellspacing=0>
	<tr>
		<td ><span class="FooterHdr">Database:</span>&nbsp;<span class="Footer"><%=CSWContext.Config.MainDatabase.SqlConnection.DataSource%>;<%=CSWContext.Config.MainDatabase.SqlConnection.Database%></span></td>
		<td class="FooterHdr" align="right"><%=Assembly.GetAssembly(o.GetType()).GetName().Name%>&nbsp;Version:</td>
		<td width=80 class="Footer"><%=Assembly.GetAssembly(o.GetType()).GetName().Version%></td></tr>
	<tr>
		<td class="Footer">&nbsp;</td>
		<td class="FooterHdr" align="right"><%=Assembly.GetAssembly(CSWContext.GetType()).GetName().Name%>&nbsp;Version:</td>
		<td width=80 class="Footer"><%=Assembly.GetAssembly(CSWContext.GetType()).GetName().Version%></td></tr>
	</table>

<% if(CSWContext.ShowNavPane) { %>
		</td>
		<!-- end of nested table for prod images here -->
		<td width="30"><img src="/CentralWeb/img/clear.gif" width="30" height="30"></td>
		<td width="20" background="/CentralWeb/img/blue_right.gif">
		<!--<img src="/CentralWeb/img/clear.gif" width="20" alt="">-->
		<div style="width: 20px"/>
		</td>
	</tr>
	<tr>
		<td width="21" height="15"><img src="/CentralWeb/img/blue_left.gif" width="21" height="15" alt="" border="0"></td>
		<td height="15" width=100% colspan="3"><img src="/CentralWeb/img/clear.gif"  height="15"></td>
		<td width="20" height="15"><img src="/CentralWeb/img/blue_right.gif" width="20" height="15" alt="" border="0"></td>
	</tr>
</table>

<table width="100%" height="22" cellpadding="0" cellspacing="0" border="0">
<tr>
<td width="21" height="22"><img src="/CentralWeb/img/blue_left.gif" width=21 height=22 alt="" border="0"></td>
    <td width="100%" height="22">
      <table width="100%" border="0" cellspacing="0" cellpadding="0">
        <tr>
          <td><img src="/CentralWeb/img/black_bar_03.gif" width="748" height="22"></td>
          <td width="100%"><img src="/CentralWeb/img/black_bar_spacer.gif" width="100%" height="22"></td>
        </tr>
      </table>
    </td>
<td width="20" height="22"><img src="/CentralWeb/img/blue_right.gif" width=20 height=22 alt="" border="0"></td>
</tr>
<tr>
<td width="21" height="21"><img src="/CentralWeb/img/blue_left.gif" width=21 height=21 alt="" border="0"></td>
    <td width="100%" height="21"><img src="/CentralWeb/img/bluebg.gif" width=100% height=21 alt="" border="0"></td>
<td width="20" height="21"><img src="/CentralWeb/img/blue_right.gif" width=20 height=21 alt="" border="0"></td>
</tr>
</table>

</td></tr></table>
<% } %>
  </body>
</html>
