<%@ Page language="c#" Codebehind="RecListTypeEdit.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Profiles.RecListTypeEdit" %>
<% CSWContext.PageTitle = "Manage Recommend Lists > Edit Recommend List Type"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<div class="SmallText">Specify Attributes for a Recommend List Type.  You will only be able to do this if no recommend lists have as yet been created of this list type.</div>

<table class="StdTable" cellspacing=0>
  <tr class="StdRow">
    <td class="StdHdr">Recommend List Type Name&nbsp;&nbsp;&nbsp;&nbsp;</td>
    <td class="StdHdr">Recommend List Items</td>
  </tr>
  <tr class="StdRow">
    <td class="SmallText"><%=RecListNewTypeName%></td>
    <td class="SmallText"><%=RecListNewTypeBaseTypeName%></td>
  </tr>
</table>
<br>

<div class="SectionTitle">Recommend List Attributes</div><br>
<div class="SmallText">Here you can specify attributes associated with recommend lists.<br>These will be used to display information about an entire list of products.</div>
<br>


<table class="StdTable" cellspacing=0>
  <!-- display currently defined attributes -->
  <% DoListAttributeListView(); %>
  <tr class="StdRow">
    <form name="frmRecListTypeAttributeAdd" method=post action="/CentralWeb/Profiles/RecListTypeEdit.aspx">
      <td>
        <input size="30" maxlength="25" name="sName" value="">
        <input type="hidden" name="label_sName" value="Recommend List Attribute Name">
        <input type="hidden" name="required_sName" value="1">
      </td>
      <td align="center"><%=RecListAttributeTypeComboBox%></td>
      <td align="center"><input type="checkbox" value="1" name="bRequired"></td>
      <td>
        <input type="submit" value="Add" onclick="SubmitForm('frmRecListTypeAttributeAdd',''); return false;" title="Create a new attribute for this Recommendation List Type">
	    <input type="hidden" name="cmd" value="RecListTypeAttributeAdd">
		<input type="hidden" name="idRecListType" value="<%=idRecListType%>">
      </td>
    </form>
  </tr>
</table>
<hr>
<br>

<div class="SectionTitle">Recommend List Item Attributes</div><br>
<div class="SmallText">Here you can specify attributes associated with individual list items.<br>These will be used to display information about an item in a list of products.</div>
<br>

<table class="StdTable"  cellspacing=0>
  <!-- display currently defined list items + attributes -->
   <% DoItemAttributeListView(); %> 
  <tr class="StdRow">
    <form name="frmRecListTypeItemAttributeAdd" method=post action="/CentralWeb/Profiles/RecListTypeEdit.aspx">
      <td>
        <input size="30" maxlength="25" name="sName" value="">
        <input type="hidden" name="label_sName" value="List Item Attribute Name">
        <input type="hidden" name="required_sName" value="1">
      </td>
      <td align="center"><%=RecListAttributeTypeComboBox%></td>
      <td align="center"><input type="checkbox" value="1" name="bRequired"></td>
      <td>
        <input type="submit" value="Add" onclick="SubmitForm('frmRecListTypeItemAttributeAdd',''); return false;" title="Create a new attribute for this Recommendation List Type">
        <input type="hidden" name="cmd" value="RecListTypeItemAttributeAdd">
        <input type="hidden" name="idRecListType" value="<%=idRecListType%>">
      </td>
    </form>
  </tr>
</table>
<hr>
<br>
  
  <!--
  <input type="submit" name="cmd" value="frmRecListTypeItemAttributeAdd" title="Create a new Recommendation List with the attributes defined above">
    -->

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

