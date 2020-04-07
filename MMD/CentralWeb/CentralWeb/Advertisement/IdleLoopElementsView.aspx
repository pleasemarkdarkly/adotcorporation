<%@ Page language="c#" Codebehind="IdleLoopElementsView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Advertisement.IdleLoopElementsView" %>
<% CSWContext.PageTitle = "Manage Idle Loop Frames - " + IdleLoop.sName; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<script language="javascript">

function SubmitCommand(sCommand,idIdleLoopElement)
{
	var ele = document.getElementById("CommandInput");
	if(ele)
	{
		ele.value = sCommand;
		ele = document.getElementById("CommandId");
		if(ele)
		{
			ele.value = idIdleLoopElement;
			document.all["frmMain"].submit();
			return;
		}
		alert("idIdleLoopElement not found");
	}
	alert("CommandInput not found");
}
</script>

<a class="BreadCrumb" href="/CentralWeb/Advertisement/IdleLoopView.aspx?idIdleLoop=<%=IdleLoop.idIdleLoop%>">Manage Idle Loop - <%=IdleLoop.sName%></a><br>
<div class="PageTitle"><%=CSWContext.PageTitle%></div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

list view, add, delete, reorder, edit<br>

<% DoListView(); %>

<div class="SectionTitle">Actions</div>
<a href="/CentralWeb/Advertisement/IdleLoopElementEdit.aspx?idIdleLoopElement=0&idIdleLoop=<%=IdleLoop.idIdleLoop%>">Add New Frame to Loop</a><br>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
