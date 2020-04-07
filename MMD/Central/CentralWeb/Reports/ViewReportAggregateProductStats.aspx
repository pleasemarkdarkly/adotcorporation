<%@ Page language="c#" Codebehind="ViewReportAggregateProductStats.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Reports.ViewReportAggregateProductStats" %>
<% CSWContext.PageTitle = "Aggregate Product Stats"; %>
<% CSWContext.ShowNavPane = false; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<% DoListView(); %>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
