<%@ Page language="c#" Codebehind="FileAdd.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Profiles.FileAdd" %>
<% CSWContext.PageTitle = "Manage Recommendation Lists &gt; Add File"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<p class="SmallText">Create a new recommend list file by specifying a name for and uploading the  file.</p>

<form name="frmAddFile" method="post" enctype="multipart/form-data" action="/CentralWeb/Profiles/FileView.aspx">
<input type="hidden" name="cmd" value="FileAdd">
<input type="hidden" name="MMDPostedFileType" value="2">

<p class="SmallText">
Recommendation List File Name<br>
<input type="text" name="sName" size="25" maxlength="200">
<input type="hidden" name="label_sName" value="Recommendation List File Name">
<input type="hidden" name="required_sName" value="1">
</p>

<p class="SmallText">
Store Concept<br>
<%=CSWContext.Lookups.StoreConcept.GetHTMLComboBox("idStoreConcept","0")%>
<input type="hidden" name="label_idStoreConcept" value="Store Concept">
<input type="hidden" name="required_idStoreConcept" value="1">

</p>

<p class="SmallText">
Recommendation List File Type : Image<br>
<input type="hidden" name="iType" value="1">

<input type="hidden" name="label_iType" value="Recommendation List File Type">
<input type="hidden" name="required_iType" value="1">
</p>

<p class="SmallText">
Recommendation List File<br>
<input type="file" name="file">
<input type="hidden" name="label_file" value="Recommendation List File">
<input type="hidden" name="required_file" value="1">
</p>

<a class="NavLink" href="javascript:SubmitForm('frmAddFile','AddFile')">Add Recommendation List File</a>

</form>


<br>
<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
<!--	TODO : Place Test information here -->
<ul>
<li>Recommend List File Name - required</li>
<li>Recommend List File Type - required
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

