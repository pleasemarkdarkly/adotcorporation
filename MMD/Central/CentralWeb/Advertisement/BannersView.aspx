<%@ Page language="c#" Codebehind="BannersView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Advertisement.BannersView" %>
<% CSWContext.PageTitle = "Manage Banners - " + BannerTypeName; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<a class="BreadCrumb" href="/CentralWeb/Advertisement/BannerTypeView.aspx">Manage Banner Types</a><br>
<div class="PageTitle">Manage Banners -&nbsp;<%=BannerTypeName%></div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

Click on Banner Name to Manage the specific banner.<br>
Click on Delete link next to banner name to delete the specific banner.<br>
<p>

<% DoListView(); %>
<br>
<a class="ShortCut" href="/CentralWeb/Advertisement/BannersView.aspx?cmd=BannerSubmitType&idBannerType=<%=idBannerType%>">Submit All Changes</a>
<br>
<br>
<form name="frmAddBanner" method=post enctype="multipart/form-data" action="/CentralWeb/Advertisement/BannersView.aspx">
<input type="hidden" name="cmd" value="BannerAdd">
<input type="hidden" name="idBannerType" value="<%=idBannerType%>">

<div class="SectionTitle">Create New <%=BannerTypeName%> Banner</div>
<table class="StdTable" cellspacing=0>
<tr class="StdRow"><td class="StdHdr" colspan=2>&nbsp;</td></tr>
<tr class="StdRow"><td class="StdHdr" align=right>Type</td><td class="StdCell"><%=BannerTypeName%></td></tr>
<tr class="StdRow">
	<td class="StdHdr" align=right>Name</td>
	<td class="StdCell">
		<input type="text" maxlength="100" size=15 name="sName" value="">
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
<tr class="StdRow"><td class="StdCell" align=center colspan=2><a href="javascript:SubmitForm('frmAddBanner','')">Create</a></td></tr>
</table>
</form>

<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>

