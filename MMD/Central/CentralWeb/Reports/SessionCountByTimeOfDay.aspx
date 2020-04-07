<%@ Page language="c#" Codebehind="SessionCountByTimeOfDay.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Reports.SessionCountByTimeOfDay" %>
<% CSWContext.PageTitle = "Session Count By Time Of Day"; %>
<% CSWContext.ShowNavPane = false; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>


<% DoListView(); %>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
