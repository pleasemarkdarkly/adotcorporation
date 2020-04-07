<%@ Page language="c#" Codebehind="RecListUsageAllStores.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Reports.RecListUsageAllStores" %>
<% CSWContext.PageTitle = "Rec List Usage - All Stores"; %>
<% CSWContext.ShowNavPane = false; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>


<% DoListView(); %>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>