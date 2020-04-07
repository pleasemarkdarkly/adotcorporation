<%@ Page language="c#" Codebehind="Promotions.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Promotions.Promotions" %>
<% CSWContext.PageTitle = ""; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<p><a class="NavLink" href="/CentralWeb/AttractLoops/ViewPromotions.aspx">All Promotions</a><br>
<span class="SmallText">View and edit all promotions.</span></p>

<p><a class="NavLink" href="/CentralWeb/AttractLoops/EditPromotionFile.aspx">Add a Promotion Item</a><br>
<span class="SmallText">Create a promotion item and upload associated files.</span></p>

<p><a class="NavLink" href="/CentralWeb/AttractLoops/CreatePromotionList.aspx">Create a Promotion Recommend List</a><br>
<span class="SmallText">View and edit individual Attract Loop Items.</span></p>

<p>&nbsp;</p>

<br>
<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
<!--	TODO : Place Test information here -->

</div>
</div>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>