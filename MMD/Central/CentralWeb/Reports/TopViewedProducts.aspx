<%@ Page language="c#" Codebehind="TopViewedProducts.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Reports.TopViewedProducts" %>
<% CSWContext.PageTitle = "Top Viewed Products"; %>
<% CSWContext.ShowNavPane = false; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<% DoListView(); %>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>