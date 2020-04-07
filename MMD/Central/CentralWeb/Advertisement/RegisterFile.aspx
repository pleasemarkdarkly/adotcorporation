<%@ Page language="c#" Codebehind="RegisterFile.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Advertisement.RegisterFile" %>
<% CSWContext.PageTitle = "Register Files - " + Type.ToString(); %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<a class="BreadCrumb" href="/CentralWeb/Advertisement/Files.aspx">Manage Ad Files</a><br>
<div class="PageTitle"><%=CSWContext.PageTitle%></div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>


<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>