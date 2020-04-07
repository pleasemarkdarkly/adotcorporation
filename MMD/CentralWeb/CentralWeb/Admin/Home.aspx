<%@ Page language="c#" Codebehind="Home.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Admin.Home" %>
<% CSWContext.PageTitle = "Admin Home"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<div class="PageTitle">Admin Home</div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

This section will contain functionality to bring stores online, as well as to configure store groups, and group stores.
In addition, monitoring stores can be performed from here, as well as basic activity at a store/store group level.

<div style="height: 15px"></div>

<div class="SectionTitle">Store Configuration</div>
<div style="padding-left: 15px; margin-bottom: 10px;">
<li>Create New Store</li>	
<li>Manage Stores</li>
</div>

<div class="SectionTitle">Store Group Configuration</div>
<div style="padding-left: 15px; margin-bottom: 10px;">
<li><a href="/CentralWeb/Admin/GroupTypeView.aspx">Manage Store Groups</a></li>
<li>Group Stores</li>
</div>

<div class="SectionTitle">Store Monitoring</div>
<div style="padding-left: 15px; margin-bottom: 10px;">
<li><a href="/CentralWeb/Admin/StoreConfig.aspx">Monitor Store</a></li>
<li>Monitor Store Groups</li>
</div>

Todo: make device groups (build off store groups), device groupings, monitor device groups, configure device, device groups<br>
Todo: make store groupings, monitor store groups, configure store, store groups<br>

device configuration: idle loop, location mode (home page), special offers??<br>
store configuration: most things in config file, such as POS, CRM, tasks, store manager mail, store location<br>
manage stores: add/remove stores<br>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
