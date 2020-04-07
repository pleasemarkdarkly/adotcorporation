<%@ Page language="c#" Codebehind="Tasks.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Tasks.Tasks" %>
<% CSWContext.PageTitle = "Delta Package Propagation"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<p><a class="NavLink" href="/CentralWeb/Tasks/EventScheduleView.aspx">View Event Scheduling</a><br>
<span class="SmallText">View, change, and run events manually.</span></p>

<p><a class="NavLink" href="/CentralWeb/Tasks/PackageListView.aspx">View Packages</a><br>
<span class="SmallText">Description.</span></p>
<!--
<p><a class="NavLink" href="javascript:NiceAlert('Not Implemented Yet')">View Activity Logs</a><br>
<span class="SmallText">Description.</span></p>

<p><a class="NavLink" href="javascript:NiceAlert('Not Implemented Yet')">Accumulate Changes and Create Baselines</a> <br>
<span class="SmallText">Description.</span></p>

<p><a class="NavLink" href="javascript:NiceAlert('Not Implemented Yet')">Configure Notification Settings</a> <br>
<span class="SmallText">Description.</span></p>
-->
<p>&nbsp;</p>
      
<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
Functional links:<br>
<ul>
<li>TODO:  Fill in this description.</li>
</ul>
</div>
</div>
      
<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>