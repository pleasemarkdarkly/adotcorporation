<%@ Page language="c#" Codebehind="RecListView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Profiles.RecListView" %>
<% CSWContext.PageTitle = "Manage Recommend Lists > All " + ((idRecListBaseType == 1) ? "Product" : "Promotion") + " Recommend Lists"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<% DoListView(); %>
 
<br>

<a class="NavLink" href="RecListView.aspx?cmd=RecListSubmitAll&idRecListBaseType=<%=idRecListBaseType%>">Submit All Changes</a><br>
<a class="NavLink" href="RecListView.aspx?cmd=RecListUndoAll&idRecListBaseType=<%=idRecListBaseType%>">Undo All Changes</a><br>

<hr>
<span class="SmallText"><b>Create New <%=((idRecListBaseType == 1) ? "Product" : "Promotion")%> Recommend List</b></span>
<form name="frmRecListAdd" method="post" action="/CentralWeb/Profiles/RecListEdit.aspx">
	<table class="StdTable" cellspacing=0>
		<tr class="StdRow">
			<td class="SmallText"><b>Concept</b></td>
			<td class="SmallText"><b>List Type</b></td>
			<% if(idRecListBaseType == 1) { %>
			<td class="SmallText"><b>Product Type</b></td>
			<% } %>
		</tr>
		<tr class="StdRow">
			<td class="SmallText">
				<b><%=CSWContext.Lookups.StoreConcept.GetHTMLComboBox("idStoreConcept","")%></b>
				<input type="hidden" name="label_idStoreConcept" value="Concept">
				<input type="hidden" name="required_idStoreConcept" value="1">
			</td>
			<td class="SmallText">
				<b><%=CSWContext.Lookups.RecListType(idRecListBaseType).GetHTMLComboBox("idRecListType","")%></b>
				<input type="hidden" name="label_idRecListType" value="List Type">
				<input type="hidden" name="required_idRecListType" value="1">
			</td>
			<% if(idRecListBaseType == 1) { %>
			<td class="SmallText">
				<b><%=CSWContext.Lookups.ProductType.GetHTMLComboBox("idProductType","")%></b>
				<input type="hidden" name="label_idProductType" value="Product Type">
				<input type="hidden" name="required_idProductType" value="1">
			</td>
			<% } %>
			<td class="SmallText">
				<b>
				<a class="NavLink" title="Now click the left mouse button" href="javascript:SubmitForm('frmRecListAdd','')">Create New <%=((idRecListBaseType == 1) ? "Product" : "Promotion")%> Recommend List</a>
				<!-- <input type="submit" value="Create" onclick="SubmitForm('frmRecListAdd',''); return false;" > -->
				<input type="hidden" name="cmd" value="RecListAdd">
				</b>
			</td>
		</tr>
	</table>
</form>

<br>
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

