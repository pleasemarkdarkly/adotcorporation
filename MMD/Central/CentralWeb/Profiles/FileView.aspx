<%@ Page language="c#" Codebehind="FileView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Profiles.FileView" %>
<% CSWContext.PageTitle = "Manage Recommendation Lists > View Files"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<p class="SmallText">These files are images for Special Offers and other promotion recommend lists.<br>  
You can add new files by clicking the link below.</p>

<a class="NavLink" href="/CentralWeb/Profiles/FileAdd.aspx">Add New Recommend List File</a><br>
<br>

<% DoListView(); %>


<br>
<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
<ul>
<li>ListView
<ul>
<li>Store Concept</li>
<li>Loop Item Name</li>
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