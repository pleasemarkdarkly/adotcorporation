<%@ Page language="c#" Codebehind="Home.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Help.Home" %>
<% CSWContext.PageTitle = "Help Popups"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<script>
function Popup(sURL)
{
	window.open(sURL,"popup","menubar=no,status=no,location=no,directories=no,toolbar=no,width=350,height=400,resizable=yes,scrollbars=yes");
}
</script>


<p class="SmallText"><a class="NavLink" href="javascript:Popup('CreateSpecialOffer.htm')">How To Create a Special Offer List</a></p>

<p class="SmallText"><a class="NavLink" href="javascript:Popup('CreateTop20List.htm')">How To Create a Top 20 List</a></p>

<p class="SmallText"><a class="NavLink" href="javascript:Popup('CreateNewReleases.htm')">How To Create a New Releases List</a></p>

<p class="SmallText"><a class="NavLink" href="javascript:Popup('UploadAttractLoopFiles.htm')">How To Upload Attract Loop Files</a></p>

<p class="SmallText"><a class="NavLink" href="javascript:Popup('CreateAttractLoopSequence.htm')">How To Create an Attract Loop Sequence</a></p>

<br>
<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
<!--	TODO : Place Test information here -->
</div>
</div>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
