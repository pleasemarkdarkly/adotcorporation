<%@ Page language="c#" Codebehind="PackageView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Tasks.PackageView" %>
<% CSWContext.PageTitle = "Delta Package Propagation > View Package"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<form name="frmPackageEdit" method="post" action="/CentralWeb/Tasks/PackageView.aspx">
	<table class="StdTable" cellspacing="5">
		<tr class="StdRow">
			<td class="SmallText"><b>Package Name</b></td>
			<td class="SmallText" colspan="7">
				<input size="20" maxlength="100" name="sDesc" value="<%=sDesc%>">
				<input type="hidden" name="label_sDesc" value="Package Name">
				<input type="hidden" name="required_sDesc" value="1">
			</td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText"><b>Source Level</b></td>
			<td class="SmallText" colspan="7"><%=CSWContext.Lookups.PackageLevel.GetHTMLComboBox("idSourceLevel",sSourceLevel,false,true)%></td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText"><b>Destination Level</b></td>
			<td class="SmallText" colspan="7"><%=CSWContext.Lookups.PackageLevel.GetHTMLComboBox("idDestLevel",sDestLevel,false,true)%></td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText"><b>Send Event</b></td>
			<td class="SmallText" colspan="7"><%=SendEventComboBox%></td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText"><b>Receive Event</b></td>
			<td class="SmallText" colspan="7"><%=RecvEventComboBox%></td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText"><b>Transport Subdirectory</b></td>
			<td class="SmallText" colspan="7">
				<input size="40" maxlength="100" name="sTrasnportSubdir" value="<%=sTrasnportSubdir%>">
				<input type="hidden" name="label_sTrasnportSubdir" value="Transport Subdirectory">
				<input type="hidden" name="required_sTrasnportSubdir" value="1">
			</td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText" colspan="10"><hr></td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText" colspan="2">
				<b><a class="NavLink" href="javascript:SubmitForm('frmPackageEdit','')">Submit Changes</a></b>
				<input type="hidden" name="cmd" value="PackageEdit">
				<input type="hidden" name="idPackage" value="<%=idPackage%>">
			</td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText" colspan="2">
				<b><a class="NavLink" href="/CentralWeb/Tasks/PackageView.aspx?idPackage=<%=idPackage%>">Undo Changes</a></b>
			</td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText" colspan="2">
				<b><a class="NavLink" href="/CentralWeb/Tasks/PackageHistoryView.aspx?idPackage=<%=idPackage%>">View Package History</a></b>
			</td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText" colspan="2">
				<b><a class="NavLink" href="/CentralWeb/Tasks/PackageListView.aspx">Back to View All Packages</a></b>
			</td>
		</tr>
	</table>
</form>
<br>

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