<%@ Page language="c#" Codebehind="ProductActivity.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Reports.ProductActivity" %>
<% CSWContext.PageTitle = "Product Activity"; %>
<% CSWContext.ShowNavPane = false; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<% DoListView(); %>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>