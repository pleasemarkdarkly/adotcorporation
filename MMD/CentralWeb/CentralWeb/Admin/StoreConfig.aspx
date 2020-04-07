<%@ Page language="c#" Codebehind="StoreConfig.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Admin.StoreConfig" %>
<% CSWContext.PageTitle = ""; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<a class="BreadCrumb" href="/CentralWeb">BreadCrumb</a><br>
<div class="PageTitle"></div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

<% DoListView(); %>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
