<%@ Page language="c#" Codebehind="RecListItemView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Profiles.RecListItemView" %>
<% CSWContext.PageTitle = "Manage Recommendation List - " + Title; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<script language="javascript">

function SubmitCommand(sCommand,idRecListItem)
{
	var ele = document.getElementById("CommandInput");
	if(ele)
	{
		ele.value = sCommand;
		ele = document.getElementById("idRecListItem");
		if(ele)
		{
			ele.value = idRecListItem;
			document.all["frmMain"].submit();
			return;
		}
		alert("idRecListItem not found");
	}
	alert("CommandInput not found");
}

function WindowOpen(idRecList)
{
	window.open('/CentralWeb/Profiles/RecListItemAdd.aspx?idRecList=' + idRecList,'_blank','directories=no, fullscreen=no, resizable=yes, scrollbars=yes, status=yes, location=no, menubar=no, toolbar=no, height=550, width=700');
	
}
</script>

<a class="BreadCrumb" href="/CentralWeb/Profiles/RecListView.aspx">Manage Recommendation Lists</a><br>
<div class="PageTitle">Manage Recommendation List<br><span class="PageSubtitle"><%=Title%></span></div>

<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

<% DoListView(); %>

<div class="SectionTitle">Other Actions</div>
<a href="javascript:WindowOpen(<%=idRecList%>)">Add Items To Recommendation List</a>

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


