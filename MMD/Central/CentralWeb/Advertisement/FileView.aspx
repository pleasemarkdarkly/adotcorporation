<%@ Page language="c#" Codebehind="FileView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Advertisement.FileView" %>
<% CSWContext.PageTitle = ""; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<a class="BreadCrumb" href="/CentralWeb">BreadCrumb</a><br>
<div class="PageTitle"><%=CSWContext.PageTitle%></div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>


<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>