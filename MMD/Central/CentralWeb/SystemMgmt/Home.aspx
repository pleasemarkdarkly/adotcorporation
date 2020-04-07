<%@ Page language="c#" Codebehind="Home.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.SystemMgmt.Home" %>
<% CSWContext.PageTitle = "System Management"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<p class="SmallText">
<a class="NavLink" href="Stores.aspx">Manage Stores</a>
</p>

<p class="SmallText">
<a class="NavLink" href="AllDevices.aspx">Manage All Devices</a>
</p>

<p class="SmallText">
<a class="NavLink" href="Home.aspx?cmd=SyncReports">Update Reports from Logs</a>
</p>


<br>
<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
<!--	TODO : Place Test information here -->
</div>
</div>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>