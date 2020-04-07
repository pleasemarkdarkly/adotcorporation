<%@ Page language="c#" Codebehind="BannerView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Advertisement.BannerView" %>
<% CSWContext.PageTitle = "Banner" + Banner._sName; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<a class="BreadCrumb" href="/CentralWeb/Advertisement/BannersView.aspx?idBannerType=<%=Banner._BannerType._idBannerType%>">Manage Banners - <%=Banner._BannerType._sName%></a><br>
<div class="PageTitle">Banner - <%=Banner._sName%></div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

<table>
<tr><td>
<table class="StdTable" cellpadding=0 cellspacing=0>
<tr class="StdRow"><td class="StdHdr" align=right>Name:</td><td class="StdCell"><%=Banner._sName%>&nbsp;</td></tr>
<tr class="StdRow">
	<td class="StdHdr" align=right>Image:</td>
	<td class="StdCell">
		<img src="<%=Banner._sURL%>" alt="<%=Banner._sName%>" width="<%=Banner._BannerType._iWidth%>" height="<%=Banner._BannerType._iHeight%>">
	</td>
</tr>	
<tr class="StdRow"><td class="StdHdr" align=right>Type:</td><td class="StdCell"><%=Banner._BannerType._sName%>&nbsp;&nbsp;&nbsp;(<%=Banner._BannerType._iWidth%>&nbsp;x&nbsp;<%=Banner._BannerType._iHeight%>)</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Link:</td><td class="StdCell"><%=Banner._sDeviceLink%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Created:</td><td class="StdCell"><%=Banner._dtCreated%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Created By:</td><td class="StdCell"><%=Banner._sCreatedBy%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Changed:</td><td class="StdCell"><%=Banner._dtChanged%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Changed By:</td><td class="StdCell"><%=Banner._sChangedBy%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Status:</td><td class="StdCell"><%=Banner._byStatus%>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" colspan=2 align=center><a class="ShortCut" href="#" onclick="idEdit.style.display='block';">Edit</a></td></tr>
</table>
</td>
<td align=center>
<div id="idEdit" style="display: none; border: solid 2px black">
<form name="frmUpdateBanner" method=post enctype="multipart/form-data" action="/CentralWeb/Advertisement/BannerView.aspx">
<input type="hidden" name="cmd" value="BannerUpdate">
<input type="hidden" name="idBanner" value="<%=Banner._idBanner%>">
<input type="hidden" name="idBannerType" value="<%=Banner._BannerType._idBannerType%>">

<div class="SectionTitle" >Update Banner</div>
<table class="StdTable"  cellspacing=0>
<tr class="StdRow"><td class="StdHdr" colspan=2>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Type</td><td class="StdCell"><%=Banner._BannerType._sName%></td></tr>
<tr class="StdRow">
	<td class="StdHdr" align=right>Name</td>
	<td class="StdCell">
		<input type="text" maxlength="100" size=15 name="sName" value="<%=Banner._sName%>">
		<input type="hidden" name="label_sName" value="Name">
		<input type="hidden" name="required_sName" value="1">
	</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>File</td>
			<td class="StdCell">
				<input type="file" size=25 name="file">
				<input type="hidden" name="label_file" value="File">
				<input type="hidden" name="required_file" value="1">
			</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Link</td><td class="StdCell"><input type="text" maxlength=500 size=25 name="sDeviceLink" value=""></td></tr>
<tr class="StdRow">
	<td class="StdCell" align="center" colspan="2">
	<a class="ShortCut" href="javascript:SubmitForm('frmUpdateBanner','')">Update</a>
	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
	<a class="ShortCut" href="#" onclick="idEdit.style.display='none';">Cancel</a></td>
	
</tr>
</table>
</form>
</div>
</td></tr>
</table>

<%
	CSWContext.Params["sched_in_reltable"] = "tblMMD_BannerScheduleStaging";
	CSWContext.Params["sched_in_reltablekey"] = "idBanner";
	CSWContext.Params["sched_in_keyvalue"] = Banner._idBanner.ToString();
	CSWContext.Params["sched_in_page"] = "/CentralWeb/Advertisement/BannerView.aspx?idBanner=" + Banner._idBanner;
	Server.Execute("/CentralWeb/Common/ManageSchedule.aspx");	
%>



<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>

