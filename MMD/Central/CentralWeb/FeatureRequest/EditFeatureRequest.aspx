<%@ Page language="c#" Codebehind="EditFeatureRequest.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.FeatureRequest.EditFeatureRequest" %>
<% CSWContext.PageTitle = "Feature Requests &gt; Edit Feature Request"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<form name="frmEditFeatureRequest" method="post" action="/CentralWeb/FeatureRequest/EditFeatureRequest.aspx">
<input type="hidden" name="cmd" value="FeatureRequestUpdate">
<input type="hidden" name="idFeatureRequest" value="<%=fr.idFeatureRequest%>">


<table cellpadding=0 cellspacing=0>
<tr><td class="SmallText"><b>Item Id</b></td>
<td><input type="text" name="idItem" maxlength="10" value="<%=(fr.idFeatureRequest != 0) ? fr.idItem.ToString() : ""%>">
<input type="hidden" name="label_idItem" value="Item id">
<input type="hidden" name="required_idItem" value="1">
<input type="hidden" name="value_idItem" value="positiveLong">
</td>
</tr>
<tr><td class="SmallText"><b>Author</b></td>
<td><input type="text" name="sAuthor" maxlength="100" value="<%=fr.sAuthor%>">
<input type="hidden" name="label_sAuthor" value="Author">
<input type="hidden" name="required_sAuthor" value="1">
</td>
</tr>
<tr>
<td class="SmallText" valign=top><b>Feature Text</b></td>
<td><textarea name="txFeature" cols="50" rows="6"><%=fr.txFeature%></textarea>
<input type="hidden" name="label_txFeature" value="Feature Text">
<input type="hidden" name="required_txFeature" value="1">
</td>
</tr>
<tr><td class="SmallText"><b>Date Changed</b></td>
<td class="SmallText"><%=fr.dtChanged.value%></td>
</tr>
<tr><td class="SmallText"><b>Changed By</b></td><td class="SmallText"><%=fr.sChangedBy%></td></tr>
</table>

<br>
<% if(fr.idFeatureRequest != 0) { %>
<!-- Updating existing loop item -->
<a class="NavLink" href="javascript:SubmitForm('frmEditFeatureRequest','')">Update Feature Request</a><br>
<a class="NavLink" href="/CentralWeb/FeatureRequest/ViewFeatureRequests.aspx">View Feature Requests</a>
<% } else { %>
<!-- Adding new loop item -->
<a class="NavLink" href="javascript:SubmitForm('frmEditFeatureRequest','')">Add Feature Request</a><br>
<a class="NavLink" href="/CentralWeb/FeatureRequest/ViewFeatureRequests.aspx">Cancel</a>
<% } %>

</form>


<br>
<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
<!--	TODO : Place Test information here -->
</div>
</div>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
