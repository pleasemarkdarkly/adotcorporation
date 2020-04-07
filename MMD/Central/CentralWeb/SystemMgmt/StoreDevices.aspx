<%@ Page language="c#" Codebehind="StoreDevices.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.SystemMgmt.StoreDevices" %>
<%@ Import Namespace="MMD.Central.CentralLib.XmlObjects"%>
<% CSWContext.PageTitle = "System Management &gt; Store&nbsp;" + store.sName; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<form name="frmUpdateStore" method="post" action="StoreDevices.aspx">
<input type="hidden" name="cmd" value="StoreUpdate">
<input type="hidden" name="idStore" value="<%=store.idStore%>">

<table cellpadding=0 cellspacing=0>
<tr>
<td class="SmallText"><b>Store Name</b></td>
<td>
<input type="text" maxlength="255" name="sName" value="<%=store.sName%>">
<input type="hidden" name="label_sName" value="Store name">
<input type="hidden" name="required_sName" value="1">
</td>
</tr>
<tr>
<td class="SmallText"><b>Store Concept</b></td>
<td><%=CSWContext.Lookups.StoreConcept.GetValue(store.idStoreConcept)%></td>
</tr>
<tr>
<tr>
<td class="SmallText"><b>Created</b></td>
<td class="SmallText"><%=NullableDateTime.GetValue(store.dtCreated)%></td>
</tr>
<td class="SmallText"><b>Address</b></td>
<td><input type="text" maxlength="100" name="sAddress" value="<%=store.sAddress%>">
<input type="hidden" name="label_sAddress" value="Address">
<input type="hidden" name="required_sAddress" value="1">
</td>
</tr>
<tr>
<td class="SmallText"><b>City</b></td>
<td><input type="text" maxlength="50" name="sCity" value="<%=store.sCity%>">
<input type="hidden" name="label_sCity" value="City">
<input type="hidden" name="required_sCity" value="1">
</td>
</tr>
<tr>
<td class="SmallText"><b>State</b></td>
<td><input type="text" maxlength="2" name="sState" value="<%=store.sState%>">
<input type="hidden" name="label_sState" value="State">
<input type="hidden" name="required_sState" value="1">
</td>
</tr>
<tr>
<td class="SmallText"><b>Zip Code</b></td>
<td><input type="text" maxlength="10" name="sZip" value="<%=store.sZip%>">
<input type="hidden" name="label_sZip" value="Zip Code">
<input type="hidden" name="required_sZip" value="1">

</td>
</tr>
<tr>
<td class="SmallText"><b>Country</b></td>
<td><input type="text" maxlength="50" name="sCountry" value="<%=store.sCountry%>"></td>
</tr>
<tr>
<td class="SmallText"><b>Phone</b></td>
<td><input type="text" maxlength="16" name="sPhone" value="<%=store.sPhone%>">
<input type="hidden" name="label_sPhone" value="Phone">
<input type="hidden" name="required_sPhone" value="1"></td>
</tr>
<tr>
<td class="SmallText"><b>Marketing Email Contact</b></td>
<td><input type="text" maxlength="100" name="sMarEmail" value="<%=store.sMarEmail%>">
<input type="hidden" name="label_sMarEmail" value="Marking Email Contact">
<input type="hidden" name="required_sMarEmail" value="1">
<input type="hidden" name="valid_sMarEmail" value="isemail"></td>
</tr>
<td class="SmallText"><b>Problem Email Contact</b></td>
<td><input type="text" maxlength="100" name="sProblemEmail" value="<%=store.sProblemEmail%>">
<input type="hidden" name="label_sProblemEmail" value="Problem Email Contact">
<input type="hidden" name="required_sProblemEmail" value="1">
<input type="hidden" name="valid_sProblemEmail" value="isemail"></td>
</tr>
<tr>
<td class="SmallText"><b>Host Name</b></td>
<td><input type="text" maxlength="100" name="sHostName" value="<%=store.sHostName%>">
</td>
</tr>

</table>

</form>

<a class="NavLink" href="javascript:SubmitForm('frmUpdateStore','')">Update Store</a><br>
<a class="NavLink" href="Stores.aspx">Cancel</a><br>

<div class="SectionTitle">Store Devices:</div>

<% DoListView(); %>

<br>
<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
<!--	TODO : Place Test information here -->
</div>
</div>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
