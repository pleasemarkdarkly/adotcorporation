<%@ Page language="c#" Codebehind="File.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Advertisement.File" %>
<% CSWContext.PageTitle = ""; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<a class="BreadCrumb" href="/CentralWeb">BreadCrumb</a><br>
<div class="PageTitle"><%=CSWContext.PageTitle%></div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>


<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
