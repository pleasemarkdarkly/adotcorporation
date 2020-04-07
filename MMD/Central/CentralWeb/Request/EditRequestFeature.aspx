<%@ Page language="c#" Codebehind="EditRequestFeature.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.RequestFeature.EditRequestFeature" %>
<% CSWContext.PageTitle = "Request Features &gt; Edit Request Feature"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<form name="frmEditRequestFeature" method="post" action="/CentralWeb/RequestFeature/EditRequestFeature.aspx">
<input type="hidden" name="cmd" value="RequestFeatureUpdate">
<input type="hidden" name="idRequestFeature" value="<%=rf.idRequestFeature%>">


<table cellpadding=0 cellspacing=0>
<tr><td class="SmallText"><b>Item Id</b></td>
<td><input type="text" name="idItem" maxlength="10" value="<%=(rf.idRequestFeature != 0) ? rf.idStoreConcept.ToString() : ""%>">
<input type="hidden" name="label_idItem" value="Item id">
<input type="hidden" name="required_idItem" value="1">
<input type="hidden" name="value_idItem" value="positiveLong">
</td>
</tr>
<tr><td class="SmallText"><b>Author</b></td>
<td><input type="text" name="sAuthor" maxlength="100" value="<%=rf.sAuthor%>">
<input type="hidden" name="label_sAuthor" value="Author">
<input type="hidden" name="required_sAuthor" value="1">
</td>
</tr>
<tr>
<td class="SmallText" valign=top><b>Feature Text</b></td>
<td><textarea name="txFeature" cols="50" rows="6"><%=rf.txFeature%></textarea>
<input type="hidden" name="label_txFeature" value="Feature Text">
<input type="hidden" name="required_txFeature" value="1">
</td>
</tr>
<tr><td class="SmallText"><b>Date Changed</b></td>
<td class="SmallText"><%=rf.dtChanged.value%></td>
</tr>
<tr><td class="SmallText"><b>Changed By</b></td><td class="SmallText"><%=rf.sChangedBy%></td></tr>
</table>

<br>
<% if(rf.idRequestFeature != 0) { %>
<!-- Updating existing loop item -->
<a class="NavLink" href="javascript:SubmitForm('frmEditRequestFeature','')">Update Request Feature</a><br>
<a class="NavLink" href="/CentralWeb/RequestFeature/ViewRequestFeatures.aspx">View Request Features</a>
<% } else { %>
<!-- Adding new loop item -->
<a class="NavLink" href="javascript:SubmitForm('frmEditRequestFeature','')">Add Request Feature</a><br>
<a class="NavLink" href="/CentralWeb/RequestFeature/ViewRequestFeatures.aspx">Cancel</a>
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
