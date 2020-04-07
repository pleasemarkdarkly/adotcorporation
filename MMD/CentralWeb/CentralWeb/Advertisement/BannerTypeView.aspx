<%@ Page language="c#" Codebehind="BannerTypeView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Advertisement.BannerTypeView" %>
<% CSWContext.PageTitle = "Manage Banners"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>


<a class="BreadCrumb" href="/CentralWeb/Advertisement/Home.aspx">Advertisement Home</a><br>
<div class="PageTitle">Manage Banners</div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

<% DoListView(); %>


<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
