<%@ Page language="c#" Codebehind="RecListTypeView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Profiles.RecListTypeView" %>
<%@ Import NameSpace="MMD.Central.CentralLib.XmlObjects" %>
<% CSWContext.PageTitle = "Manage Recommend Lists > Manage Recommend List Types"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<div class="SmallText">These Recommend List Types already exist.</div>
<br>
<% RecListType[] rlts = GetRecListTypes(); %>

<% foreach(RecListType rlt in rlts) {
if(rlt.BaseType.bSystem == false)
{

 %>
<hr>
<table border="0" width="100%" cellpadding="1" cellspacing="1">
	<tr>
		<td width="40%"><span class="SmallText"><b><%=rlt.sName%>&nbsp;List</b></span></td>
		<td width="15%"><span class="SmallText"><b><%=rlt.BaseType.sName%></b></span></td>
		<td width="15%"><span class="SmallText"><b><%=GetStatus(rlt)%></b></span></td>
		<td width="10%"><span class="SmallText"><b><%=Edit(rlt)%></b></span></td>
		<td width="10%"><span class="SmallText"><b><%=Undo(rlt)%></b></span></td>
		<td width="20%"><span class="SmallText"><b><%=Delete(rlt)%></b></span></td>
	</tr>
	<tr>
		<td><font face="Trebuchet MS" size="2">Concept [attribute]</font></td>
		<td><font face="Trebuchet MS" size="2">Number</font></td>
		<td>&nbsp;</td>
	</tr>
	<tr>
		<td><font face="Trebuchet MS" size="2">Product Type [attribute]</font></td>
		<td><font face="Trebuchet MS" size="2">Number</font></td>
		<td>&nbsp;</td>
	</tr>
	<tr>
<% if(rlt.RecListAttributes.Length > 0) { 
		foreach(RecListTypeAttribute rlta in rlt.RecListAttributes)
		{ %>
	<tr>
		<td><font face="Trebuchet MS" size="2"><%=rlta.sName%></font></td>
		<td><font face="Trebuchet MS" size="2"><%=rlta.AttributeType.sName%></font></td>
		<td>&nbsp;</td>
	</tr>
<%		}
 } else { %>
	<tr>
		<td><font face="Trebuchet MS" size="2">list type has no extended attributes</font></td>
		<td><font face="Trebuchet MS" size="2">N/A</font></td>
		<td>&nbsp;</td>
	</tr>
<% } %>
	<tr>
		<td colspan="3"><span class="SmallText"><b><%=rlt.sName%>&nbsp;List Items</b></span></td>
	</tr>
	<tr>
		<td><font face="Trebuchet MS" size="2">Sequence [attribute]</font></td>
		<td><font face="Trebuchet MS" size="2">Number</font></td>
		<td>&nbsp;</td>
	</tr>	
<% if(rlt.RecListItemAttributes.Length > 0) {
		foreach(RecListTypeItemAttribute rltia in rlt.RecListItemAttributes)
		{ %>
	<tr>
		<td><font face="Trebuchet MS" size="2"><%=rltia.sName%></font></td>
		<td><font face="Trebuchet MS" size="2"><%=rltia.AttributeType.sName%></font></td>
		<td>&nbsp;</td>
	</tr>
<%		}
 } else { %>
	<tr>
		<td><font face="Trebuchet MS" size="2">list item type has no attributes</font></td>
		<td><font face="Trebuchet MS" size="2">N/A</font></td>
		<td>&nbsp;</td>
    </tr>
<% } %>
	
</table>
<hr>


<% } 
} %>
<div class="NavLink"><%=SubmitAll()%></div><br>
<a class="NavLink" href="/CentralWeb/Profiles/RecListTypeAdd.aspx">Create a Recommend List Type</a><br><br>

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

