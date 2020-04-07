<%@ Page language="c#" Codebehind="AddAttractLoopItem.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.AttractLoops.AddAttractLoopItem" %>
<% CSWContext.PageTitle = "Manage Attract Loops &gt; Add an Attract Loop File"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<p class="SmallText">Create a new attract loop item by specifying a name for and uploading the attract loop file.</p>

<form name="frmAddAttractLoopItem" method="post" enctype="multipart/form-data" action="/CentralWeb/AttractLoops/AddAttractLoopItem.aspx">
<input type="hidden" name="cmd" value="FileAdd">
<input type="hidden" name="sSuccessPage" value="ViewAttractLoopItems.aspx">
<input type="hidden" name="MMDPostedFileType" value="1">

<p class="SmallText">
Attract Loop Item Name<br>
<input type="text" name="sName" size="25" maxlength="200">
<input type="hidden" name="label_sName" value="Attract Loop Item name">
<input type="hidden" name="required_sName" value="1">
</p>

<p class="SmallText">
Store Concept<br>
<%=CSWContext.Lookups.StoreConcept.GetHTMLComboBox("idStoreConcept")%>
<input type="hidden" name="label_idStoreConcept" value="Store Concept">
<input type="hidden" name="required_idStoreConcept" value="1">

</p>

<p class="SmallText">
Attract Loop Item Type<br>
<select name="iType">
	<option value="" selected></option>
	<option value="1">Image</option>
	<option value="2">Video</option>
	<option value="3">Screen ML</option>
</select>
<input type="hidden" name="label_iType" value="Attract Loop Item Type">
<input type="hidden" name="required_iType" value="1">
</p>

<p class="SmallText">
Attract Loop Item File<br>
<input type="file" name="file">
<input type="hidden" name="label_file" value="Attract Loop Item File">
<input type="hidden" name="required_file" value="1">
</p>

<a class="NavLink" href="javascript:SubmitForm('frmAddAttractLoopItem','AddAttractLoopItem')">Create this Attract Loop Item</a>

</form>


<br>
<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
<!--	TODO : Place Test information here -->
<ul>
<li>Attract Loop Item Name - required</li>
<li>Attract Loop Item Type - required
<ul>
<li>Image - valid file types - *.bmp, *.gif, *.jpeg, *.jpg, *.png</li>
<li>Video - valid file types - *.asf, *.wmv</li>
<li>Screen ML - valid file types - *.xml</li>
</ul>
</li>
<li>Attract Loop Item File - required</li>

</ul>
</div>
</div>


<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
