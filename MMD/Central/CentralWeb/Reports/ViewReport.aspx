<%@ Page language="c#" Codebehind="ViewReport.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Reports.ViewReport" %>
<% CSWContext.PageTitle = ""; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<a class="BreadCrumb" href="/CentralWeb">BreadCrumb</a><br>
<div class="PageTitle"><%=CSWContext.PageTitle%></div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>


<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>