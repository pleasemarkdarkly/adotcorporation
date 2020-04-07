<%@ Page language="c#" Codebehind="Home.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Reports.Home" %>
<% CSWContext.PageTitle = "Reports"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>


<a class="NavLink" href="RecListUsageAllStores.aspx">Rec List Usage - All Stores</a><br>
<span class="SmallText">Popular selections from product recommendation lists.</span>
<br>&nbsp;<br>

<a class="NavLink" href="ViewReportAggregateProductStats.aspx">Aggregate Product Stats</a><br>
<span class="SmallText">Popular product scans and viewings per week.</span>
<br>&nbsp;<br>

<a class="NavLink" href="TopScannedProducts.aspx">Top Scanned Products</a>
<br>&nbsp;<br>

<a class="NavLink" href="TopViewedProducts.aspx">Top Viewed Products</a><br>
<span class="SmallText">Popular products whose content was viewed by IMS users.</span>
<br>&nbsp;<br>

<a class="NavLink" href="NewReleaseActivity.aspx">New Release Activity</a><br>
<span class="SmallText">Activity of most recently released products.</span>
<br>&nbsp;<br>

<a class="NavLink" href="NewReleaseDetail.aspx">New Release Detail</a><br>
<span class="SmallText">Detailed activity of most recently released products.</span>
<br>&nbsp;<br>

<a class="NavLink" href="DeviceUtilization.aspx">Device Utilization</a><br>
<span class="SmallText">IMS device utilization by users per store.</span>
<br>&nbsp;<br>

<a class="NavLink" href="SessionCountByTimeOfDay.aspx">Session Count By Time Of Day</a>
<br>&nbsp;<br>

<a class="NavLink" href="ShopperSuccess.aspx">Shopper Success / Interest</a><br>
<span class="SmallText">Qualitative assetment of customer sessions on IMS devices per store.</span>
<br>&nbsp;<br>

<a class="NavLink" href="ProductActivity.aspx">Product Activity</a><br>
<span class="SmallText">Average number of product scans and viewed content per media type per IMS device per store.</span>
<br>&nbsp;<br>

<p class="SmallText">
<b>Using Reports</b><br>
<ul>
<li>Click on column headers to sort by that column.  Successful clicks will toggle ascending, descending, or no sort.</li>
<li>Holding down <em>Control</em> while clicking a column header will add the column to the current sort.  You may sort by up to 3 columns at a time.</li>
<li>You can filter reports via the filter header.  To filter on one or more columns, specify your filter(s), then click <b>Filter</b>.  The different filter types are:
<ul>
	<li><em>Combo Boxes</em> - You may filter by one item in a given combo box.  To disable filtering on a column with a combo box, select the empty field in the combo box.</li>
	<li><em>Numbers</em> - You can either specify a number to only display records that match that particular number.  You may filter for numbers greater than or less than by prefixing the number filter with <span style="font-weight: bold">&gt;</span> or <b>&lt;</b>, respectively. You can also perform inclusive comparisons with <b>&gt;=</b> or <b>&lt;=</b>.</li>
	<li><em>Text</em> - For columns that display text, you can filter by specifying a substring.  By default the filter only matches substrings at the beginning of the text.  If you prefer to attempt to match the substring anywhere in the text, prefix the substring with <b>%</b>.</li>
</ul>
</li>
<li>By default, up to 25 records are displayed per page.  Click <b>Show All</b> to display all the records for a given report.</li>
<li>Click the <b>Excel</b> link to export the data to Excel.  The current filter settings will be applied.</li>
</ul>
</p>

<a class="NavLink" href="Home.aspx?cmd=SyncReports">Update Reports</a><br>
<span class="SmallText">Update reports from logged device usage.</span>
<br>&nbsp;<br>


<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>