<%@ Page language="c#" Codebehind="EditDevice.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.SystemMgmt.EditDevice" %>
<%@ Import Namespace="MMD.Central.CentralLib.XmlObjects" %>
<% CSWContext.PageTitle = "System Management &gt; Edit Device&nbsp;" + device.sMacAddress; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<form name="frmUpdateDevice" method="post" action="EditDevice.aspx">
<input type="hidden" name="cmd" value="DeviceUpdate">
<input type="hidden" name="idStore" value="<%=store.idStore%>">
<input type="hidden" name="idDevice" value="<%=device.idDevice%>">

<table cellpadding=1 cellspacing=0>
<tr>
<td class="SmallText"><b>Device ID</b></td>
<td class="SmallText"><%=device.idDevice%></td>
</tr>
<tr>
<td class="SmallText"><b>Store</b></td>
<td class="SmallText"><%=store.sName%>&nbsp;(<%=store.idStore%>)</td>
</tr>
<tr>
<td class="SmallText"><b>MAC Address</b></td>
<td class="SmallText"><%=device.sMacAddress%></td>
</tr>
<tr>
<td class="SmallText"><b>Calibrated</b></td>
<td class="SmallText"><%=CSWContext.Lookups.YesNo.GetValue(device.bIsCalibrated ? 1 : 0)%></td>
</tr>
<tr>
<td class="SmallText"><b>Location Mode</b></td>
<td>
<%=CSWContext.Lookups.LocationMode(store.idStoreConcept).GetHTMLComboBox("idLocationMode",device.idLocationMode)%>
<input type="hidden" name="label_idLocationMode" value="Location Mode">
<input type="hidden" name="required_idLocationMode" value="1">
</td>
</tr>
<tr>
<td class="SmallText"><b>Default Media Type</b></td>
<td>
<%=CSWContext.Lookups.MediaType.GetHTMLComboBox("idDefaultMediaType",NullableInt.GetValue(device.DefaultMediaType))%>
</td>
</tr>
<tr>
<td class="SmallText"><b>Default Genre</b></td>
<td>
<%=CSWContext.Lookups.Genre.GetHTMLComboBox("idDefaultGenre",NullableInt.GetValue(device.DefaultGenre))%>
</td>
</tr>
<tr>
<td class="SmallText"><b>Attract Loop</b></td>
<td><%=CSWContext.Lookups.AttractLoop(store.idStoreConcept).GetHTMLComboBox("idIdleLoop",device.idIdleLoop)%>
<input type="hidden" name="label_idIdleLoop" value="Attract Loop">
<input type="hidden" name="required_idIdleLoop" value="1">
</td>
</tr>
<tr>
<td class="SmallText"><b>Attract Loop Timeout</b></td>
<td class="SmallText">
<input type="text" name="cSecsUntilIdle" value="<%=device.cSecsUntilIdle%>">Seconds
<input type="hidden" name="label_cSecsUntilIdle" value="Attract Loop Timeout">
<input type="hidden" name="required_cSecsUntilIdle" value="1">
<input type="hidden" name="valid_cSecsUntilIdle" value="nonnegLong">
</td>
</tr>

</table>

</form>
<a class="NavLink" href="javascript:SubmitForm('frmUpdateDevice','')">Update Device</a><br>


<br>
<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
<!--	TODO : Place Test information here -->
</div>
</div>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>