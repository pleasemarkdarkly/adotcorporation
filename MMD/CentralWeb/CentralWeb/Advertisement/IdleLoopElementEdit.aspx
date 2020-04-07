<%@ Page language="c#" Codebehind="IdleLoopElementEdit.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Advertisement.IdleLoopElementEdit" %>
<% CSWContext.PageTitle = "Edit Idle Loop Frame " + Loop.sName + " - " + ((Element != null) ? Element.sName : "New"); %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<a class="BreadCrumb" href="/CentralWeb/Advertisement/IdleLoopElementsView.aspx?idIdleLoop=<%=Loop.idIdleLoop%>">Manage Idle Loop Frames - <%=Loop.sName%></a><br>
<div class="PageTitle"><%=CSWContext.PageTitle%></div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

<form name="frmMain" method="post" enctype="multipart/form-data" action="/CentralWeb/Advertisement/IdleLoopElementView.aspx">
<input type="hidden" name="cmd" value="IdleLoopElementUpdate">
<input type="hidden" name="idIdleLoopElement" value="<%=(Element != null) ? Element.idIdleLoopElement : 0%>">
<input type="hidden" name="idIdleLoop" value="<%=Loop.idIdleLoop%>">

<table class="StdTable" cellspacing=0 cellpadding=0>
<tr class="StdRow"><td class="StdHdr" align=right>Loop Name:</td><td class="StdCell"><%=Loop.sName%></td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Element Name:</td>
	<td class="StdCell">
		<input type="text" name="sName" size=20 maxlength=100 value="<%=(Element != null) ? Element.sName : ""%>">
		<input type="hidden" name="label_sName" value="Name">
		<input type="hidden" name="required_sName" value="1">
	</td></tr>	
<tr class="StdRow"><td class="StdHdr" align=right>Sequence:</td>
	<td class="StdCell">
		<% MakeSequenceComboBox(); %>
	</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Display Time:</td>
	<td class="StdCell">
		<input type="text" name="cSecsPlayingTime" value="<%=(Element != null) ? Element.cSecsPlayingTime.ToString() : ""%>">
		<input type="hidden" name="label_cSecsPlayingTime" value="Display Time">
		<input type="hidden" name="required_cSecsPlayingTime" value="1">
		<input type="hidden" name="valid_cSecsPlayingTime" value="nonnegLong">	
	</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>File:</td><td class="StdCell">
	<input type="file" name="File">
	<input type="hidden" name="label_File" value="File">
	<input type="hidden" name="required_File" value="1">
</td></tr>
</table>

</form>

<div class="SectionTitle">Actions</div>
<a href="javascript:SubmitForm('frmMain','IdleLoopUpdate')"><%=(Element != null) ? "Update" : "Add"%></a><br>


<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
