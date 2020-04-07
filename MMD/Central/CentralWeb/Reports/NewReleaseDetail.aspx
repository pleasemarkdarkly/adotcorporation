<%@ Page language="c#" Codebehind="NewReleaseDetail.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Reports.NewReleaseDetail" %>
<% CSWContext.PageTitle = "New Release Detail"; %>
<% CSWContext.ShowNavPane = false; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<% DoListView(); %>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>