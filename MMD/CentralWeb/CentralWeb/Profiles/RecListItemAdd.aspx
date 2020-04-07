<%@ Page language="c#" Codebehind="RecListItemAdd.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Profiles.RecListItemAdd" %>
<% CSWContext.PageTitle = ""; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<script language="javascript">

function SubmitCommand(sCommand,idRecListItem)
{
	window.opener.SubmitCommand(sCommand,idRecListItem);
}

</script>

<div class="PageTitle">Add Items To Recommendation List</div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

<% DoListView(); %>

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



