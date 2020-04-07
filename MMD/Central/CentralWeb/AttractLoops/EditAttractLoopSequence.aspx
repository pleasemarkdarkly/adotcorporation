<%@ Page language="c#" Codebehind="EditAttractLoopSequence.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.AttractLoops.EditAttractLoopSequence" %>
<%@ Import NameSpace="MMD.Central.CentralLib.XmlObjects" %>
<% CSWContext.PageTitle = "Manage Attract Loops > Edit Attract Loop Sequence "; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<p><span class="SmallText">Click [Submit Changes] immediately after changing the loop sequence name to prevent loss of unsaved changes.</span></p>

<form name="frmUpdateLoop" action="EditAttractLoopSequence.aspx" method="post">
<input type="hidden" name="cmd" value="AttractLoopUpdate">
<input type="hidden" name="idIdleLoop" value="<%=idleLoop.idIdleLoop%>">

<span class="SectionTitle">Attract Loop Sequence Attributes:</span>&nbsp;<br>
<table cellpadding=0 cellspacing = 0 style="border-top: solid 1px black">
<tr>
<td><span class="SmallText"><b>Loop Sequence Name&nbsp;</b></span></td>
<td>
<input size="25" type="text" maxlength="200" name="sName" value="<%=idleLoop.sName%>">
<input type="hidden" name="label_sName" value="Loop Sequence name">
<input type="hidden" name="required_sName" value="1">
</td>
</tr>
<tr><td class="SmallText"><b>Store Concept</b></td>
<td><%=CSWContext.Lookups.StoreConcept.GetHTMLComboBox("idStoreConcept",idleLoop.idStoreConcept.ToString())%></td>
</tr>
<!--
<tr>
<td valign=top><span class="SmallText"><b>Info</b></span></td>
<td>
<textarea name="sInfo" cols=50 rows=5><%=idleLoop.sInfo%></textarea>
<input type="hidden" name="label_sInfo" value="Info">
<input type="hidden" name="length_sInfo" value="1000">
</td>
</tr>
-->
<tr>
<td><span class="SmallText"><b>Change Date</b></span></td>
<td class="SmallText"><%=NullableDateTime.GetValue(idleLoop.dtChanged,null)%></td>
</tr>
<tr>
<td class="SmallText"><b>Changed By</b></td>
<td class="SmallText"><%=idleLoop.sChangedBy%></td>
</tr>
</table>

<a class="NavLink" href="javascript:SubmitForm('frmUpdateLoop','')">Submit Changes to Attract Loop Sequence</a>

</form>

<div class="SectionTitle">Attract Loop Sequence Items:</div>
<table cellpadding=0 cellspacing=0 border=0 style="border-top: solid 1 px black">
<% DoAttractLoopItemsListView(); %>
<% if(IdleLoopItems == 0) { %>
<tr><td colspan="6" align=center><span class="SmallText"><em>No Attract Loop Items</em></span></td></tr>
<% } %>
</table>


<br>
<span class="SectionTitle" style="border-bottom: solid 1px black">Add a Special Item to this Attract Loop:</span>&nbsp;
<div style="">
<a class="NavLink" href="EditAttractLoopSequenceItem.aspx?idIdleLoop=<%=idleLoop.idIdleLoop%>&iFrameType=4">Add Product List sequence to this Attract Loop</a><br>
<a class="NavLink" href="EditAttractLoopSequenceItem.aspx?idIdleLoop=<%=idleLoop.idIdleLoop%>&iFrameType=5">Add Promotion List sequence to this Attract Loop</a><br>
<a class="NavLink" href="EditAttractLoopSequenceItem.aspx?idIdleLoop=<%=idleLoop.idIdleLoop%>&iFrameType=6">Add a single Product page to this Attract Loop</a><br>
</div>
<br>
<div class="SectionTitle">Add a File to this Attract Loop:</div>

<table cellpadding=0 cellspacing=0  style="border-top: solid 1 px black">
<% DoAttractLoopItemFileListView(); %>
</table>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>