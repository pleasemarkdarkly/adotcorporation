<%@ Page language="c#" Codebehind="AttractLoops.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.AttractLoops.AttractLoops" %>
<% CSWContext.PageTitle = "Manage Attract Loops"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<p><a class="NavLink" href="/CentralWeb/AttractLoops/ViewAttractLoops.aspx">Manage Attract Loops</a><br>
<span class="SmallText">View and edit all attract loops.</span></p>

<p><a class="NavLink" href="/CentralWeb/AttractLoops/ViewAttractLoopItems.aspx">Manage Attract Loop Files</a><br>
<span class="SmallText">View and edit individual Attract Loop Files.</span></p>



<p>&nbsp;</p>


<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
Functional links:<br>
<ul>
<li>View All Attract Loops</li>
<li>Add an Attract Loop Sequence</li>
<li>View attract loop items, basically files that have been uploaded to be part of attract loops</li>
<li>Add an attract loop items</li>
</ul>
</div>
</div>
      
<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>