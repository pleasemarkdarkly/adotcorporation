<%@ Page language="c#" Codebehind="TopScannedProducts.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Reports.TopScannedProducts" %>
<% CSWContext.PageTitle = "Top Scanned Products"; %>

<% CSWContext.ShowNavPane = false; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<% DoListView(); %>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>