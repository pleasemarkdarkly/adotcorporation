<%@ Page language="c#" Codebehind="ViewAttractLoops.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.AttractLoops.ViewAttractLoops" %>
<% CSWContext.PageTitle = "Manage Attract Loops > View All Attract Loops"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<p class="SmallText">
Click the Attract Loop name to edit the attract loop.<br>
Or, Click below to create a new Attract Loop.
</p>

<a class="NavLink" href="/CentralWeb/AttractLoops/EditAttractLoopSequence.aspx?cmd=AddAttractLoopSequence">Add an Attract Loop Sequence</a><br>

<div class="SectionTitle">Existing Attract Loops:</div>
<a class="NavLink" href="ViewAttractLoops.aspx?cmd=AttractLoopSubmitAll">Submit All Changes</a><br>
<a class="NavLink" href="ViewAttractLoops.aspx?cmd=AttractLoopUndoAll">Undo All Changes</a><br>

<br>

<% DoListView(); %>

<br>

<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
Functional links:<br>
<ul>
<li>Submit All Changes - submit (q.v.) all attract loop changes</li>
<li>Undo all changes - undo (q.v.) all</li>
<li>Listview - columns
	<ul>
	<li>Name - Attract Loop Name, click to edit</li>
	<li>Frames - number of attract loop items</li>
	<li>Assoc - number of devices associated with this loop</li>
	<li>Status - New, Unchanged, Changed, Deleted</li>
	<li>Date Changed</li>
	<li>Changed By - last person to modify loop or loop items of this loop</li>
	<li>Delete - link should appear when status is unchanged.  clicking marks list for deletion</li>
	<li>Undo - link should apper when status is not unchanged.  clicking undo's changes
		<ul>
			<li>New - deletes list from staging</li>
			<li>Changed - reverts changes from deploy database</li>
			<li>Deleted - reverts changes from deploy database</li>
		</ul>
	</li>
	<li>
		Submit - link should appear when status is not unchanged.  click submit does:
		<ul>
			<li>New - adds list to deployment database</li>
			<li>Changed - modifies deployment database version appropriately</li>
			<li>Deleted - Deletes list from staging and deploy database</li>
		</ul>
		once submitted, status should be unchanged
	</li>
	</ul>
</li>
<li>Add an attract loop items</li>
</ul>
</div>
</div>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>

