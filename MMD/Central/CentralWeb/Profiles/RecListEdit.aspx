<%@ Page language="c#" Codebehind="RecListEdit.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Profiles.RecListEdit" %>
<% CSWContext.PageTitle = "Manage Recommend Lists > Edit Recommendation List"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<script>

function ViewItems()
{
	var ele = document.forms["frmRecListEdit"].elements["idProductType"];
	if(ele.value == '')
		WarnMsg(ele,'You must specify a product type before viewing items.',true);
	else
		window.open('RecListItemAdd.aspx?idProductType=' + ele.value,"ViewItems");
}

</script>

<span class="SmallText"><b>Edit [<%=RecListType.sName%>] List</b></span>
<hr>

<form name="frmRecListEdit" method=post action="/CentralWeb/Profiles/RecListEdit.aspx">
<table class="StdTable" cellspacing=0>
	<tr class="StdRow">
		<td><span class="SmallText"><b>Concept</b></span></td>
		<td><span class="SmallText"><%=CSWContext.Lookups.StoreConcept.GetValue(recList.idStoreConcept.ToString())%></span></td>
		<input type="hidden" name="idStoreConcept" value="<%=recList.idStoreConcept%>">
	</tr>
<% if(RecListType.BaseType.sName == "Product") { %>
	<tr class="StdRow">
		<td><span class="SmallText"><b>Product Type</b></span></td>
		<td><span class="SmallText"><%=CSWContext.Lookups.ProductType.GetValue(recList.idProductType.ToString())%></span></td>
		<input type="hidden" name="idProductType" value="<%=recList.idProductType%>">
	</tr>
<% } %>
<% if(ExtendedAttributeCount() > 0) { %>
	<% DoListExtendedAttributes(); %>
	<tr class="StdRow">
		<td colspan="2">
			<span class="SmallText"><span style="color: red">*</span>&nbsp;- Required Field</span>
		</td>
	</tr>
	<tr class="StdRow">
		<td colspan="2">
			<a class="NavLink" href="javascript:SubmitForm('frmRecListEdit','')">Update Recommend List</a>
			<input type="hidden" name="cmd" value="RecListEdit">
			<input type="hidden" name="idRecList" value="<%=idRecList%>">
		</td>
	</tr>
<% } %>
</table>
</form>
<hr>

<span class="SmallText"><b>Recommend List Items</b></span>	
<% DoItemListViewStart(); %>
	<% DoItemListViewRows(); %>	
	<tr>
<% if(RLTItemsHaveExtAttr()) { %>
	<form name="frmRecListItemAdd" method=post action="/CentralWeb/Profiles/RecListItemAttrEdit.aspx">
<% } else { %>
	<form name="frmRecListItemAdd" method=post action="/CentralWeb/Profiles/RecListEdit.aspx">
<% } %>
		<td class="LVViewCell" colspan="2">&nbsp;</td>
		<td class="LVViewCell">Add New:</td>
		<td class="LVViewCell">
			<input type="text" size="8" maxlength="8" name="idItem">
	        <input type="hidden" name="label_idItem" value="">
	        <input type="hidden" name="required_idItem" value="1">
		</td>
		<td class="LVViewCell">&nbsp;</td>
		<td class="LVViewCell">&nbsp;</td>
		<td class="">
			<a class="NavLink" title="Add an item to this Recommendation List" href="javascript:SubmitForm('frmRecListItemAdd','')">Add</a>
			<input type="hidden" name="cmd" value="RecListItemAdd">
			<input type="hidden" name="idRecList" value="<%=idRecList%>">
		</td>
		<td valign=middle >&nbsp;
<% if(RecListType.BaseType.sName == "Product") { %>
			<span class="SmallTextBold"><a  href="javascript:ViewItems()">Find Items</a></span>
<% } %>
&nbsp;
		</td>
	</form>
	</tr>
<% DoItemListViewEnd(); %>
<hr>
<br>
<a class="NavLink" href="/CentralWeb/Profiles/RecListView.aspx?idRecListBaseType=<%=RecListType.BaseType.idRecListBaseType%>">Back to [<%=RecListType.BaseType.sName%>] Recommendation List</a>
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
