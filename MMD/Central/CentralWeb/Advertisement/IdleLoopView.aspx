<%@ Page language="c#" Codebehind="IdleLoopView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Advertisement.IdleLoopView" %>
<% CSWContext.PageTitle = "Manage IdleLoop - " + IdleLoop.sName; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<a class="BreadCrumb" href="/CentralWeb/Advertisement/IdleLoopsView.aspx">Manage Idleloops</a><br>
<div class="PageTitle">Manage IdleLoop - <%=IdleLoop.sName%></div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

<table class="StdTable" cellpadding=0 cellspacing=0>
<tr class="StdRow"><td class="StdHdr" align=right>Name:</td><td class="StdCell"><%=IdleLoop.sName%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Description:</td><td class="StdCell"><%=IdleLoop.sInfo%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Frames:</td><td class="StdCell"><%=IdleLoop.Elements.Count%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Status:</td><td class="StdCell"><%=IdleLoop.byStatus%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Created:</td><td class="StdCell"><%=IdleLoop.dtCreated%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Created By:</td><td class="StdCell"><%=IdleLoop.sCreatedBy%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Changed:</td><td class="StdCell"><%=IdleLoop.dtChanged%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Changed By:</td><td class="StdCell"><%=IdleLoop.sChangedBy%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdCell" colspan=2 align=center>	
	<a class="ShortCut" href="javascript:FormAlert('frmEdit','Update Idle Loop', 'Enter the new name for the Idle Loop <b><%=IdleLoop.sName%></b> and click Update to submit changes.', 'divEdit', 50, 12)">Edit</a>
</td></tr>
</table>

	
<div id="divEdit" style="display: none">
	<form id="frmEdit" method=post action="/CentralWeb/Advertisement/IdleLoopView.aspx">
		<input type="hidden" name="cmd" value="IdleLoopUpdate">
		<input type="hidden" name="idIdleLoop" value="<%=IdleLoop.idIdleLoop%>">
		<input type="hidden" name="sOldName" value="<%=IdleLoop.sName%>">
		<table class="StdTable" cellspacing=0>
		<tr class="StdRow">
			<td class="StdHdr">Name</td>
			<td class="StdHdr">Description</td>
			<td class="StdHdr">Commands</td>
		</tr>
		<tr class="StdRow">
			<td class="StdCell" valign=top>
				<input type="text" size=40 maxlength=255 name="sName" value="<%=IdleLoop.sName%>">
				<input type="hidden" name="label_sName" value="Idle Loop Name">
				<input type="hidden" name="required_sName" value="1">
			</td>
			<td class="StdCell">
				<textarea cols="35" rows="4" name="sInfo"><%=IdleLoop.sInfo%></textarea>
			</td>
			<td class="StdCell" valign=top>
				<a class="ShortCut" href="#" onclick="SubmitAlertForm('frmEdit');">Update</a>
				&nbsp;&nbsp;&nbsp;&nbsp;
				<a class="ShortCut" href="#" onclick="DoClose(0)">Cancel</a>
			</td>
		</tr>
		</table>
	</form>
</div>

<div class="SectionTitle">Actions</div>
<a class="ShortCut" href="/CentralWeb/Advertisement/IdleLoopElementsView.aspx?idIdleLoop=<%=IdleLoop.idIdleLoop%>">Manage Idle Loop Elements</a><br>


<%
	CSWContext.Params["sched_in_reltable"] = "tblMMD_IdleLoopScheduleStaging";
	CSWContext.Params["sched_in_reltablekey"] = "idIdleLoop";
	CSWContext.Params["sched_in_keyvalue"] = IdleLoop.idIdleLoop.ToString();
	CSWContext.Params["sched_in_page"] = "/CentralWeb/Advertisement/IdleLoopView.aspx?idIdleLoop=" + IdleLoop.idIdleLoop;
	//CSWContext.Params["sched_in_showeditlinks"] = "0";
	Server.Execute("/CentralWeb/Common/ManageSchedule.aspx");	
%>




<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>