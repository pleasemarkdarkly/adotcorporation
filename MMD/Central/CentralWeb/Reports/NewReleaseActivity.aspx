<%@ Page language="c#" Codebehind="NewReleaseActivity.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Reports.NewReleaseActivity" %>
<% CSWContext.PageTitle = "New Release Activity"; %>
<% CSWContext.ShowNavPane = false; %>

<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<% DoListView(); %>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>