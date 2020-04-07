<%@ Page language="c#" Codebehind="default.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb._default" %>
<% CSWContext.PageTitle = "Fullplay Media Merchandising Console"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>


<p class="SmallText">
This application allows you to manage all aspects of the Fullplay Media Merchandising 
System.  You can manage lists and attract loops, as well as view shopper activity and generated
reports from clickstream data.
</p>

<a class="NavLink" href="/CentralWeb/Profiles/RecLists.aspx">Recommend Lists</a><br>
<span class="SmallText">Create and modify recommend lists such as Top 20, New Releases, and Promotions</span><br>
&nbsp;<br>

<a class="NavLink" href="/CentralWeb/AttractLoops/AttractLoops.aspx">Attract Loops</a><br>
<span class="SmallText">Create and modify advertising loops that play on idle devices.</span><br>
&nbsp;<br>
<a class="NavLink" href="/CentralWeb/Reports/Home.aspx">Shopper Activity Reports</a><br>
<span class="SmallText">View reports from clickstream data uploaded from devices in stores.</span><br>
&nbsp;<br>
<a class="NavLink" href="/CentralWeb/SystemMgmt/Home.aspx">Configure and Monitor System</a><br>
<span class="SmallText">Configure devices and stores, and monitor system status.</span><br>
&nbsp;<br>
<a class="NavLink" href="/CentralWeb/Content/Home.aspx">Configure Delta Propagation</a><br>
<span class="SmallText">Manage and schedule information between this server and individual store servers.</span><br>
&nbsp;<br>
<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
