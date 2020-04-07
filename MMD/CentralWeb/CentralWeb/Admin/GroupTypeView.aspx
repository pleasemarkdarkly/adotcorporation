<%@ Page language="c#" Codebehind="GroupTypeView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Admin.GroupTypeView" %>
<%@ Import NameSpace="System.Data" %>

<% CSWContext.PageTitle = "Admin - Manage Group Types"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<a class="BreadCrumb" href="/CentralWeb/Admin/Home.aspx">Admin Home</a><br>
<div class="PageTitle">Manage Group Types</div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

<div class="Text">
Talk a little bit about the functionality here.  you can add new group types at the bottom of the screen.
For the existing group types, you can:
<ul>
	<li><b>Edit</b>&nbsp;- Update the name of a group type</li>
	<li><b>Remove</b>&nbsp;- Delete the group type, its groups, and respective store groupings</li>
	<li><b>Manage</b>&nbsp;- Manage the underlying group members</li>
</ul>
</div>

<div class="SectionTitle">Existing Group Types</div>
<% if(GroupType.Rows.Count > 0) { %>
<table class="StdTable" cellspacing=0>
<tr class="StdRow">
	<td class="StdHdr">Name&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</td>
	<td class="StdHdr">Commands</td>
</tr>
<%	int i=0;
	foreach(DataRow row in GroupType.Rows) {
		i++;
%>
<tr class="StdRow">
<td class="StdCell"><%=row["sTypeName"]%></td>
<td class="StdCell">
	<a class="ShortCut" href="javascript:FormAlert('frmEdit_<%=i%>','Update Group Type', 'Enter the new name for the Group Type <b><%=row["sTypeName"]%></b> and click Update to submit changes.', 'divEdit_<%=i%>', 35, 10)">Edit</a>&nbsp;&nbsp;
	<a class="ShortCut" href="javascript:ConfirmLink('Are you sure you want to remove \'<%=row["sTypeName"]%>\'?  All grouping and store groups of this Group Type will be deleted.','/CentralWeb/Admin/GroupTypeView.aspx?cmd=GroupTypeDelete&idGroupType=<%=row["idGroupType"]%>');">Remove</a>&nbsp;&nbsp;
	<a class="ShortCut" href="/CentralWeb/Admin/GroupView.aspx?idGroupType=<%=row["idGroupType"]%>">Manage</a>
<div id="divEdit_<%=i%>" style="display: none">
	<form id="frmEdit_<%=i%>" method=post action="/CentralWeb/Admin/GroupTypeView.aspx">
		<input type="hidden" name="cmd" value="GroupTypeUpdate">
		<input type="hidden" name="idGroupType" value="<%=row["idGroupType"]%>">
		<input type="hidden" name="sOldTypeName" value="<%=row["sTypeName"]%>">
		<table class="StdTable" cellspacing=0>
		<tr class="StdRow">
			<td class="StdHdr">Group Type Name</td>
			<td class="StdHdr">Commands</td>
		</tr>
		<tr class="StdRow">
			<td class="StdCell">
				<input type="text" size=40 maxlength=255 name="sTypeName" value="<%=row["sTypeName"]%>">
				<input type="hidden" name="label_sTypeName" value="Group Type Name">
				<input type="hidden" name="required_sTypeName" value="1">
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
<div style="padding-left: 5mm;"><i>No Existing Group Types</i></div>
<% } %>
	
<form id="frmMain" name="frmMain" method=post action="/CentralWeb/Admin/GroupTypeView.aspx">
<div class="SectionTitle">Add New Group Type</div>

<table class="StdTable" cellspacing=0 cellpadding=0 >
<tr class="StdRow">
	<td class="StdHdr">Group Type Name</td>
	<td class="StdHdr">Commands</td>
</tr>
<tr class="StdRow">
	<td class="StdCell">
		<input type="hidden" name="cmd" value="GroupTypeAdd">
		<input type="text" name="sTypeName" size=40 maxlength=255 value="">
		<input type="hidden" name="required_sTypeName" value="1">
		<input type="hidden" name="label_sTypeName" value="Group Type Name">
	</td>
	<td class="StdCell"><a class="ShortCut" href="javascript:SubmitForm('frmMain','AddGroupType')">Add</a></td>
</tr>
</table>

</form>


<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
