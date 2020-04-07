<%@ Page language="c#" Codebehind="EventCommandHistoryView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Tasks.EventCommandHistoryView" %>
<% CSWContext.PageTitle = "Delta Package Propagation > View Event History"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<table class="StdTable" cellspacing="5">
	<tr class="StdRow">
		<td class="SmallText"><b>Event Name</b></td>
		<td class="SmallText" colspan="7"><%=sDesc%></td>
	</tr>
	<tr class="StdRow">
		<td class="SmallText"><b>Event ID</b></td>
		<td class="SmallText" colspan="7"><%=idEventSchedule%></td>
	</tr>
</table>
<br>


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