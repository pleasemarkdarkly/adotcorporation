<%@ Page language="c#" Codebehind="EditAttractLoopSequenceItem.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.AttractLoops.EditAttractLoopSequenceItem" %>
<%@ Import Namespace="MMD.Central.CentralLib.XmlObjects" %>

<% CSWContext.PageTitle = "Manage Attract Loops > " + ((ile.idIdleLoopElement != 0) ? "Edit" : "Add") + " Attract Loop Item"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<form method="post" name="frmEditAttractLoopItem" action="/CentralWeb/AttractLoops/EditAttractLoopSequence.aspx">
<input type="hidden" name="cmd" value="AttractLoopItemUpdate">
<input type="hidden" name="idIdleLoopElement" value="<%=ile.idIdleLoopElement%>">
<input type="hidden" name="idIdleLoop" value="<%=il.idIdleLoop%>">
<input type="hidden" name="iFrameType" value="<%=(int)ile.frameType%>">
<input type="hidden" name="idFile" value="<%=idFile%>">

<table cellpadding=0 cellspacing=0>
<tr>
<td class="SmallText"><b>Loop Item Name&nbsp;</b></td>
<td>
<input type="text" name="sName" maxlength="100" value="<%=LoopItemName%>">
<input type="hidden" name="label_sName" value="Loop Item name">
<% if(!(ile is BaseRecListFrame)) { %>
<input type="hidden" name="required_sName" value="1">
<% } %>
</td>
</tr>
<tr>
<td class="SmallText"><b>Sequence</b></td>
<td class="SmallText"><%=(ile.idIdleLoopElement != 0) ? ile.iSeq.ToString() : "New"%></td>
</tr>
<tr>
<td class="SmallText"><b>Idle Loop Name</b></td>
<td class="SmallText"><%=il.sName%></td>
</tr>
<tr>
<td class="SmallText"><b>Frame Type</b></td>
<td class="SmallText"><%=ile.frameType%></td>
</tr>
<tr>
<td class="SmallText"><b>Date Changed</b></td>
<td class="SmallText"><%=NullableDateTime.GetValue(ile.dtChanged)%></td>
</tr>
<tr>
<td class="SmallText"><b>Changed By</b></td>
<td class="SmallText"><%=(ile.idIdleLoopElement != 0) ? ile.sChangedBy : CSWContext.UserName%></td>
</tr>
<tr><td>&nbsp;</td><td></td></tr>
<% if(ile is ScreenMLFrame) { %>
<tr>
<td class="SmallText"><b>File</b></td>
<td><a class="NavLink" href="<%=FileLink(ile.Files[0].sFileName)%>"><%=ile.Files[0].sFileName%></a></td>
</tr>

<tr>
<td class="SmallText"><b>Show Time</b></td>
<td><input type="text" size="4" maxlength="10" name="cShowTime" value="<%=(ile.idIdleLoopElement != 0) ? ((ScreenMLFrame)ile).cShowTime.ToString() : ""%>">
&nbsp;Seconds
<input type="hidden" name="label_cShowTime" value="Show Time">
<input type="hidden" name="required_cShowTime" value="1">
<input type="hidden" name="valid_cShowTime" value="positiveLong">
</td>
</tr>
<tr>
<td class="SmallText"><b>Query String</b></td>
<td><input type="text" name="sLink" size="15" maxlength="500" value="<%=(ile.idIdleLoopElement != 0) ? ((ScreenMLFrame)ile).sQueryString : ""%>"></td>
</tr>
<!--
<tr>
<td class="SmallText"><b>Template</b></td>
<td><input type="text" size="15" maxlength="100" value="<%=(ile.idIdleLoopElement != 0) ? ((ScreenMLFrame)ile).sTemplate : ""%>"></td>
</tr>
-->
<% } else if(ile is ImageFrame) { %>
<tr>
<td class="SmallText"><b>Show Time</b></td>
<td><input type="text" size="4" maxlength="10" name="cShowTime" value="<%=(ile.idIdleLoopElement != 0) ? ((ImageFrame)ile).cShowTime.ToString() : ""%>">
&nbsp;Seconds
<input type="hidden" name="label_cShowTime" value="Show Time">
<input type="hidden" name="required_cShowTime" value="1">
<input type="hidden" name="valid_cShowTime" value="positiveLong">
</td>
</tr>
<td class="SmallText"><b>Link</b></td>
<td><input type="text" name="sLink" size="15" maxlength="500" value="<%=(ile.idIdleLoopElement != 0) ? ((ImageFrame)ile).sLink : ""%>"></td>
</tr>
<% } else if(ile is VideoFrame) { %>
<tr>
<td class="SmallText"><b>Start Time</b></td>
<td><input type="text" size="4" maxlength="10" name="iStartTime" value="<%=(ile.idIdleLoopElement != 0) ? SafeString(NullableInt.GetValue(((VideoFrame)ile).iStartTime),"") : ""%>">
&nbsp;<span class="SmallText">milliseconds</span>
<input type="hidden" name="label_iStartTime" value="Start Time">
<input type="hidden" name="valid_iStartTime" value="nonnegLong">
</td>
</tr>
<tr>
<td class="SmallText"><b>End Time</b></td>
<td><input type="text" size="4" maxlength="10" name="iEndTime" value="<%=(ile.idIdleLoopElement != 0) ? SafeString(NullableInt.GetValue(((VideoFrame)ile).iEndTime),"") : ""%>">
&nbsp;<span class="SmallText">milliseconds</span>
<input type="hidden" name="label_iStartTime" value="End Time">
<input type="hidden" name="valid_iStartTime" value="nonnegLong">
</td>
</tr>
<% } else if(ile is BaseRecListFrame) { %>
<tr>
<td class="SmallText"><b>Show Time</b></td>
<td><input type="text" size="4" maxlength="10" name="cShowTime" value="<%=(ile.idIdleLoopElement != 0) ? ((BaseRecListFrame)ile).cShowTime.ToString() : ""%>">
&nbsp;Seconds
<input type="hidden" name="label_cShowTime" value="Show Time">
<input type="hidden" name="required_cShowTime" value="1">
<input type="hidden" name="valid_cShowTime" value="positiveLong">
</td>
</tr>
<tr>
<td class="SmallText"><b>List Type</b></td>
<td><%=RecListTypeComboBox((BaseRecListFrame)ile)%>
<input type="hidden" name="label_idRecListType" value="List type">
<input type="hidden" name="required_idRecListType" value="1">
</td>
</tr>
<tr>
<td class="SmallText"><b>Items Per Page</b></td>
<td><span class="SmallText">4</span>
<!--<input type="text" size="4" maxlength="10" name="cItemsPerPage" value="<%=(ile.idIdleLoopElement != 0) ? ((BaseRecListFrame)ile).cItemsPerPage.ToString() : ""%>">-->
<input type="hidden" name="cItemsPerPage" value="4">
<input type="hidden" name="label_cItemsPerPage" value="Items Per Page">
<input type="hidden" name="required_cItemsPerPage" value="1">
<input type="hidden" name="valid_cItemsPerPage" value="positiveLong"> 

</td>
</tr>

<% } else if(ile is ProductFrame) { %>
<tr>
<td class="SmallText"><b>Show Time</b></td>
<td><input type="text" size="4" maxlength="10" name="cShowTime" value="<%=(ile.idIdleLoopElement != 0) ? ((ProductFrame)ile).cShowTime.ToString() : ""%>">
&nbsp;Seconds
<input type="hidden" name="label_cShowTime" value="Show Time">
<input type="hidden" name="required_cShowTime" value="1">
<input type="hidden" name="valid_cShowTime" value="positiveLong">
</td>
</tr>
<tr><td class="SmallText"><b>Item Id</b></td>
<td><input type="text" size="8" maxlength="10" name="idItem" value="<%=(ile.idIdleLoopElement != 0) ? ((ProductFrame)ile).idItem.ToString() : ""%>">
<input type="hidden" name="label_idItem" value="Item id">
<input type="hidden" name="required_idItem" value="1">
<input type="hidden" name="valid_idItem" value="positiveLong">
</td>

</tr>
<% } %>
</table>

<% if(ile.idIdleLoopElement != 0) { %>
<!-- Updating existing loop item -->
<a class="NavLink" href="javascript:SubmitForm('frmEditAttractLoopItem','')">Update Attract Loop Item</a><br>
<a class="NavLink" href="/CentralWeb/AttractLoops/EditAttractLoopSequence.aspx?idIdleLoop=<%=il.idIdleLoop%>">Edit Attract Loop</a>
<% } else { %>
<!-- Adding new loop item -->
<a class="NavLink" href="javascript:SubmitForm('frmEditAttractLoopItem','')">Add Attract Loop Item</a><br>
<a class="NavLink" href="/CentralWeb/AttractLoops/EditAttractLoopSequence.aspx?idIdleLoop=<%=il.idIdleLoop%>">Cancel</a>
<% } %>


<br>

</form>

<br>
<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
<!--	TODO : Place Test information here -->
We can get to this screen by either editing an existing attract loop item or adding a new one.

If adding, the following general functionality is available:
<ul>
<li>Add Attract Loop Item - add item to list, and go to edit attract loop item mode</li>
<li>Cancel - abort adding item to list, go back to attract loop</li>
</ul>
If Editing
<ul>
<li>Update Attract Loop Item - update changes, continue editing</li>
<li>Edit Attract Loop - go back to attract loop</li>
</ul>

Depending on what the frame type of the attract loop item is, the following values can be submitted.
For all frame types, the loop item name can be changed...a non-empty string must be specified.
Here's the different values for the other frame types

<ul>
<li>Image - a full screen image to be displayed in the idle loop
<ul>
	<li>Show Time - length of time to show image, must be a positive integer</li>
	<li>Link - URL to go to if image is touched while in idle loop, optional</li>
</ul>
</li>
<li>Video - video clip to play in idle loop
<ul>
	<li>Start Time - offset in milliseconds of where to begin playing clip, optional</li>
	<li>End Time - offset in milliseconds of where to stop playing clip, optional</li>
</ul>
</li>
<li>Screen ML - screen ml page to render
<ul>
	<li>File link - view uploaded file</li>
	<li>Show Time - length of time to show image, must be a positive integer</li>
	<li>Query String - optional query string for URL</li>
</ul>
</li>
<li>Product Rec List - recommendation list to display
<ul>
	<li>Show Time - length of time to show each page in the rec list, must be a positive integer</li>
	<li>List Type - rec list type, base type should be product - list types create by rec list manager</li>
	<li>Media Type - media type of rec list</li>
	<li>Items Per Page - number of rec list items to be displayed per page</li>
</ul>
</li>
<li>Special Offer - not working, will require testing once promotions have been added</li>
<li>Product - view a specific product
<ul>
	<li>Show Time - length of time to show page, must be a positive integer</li>
	<li>Item Id - item id of product to show, need to create a link to a view items page...i'll add that in soon.</li>
</ul>
</li>

</ul>

</div>
</div>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
