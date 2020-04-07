<%@ Page language="c#" Codebehind="ViewFeatureRequests.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.FeatureRequest.ViewFeatureRequests" %>
<% CSWContext.PageTitle = "Feature Requests"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<% DoListView(); %>

<br>
<a class="NavLink" href="/CentralWeb/FeatureRequest/EditFeatureRequest.aspx">Add New Feature Request</a>
<br>
<br>
<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
<!--	TODO : Place Test information here -->
</div>
</div>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>