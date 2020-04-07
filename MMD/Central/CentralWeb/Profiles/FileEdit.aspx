<%@ Page language="c#" Codebehind="FileEdit.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Profiles.FileEdit" %>
<%@ Import NameSpace="MMD.Central.CentralLib.XmlObjects" %>
<% CSWContext.PageTitle = "Manage Recommendation Lists &gt; Update File"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<p class="SmallText">Updated a recommendation list file by modifying its name and optionally uploading a new file.</p>

<% MMDFile file = File; %>

<form name="frmUpdateFile" method="post" enctype="multipart/form-data" action="/CentralWeb/Profiles/FileEdit.aspx">
<input type="hidden" name="cmd" value="FileUpdate">
<input type="hidden" name="idFile" value="<%=file.idFile%>">
<input type="hidden" name="MMDPostedFileType" value="<%=(int)file.FileType%>">

<p class="SmallText">
Recommendation List File Name<br>
<input type="text" name="sName" size="25" maxlength="200" value="<%=file.sName%>">
<input type="hidden" name="label_sName" value="Attract Loop Item name">
<input type="hidden" name="required_sName" value="1">
</p>

<p class="SmallText">
Store Concept<br>
<%=CSWContext.Lookups.StoreConcept.GetHTMLComboBox("idStoreConcept",file.idStoreConcept.ToString())%>
<input type="hidden" name="label_idStoreConcept" value="Store Concept">
<input type="hidden" name="required_idStoreConcept" value="1">
</p>

<p class="SmallText">
Recommendation List File Type : Image<br>
<input type="hidden" name="iType" value="1">
<input type="hidden" name="label_iType" value="Attract Loop Item Type">
<input type="hidden" name="required_iType" value="1">
</p>

<p class="SmallText">
Recommendation List File : <a class="NavLink" href="<%=FileHref()%>"><%=file.sFileName%></a><br>
<input type="file" name="file">
<input type="checkbox" id="bNewFile" name="bNewFile" value="1"><label for="bNewFile">Upload New File</label>
<input type="hidden" name="label_file" value="Recommendation List File">
</p>

<a class="NavLink" href="javascript:SubmitForm('frmUpdateFile','UpdateFile')">Update Recommendation List File</a>

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
<li>File link - view current file</li>
<li>Attract Loop Item File - if checkbox checked, new file required, must match valid file type</li>
<li>Update link - change file name, if checkbox checked, upload new file, which matches new file type</li>
</ul>
</div>
</div>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>