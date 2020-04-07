<%@ Page language="c#" Codebehind="RecListItemAttrEdit.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Profiles.RecListItemAttrEdit" %>
<%@ Import NameSpace="MMD.Central.CentralLib.XmlObjects" %>
<% CSWContext.PageTitle = "Manage Recommend Lists > Edit Recommend List > Edit List Item Attributes"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<form name="frmRecListItemEdit" method=post action="/CentralWeb/Profiles/RecListEdit.aspx">
<input type="hidden" name="cmd" value="RecListItemEdit">
<input type="hidden" name="idRecList" value="<%=recListItem.idRecList%>">
<input type="hidden" name="idRecListItem" value="<%=recListItem.idRecListItem%>">
<input type="hidden" name="iSeq" value="<%=recListItem.iSeq%>">

<table class="StdTable" cellspacing=0>
<tr><td colspan="4"><span class="SmallText"><b>[<%=recList.Type.sName%>] List Item Attributes</b></span></td></tr>
<tr>
<td><span class="SmallText"><b>Sequence</b></span></td>
<td><span class="SmallText"><b>Item ID</b></span></td>
<% if(recList.Type.BaseType.sName == "Product") { %>
<td><span class="SmallText"><b>Title</b></span></td>
<td><span class="SmallText"><b>Artist / Director</b></span></td>
<% } else { %>
<td>&nbsp;</td><td>&nbsp;</td>
<% } %>
</tr>
<tr>
<td><span class="SmallText"><%=recListItem.iSeq%></span></td>
<td>
	<span class="SmallText">
	<input type="text" size="8" maxlength="8" name="idItem" value="<%=recListItem.idItem%>">
	<input type="hidden" name="label_idItem" value="">
	<input type="hidden" name="required_idItem" value="1">
	</span>
</td>
<% if(recList.Type.BaseType.sName == "Product") { %>
<td><span class="SmallText"><%=Title%></span></td>
<td><span class="SmallText"><%=Artist%></span></td>
<% } else { %>
<td>&nbsp;</td><td>&nbsp;</td>
<% } %>
</tr>

<%	
RecListItemAttribute[] rlias = recListItem.RecListItemAttributes;

if(rlias.Length > 0)
{
	int idx = 0;
%>
<tr><td colspan="4">&nbsp;</td></tr>
<tr><td colspan="4"><span class="SmallText"><b>[<%=recList.Type.sName%>] List Item Extended Attributes</b></span></td></tr>
<%	foreach(RecListItemAttribute rlia in rlias)	{ %>
<input type="hidden" name="ixAttr_<%=idx%>" value="<%=rlia.idRecListItemAttribute%>">
<tr><td colspan="4"><span class="SmallText"><b><%=rlia.TypeItemAttribute.sName%></b> [<%=rlia.TypeItemAttribute.AttributeType.sName%>]

<%	if(rlia.TypeItemAttribute.bRequired == true) { %>
	<span style=" color: red;">*</span></td></tr>
	<input type="hidden" name="label_ixAttrValue_<%=idx%>" value="<%=rlia.TypeItemAttribute.sName%>">
	<input type="hidden" name="required_ixAttrValue_<%=idx%>" value="1">
<% } %>
</span></td></tr>

<tr><td colspan="4">
<% if(rlia.TypeItemAttribute.AttributeType.sName == "Text") { %>
<textarea cols="50" rows="3" name="ixAttrValue_<%=idx%>"><%=rlia.txValue%></textarea>

<% } else if(rlia.TypeItemAttribute.AttributeType.sName == "Number") { %>
<input type="text" size="20" maxlength="255" name="ixAttrValue_<%=idx%>" value="<%=NullableInt.GetValue(rlia.iValue)%>">

<% } else if(rlia.TypeItemAttribute.AttributeType.sName == "String") { %>
<input type="text" size="50" maxlength="255" name="ixAttrValue_<%=idx%>" value="<%=rlia.sValue%>">

<% } else if(rlia.TypeItemAttribute.AttributeType.sName == "Binary") { %>
<%=CSWContext.Lookups.YesNo.GetHTMLComboBox("ixAttrValue_" + idx, Convert.ToString(NullableInt.GetValue(rlia.iValue)), true, true)%>

<% } else if(rlia.TypeItemAttribute.AttributeType.sName == "URL") { %>
<input type="text" size="50" maxlength="255" name="ixAttrValue_<%=idx%>" value="<%=rlia.sValue%>">

<% } else if(rlia.TypeItemAttribute.AttributeType.sName == "File") { %>
<%=CSWContext.Lookups.RecListFiles(recList.idStoreConcept).GetHTMLComboBox("ixAttrValue_" + idx, Convert.ToString(NullableInt.GetValue(rlia.iValue)), true, true)%>

<% } else { %>
<span class="SmallText"><b>ERROR: Unknown List Extended Attribute Type!</b></span>

<% } %>
</td></tr>
<tr><td colspan="4">&nbsp;</td></tr>

<% idx++;
 }
} %>

<input type="hidden" name="ixAttrCount" value="<%=rlias.Length%>">

</table>
</form>
<span class="SmallText"><span style="color: red">*</span>&nbsp;- Required Field</span>
<br>

<a class="NavLink" href="javascript:SubmitForm('frmRecListItemEdit','')" title="Submit all changes to this Recommendation List Item">Update List Attribute Changes</a>
<br>
<!-- Do a Back Button Here -->
<a class="NavLink" href="/CentralWeb/Profiles/RecLIstEdit.aspx?idRecList=<%=recList.idRecList%>">Back to Edit [<%=recList.Type.sName%>] Recommendation List</a>
<br><br>

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
