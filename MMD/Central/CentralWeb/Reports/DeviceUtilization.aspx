<%@ Page language="c#" Codebehind="DeviceUtilization.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Reports.DeviceUtilization" %>
<% CSWContext.PageTitle = "Device Utilization"; %>
<% CSWContext.ShowNavPane = false; %>

<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<% DoListView(); %>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
