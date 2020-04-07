<%@ Page language="c#" Codebehind="GroupView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Admin.GroupView" %>
<%@ Import NameSpace="System.Data" %>

<% CSWContext.PageTitle = "Admin - Manage Group - " + GroupTypeName; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<a class="BreadCrumb" href="/CentralWeb/Admin/GroupTypeView.aspx">Manage Group Types</a><br>
<div class="PageTitle">Manage Group - <%=GroupTypeName%></div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

<div class="SectionTitle">Existing Members of <%=GroupTypeName%></div>
<% if(Group.Rows.Count > 0) { %>
<table class="StdTable" cellspacing=0>
<tr class="StdRow">
	<td class="StdHdr">Group Name</td>
	<td class="StdHdr">Commands</td>
</tr>
<%	int i=0;
	foreach(DataRow row in Group.Rows) {
		i++;
%>
<tr class="StdRow">
<td class="StdCell"><%=row["sName"]%></td>
<td class="StdCell">
	<a class="ShortCut" href="javascript:FormAlert('frmEdit_<%=i%>','Update Group', 'Enter the new name for the Group <b><%=row["sName"]%></b> and click Update to submit changes.', 'divEdit_<%=i%>', 35, 10)">Edit</a>&nbsp;&nbsp;
	<a class="ShortCut" href="javascript:ConfirmLink('Are you sure you want to remove Group \'<%=row["sName"]%>\'?  All store groups of for this group member will be deleted.','/CentralWeb/Admin/GroupView.aspx?cmd=GroupDelete&idGroup=<%=row["idGroup"]%>&sName=<%=row["sName"]%>&sTypeName=<%=GroupTypeName%>');">Remove</a>
<div id="divEdit_<%=i%>" style="display: none">
	<form id="frmEdit_<%=i%>" method=post action="/CentralWeb/Admin/GroupView.aspx">
		<input type="hidden" name="cmd" value="GroupUpdate">
		<input type="hidden" name="idGroup" value="<%=row["idGroup"]%>">
		<input type="hidden" name="sOldName" value="<%=row["sName"]%>">
		<input type="hidden" name="sTypeName" value="<%=GroupTypeName%>">
		<table class="StdTable" cellspacing=0>
		<tr class="StdRow">
			<td class="StdHdr">Group Name</td>
			<td class="StdHdr">Information</td>
			<td class="StdHdr">Commands</td>
		</tr>
		<tr class="StdRow">
			<td class="StdCell">
				<input type="text" size=40 maxlength=255 name="sName" value="<%=row["sName"]%>">
				<input type="hidden" name="label_sName" value="Group Name">
				<input type="hidden" name="required_sName" value="1">
			</td>
			<td class="StdCell">
				<input type="text" size=40 maxlength=1000 name="sInfo" value="<%=row["sInfo"]%>">
			</td>
			<td class="StdCell">
				<a class="ShortCut" href="#" onclick="SubmitAlertForm('frmEdit_<%=i%>');">Update</a>
				&nbsp;&nbsp;&nbsp;&nbsp;
				<a class="ShortCut" href="#" onclick="DoClose(0)">Cancel</a>
			</td>
		</tr>
		</table>
	</form>
</div>
</td>
</tr>
<% } %>
</table>
<% } else { %>
<div style="padding-left: 5mm;"><i>No Existing Members</i></div>
<% } %>
	

<form id="frmMain" name="frmMain" method=post action="/CentralWeb/Admin/GroupView.aspx">
<div class="SectionTitle">Add New Member To <%=GroupTypeName%></div>

<table class="StdTable" cellspacing=0 cellpadding=0 >
<tr class="StdRow">
	<td class="StdHdr">Group Name</td>
	<td class="StdHdr">Information</td>
	<td class="StdHdr">Commands</td>
</tr>
<tr class="StdRow">
	<td class="StdCell">
		<input type="hidden" name="cmd" value="GroupAdd">
		<input type="hidden" name="idGroupType" value="<%=GroupType%>">
		<input type="hidden" name="sTypeName" value="<%=GroupTypeName%>">
		<input type="text" name="sName" size=40 maxlength=255 value="">
		<input type="hidden" name="required_sName" value="1">
		<input type="hidden" name="label_sName" value="Group Name">
	</td>
	<td class="StdCell"><input type="text" name="sInfo" size=40 maxlength=1000 value=""></td>
	<td class="StdCell"><a class="ShortCut" href="javascript:SubmitForm('frmMain','AddGroup')">Add</a></td>
</tr>
</table>

</form>


<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>

