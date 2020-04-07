<%@ Page language="c#" Codebehind="AllDevices.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.SystemMgmt.AllDevices" %>
<% CSWContext.PageTitle = "System Management &gt; All Devices"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<% DoListView(); %>

<br>
<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
<!--	TODO : Place Test information here -->
</div>
</div>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>

