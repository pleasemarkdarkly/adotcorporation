<%@ Page language="c#" Codebehind="IdleLoopElementView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Advertisement.IdleLoopElementView" %>
<% CSWContext.PageTitle = "View Idle Loop Frame " + Loop.sName + " - " + Element.sName; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<a class="BreadCrumb" href="/CentralWeb/Advertisement/IdleLoopElementsView.aspx?idIdleLoop=<%=Loop.idIdleLoop%>">Manage Idle Loop Frames - <%=Loop.sName%></a><br>
<div class="PageTitle"><%=CSWContext.PageTitle%></div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

<table class="StdTable" cellspacing=0 cellpadding=0>
<tr class="StdRow"><td class="StdHdr" align=right>Loop Name:</td><td class="StdCell"><%=Loop.sName%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Element Name:</td><td class="StdCell"><%=Element.sName%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Sequence:</td><td class="StdCell"><%=Element.iSeq%>&nbsp;of&nbsp;<%=Loop.Elements.Count%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>MIME Type:</td><td class="StdCell"><%=Element.sMIMEType%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Display Time:</td><td class="StdCell"><%=Element.cSecsPlayingTime%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Changed:</td><td class="StdCell"><%=Element.dtChanged%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Changed By:</td><td class="StdCell"><%=Element.sChangedBy%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>URL:</td><td class="StdCell"><a target="_blank" href="<%=CSWContext.Config.IdleLoopDirectory(true) + "/" + Element.sURL%>"><%=Element.sURL%></a>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Info:</td><td class="StdCell"><%=Element.sAssetInfo%>&nbsp;</td></tr>
</table>

<br>

<div class="SectionTitle">Actions</div>
<a href="/CentralWeb/Advertisement/IdleLoopElementEdit.aspx?idIdleLoopElement=<%=Element.idIdleLoopElement%>">Edit</a><br>
Delete<br>
Reorder<br>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
