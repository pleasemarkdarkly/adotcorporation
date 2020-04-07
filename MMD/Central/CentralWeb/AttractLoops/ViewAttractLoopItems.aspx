<%@ Page language="c#" Codebehind="ViewAttractLoopItems.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.AttractLoops.ViewAttractLoopItems" %>
<% CSWContext.PageTitle = "Manage Attract Loops > View Attract Loop Files"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<p class="SmallText">Click [Edit] to change any of the existing attract loops.<br>
Or, click below to add an attract loop file.</p>

<a class="NavLink" href="/CentralWeb/AttractLoops/AddAttractLoopItem.aspx">Add an Attract Loop File</a> <br>
<a class="NavLink" href="/CentralWeb/AttractLoops/EditAttractLoopSequence.aspx?cmd=AddAttractLoopSequence">Add an Attract Loop Sequence</a><br>
<br>

<% DoListView(); %>

<br>
<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
<ul>
<li>ListView
<ul>
<li>Loop Item Name</li>
<li>MIME TYpe</li>
<li>Type</li>
<li>File - link to open file in new window</li>
<li>Assoc - number of loop items this file is associated with</li>
<li>Change Date</li>
<li>Changed By</li>
<li>Edit - link to edit attract loop</li>
<li>Remove - only can remove attract loop files when assocation = 0</li>
<li></li>
</ul>
</li>
</ul>
</div>
</div>


<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>

