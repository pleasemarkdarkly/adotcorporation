<%@ Page language="c#" Codebehind="PackageListView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Tasks.PackageListView" %>
<% CSWContext.PageTitle = "Delta Package Propagation > View Packages"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<% DoListView(); %>

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