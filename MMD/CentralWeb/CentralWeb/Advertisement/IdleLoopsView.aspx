<%@ Page language="c#" Codebehind="IdleLoopsView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Advertisement.IdleLoopsView" %>
<% CSWContext.PageTitle = "Manage IdleLoops"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<a class="BreadCrumb" href="/CentralWeb/Advertisement/Home.aspx">Advertisement Home</a><br>
<div class="PageTitle">Manage IdleLoops</div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

<div class="Text">
Click on the name of an idle loop to view its detailed information.<br>
All changes will not be committed to the store deployment system until they've been submitted.<br>
</div>
<br>
<p>

<% DoListView(); %>

	
<div id="divAdd" style="display: none">
	<form id="frmAdd" method=post action="/CentralWeb/Advertisement/IdleLoopsView.aspx">
		<input type="hidden" name="cmd" value="IdleLoopAdd">
		<table class="StdTable" cellspacing=0>
		<tr class="StdRow">
			<td class="StdHdr">Name</td>
			<td class="StdHdr">Description</td>
			<td class="StdHdr">Commands</td>
		</tr>
		<tr class="StdRow">
			<td class="StdCell" valign=top>
				<input type="text" size=40 maxlength=255 name="sName" value="">
				<input type="hidden" name="label_sName" value="Idle Loop Name">
				<input type="hidden" name="required_sName" value="1">
			</td>
			<td class="StdCell">
				<textarea cols="35" rows="4" name="sInfo"></textarea>
			</td>
			<td class="StdCell" valign=top>
				<a class="ShortCut" href="#" onclick="SubmitAlertForm('frmAdd');">Create</a>
				&nbsp;&nbsp;&nbsp;&nbsp;
				<a class="ShortCut" href="#" onclick="DoClose(0)">Cancel</a>
			</td>
		</tr>
		</table>
	</form>
</div>

<div class="SectionTitle">Actions</div>
<a class="ShortCut" href="javascript:FormAlert('frmAdd','Add New Idle Loop', 'Enter the name for the new Idle Loop and click <b>Create</b> to submit.', 'divAdd', 50, 12)">Create New Idle Loop</a><br>
<a class="ShortCut" href="javascript:ConfirmLink('Are you sure you want to submit all changes to the deployment database?  Once submitted, these changes cannot be undone.','/CentralWeb/Advertisement/IdleLoopsView.aspx?cmd=IdleLoopSubmitAll')">Submit changes to all idle loop.</a>


<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>