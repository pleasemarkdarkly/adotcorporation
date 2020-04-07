<%@ Page language="c#" Codebehind="Files.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Advertisement.Files" %>
<% CSWContext.PageTitle = "Advertisement Files"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<div class="PageTitle"><%=CSWContext.PageTitle%></div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

<% DoListView(); %>

Register New Files 


specify ad file type, goto view of non-registered files, select, which takes you to new file with filename already filled in, specify MIMEtype

Upload New Files
specify ad file type, upload file



<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
