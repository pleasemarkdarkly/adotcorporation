<%@ Page language="c#" Codebehind="RecLists.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Profiles.RecLists" %>
<% CSWContext.PageTitle = "Manage Recommend Lists"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<p><a class="NavLink" href="/CentralWeb/Profiles/RecListView.aspx?idRecListBaseType=1">Product Recommend Lists</a><br>
<span class="SmallText">Manage recommend lists that contain products. [<b>Top 20</b>, <b>Staff Picks</b>, <b>New Releases</b>, <b>Reserve Yours</b>]<br>
Exists lists can be modified and new lists can be created here.</span></p>

<p><a class="NavLink" href="/CentralWeb/Profiles/RecListView.aspx?idRecListBaseType=2">Promotion Recommend Lists</a><br>
<span class="SmallText">Manage recommend lists that contain promotions. [<b>Free CDs</b>, <b>Special Offers</b>].<br>
Existing lists can be modified and new lsits can be created here.</span></p>

<p><a class="NavLink" href="/CentralWeb/Profiles/FileView.aspx">Manage Recommend List Files</a><br>
<span class="SmallText">Add or remove files associated with recommend lists.<br>
This is how you add images to be displayed in the recommend lists.</span></p>

<p><a class="NavLink" href="/CentralWeb/Profiles/RecListTypeView.aspx">Manage Recommend List Types</a><br>
<span class="SmallText">View, edit, and create recommend list types.</span></p>


<p>&nbsp;</p>
      
<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
Functional links:<br>
<ul>
<li>TODO:  Fill in this description.</li>
</ul>
</div>
</div>
      
<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>