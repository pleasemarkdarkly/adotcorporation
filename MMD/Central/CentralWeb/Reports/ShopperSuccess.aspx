<%@ Page language="c#" Codebehind="ShopperSuccess.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Reports.ShopperSuccess" %>
<% CSWContext.PageTitle = "Shopper Success / Interest"; %>
<% CSWContext.ShowNavPane = false; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<div class="SmallText">
*Good sessions are those with more than one scan event or recommendation list viewing with samples played.
</div>
<p>&nbsp;</p>

<% DoListView(); %>


<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
