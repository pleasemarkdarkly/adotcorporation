<%@ Page language="c#" Codebehind="RecListTypeAdd.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Profiles.RecListTypeAdd" %>
<% CSWContext.PageTitle = "Manage Recommend Lists > Add Recommend List Type"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<div class="SmallText">Creating a recommend list type allows you to 
create new recommend lists based upon this type.  After you create the List 
Type, you can then create attributes for the list and individual list items on 
the Edit Recommend List Items page.</div>

<form name="frmRecListTypeAdd" method=post action="/CentralWeb/Profiles/RecListTypeEdit.aspx">
	<table class="StdTable" cellspacing=0>
	<tr class="StdRow">
		<td class="StdHdr">Recommend List Type Name</td>
		<td class="StdHdr">Recommend List Items</td>
		<td class="StdHdr">&nbsp;</td>
	</tr>
	<tr class="StdRow">
		<td>
		<input size="30" maxlength="25" name="sName" value="">
		<input type="hidden" name="label_sName" value="Recommend List Type Name">
		<input type="hidden" name="required_sName" value="1">
		</td>
		<td align="center" id="idBaseType"><%=RecListBaseTypeComboBox%></td>
		<td align="center">
		<input type="submit" value="Next >" onclick="SubmitForm('frmRecListTypeAdd',''); return false;" title="Create a new Recommendation List Type">
		<input type="hidden" name="cmd" value="RecListTypeAdd">
		</td>
	</tr>
	</table>
</form>

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

