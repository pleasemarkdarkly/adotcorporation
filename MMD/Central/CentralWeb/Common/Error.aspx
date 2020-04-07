<%@ Page language="c#" Codebehind="Error.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Common.Error" %>
<% CSWContext.PageTitle = "Error"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>


<div class="PageTitle">Error</div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

An error has occurred in the MMD Central Application.  
Please shutdown server and contact your company's legal affiliates to prepare 
appropriate lawsuit against the product developer.




<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
