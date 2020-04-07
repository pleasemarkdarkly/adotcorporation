<%@ Page language="c#" Codebehind="ManageSchedule.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Common.ManageSchedule" %>
<%@ Import NameSpace="System.Data" %>
<!-- Begin Manage Schedule -->
<div class="SectionTitle">Schedule Items</div>
<% if(Rows.Count > 0) { %>
<table >
<tr><th>Priority</th><th>Start Date</th><th>End Date</th><th>Days of Week</th><th>Start Time</th><th>End Time</th><th>&nbsp;</th><th>&nbsp;</th><th>&nbsp;</th></tr>
<% foreach(DataRow row in Rows) { 
	int i = (int)row["idSchedule"];
%>

<tr>
	<td><%=row["iPriority"]%></td>
	<td><%=Convert.ToDateTime(row["dtBegin"]).ToShortDateString()%></td>
	<td><%=Convert.ToDateTime(row["dtEnd"]).ToShortDateString()%></td>
	<td><%=DaysOfWeek(row)%></td>
	<td><%=Convert.ToDateTime(row["dtTimeStart"]).ToString("t")%></td>
	<td><%=Convert.ToDateTime(row["dtTimeEnd"]).ToString("t")%></td>
<%	if(ShowEditLinks) { %>	
	<td>
		<a class="ShortCut" href="javascript:FormAlert('frmEdit_<%=i%>','Update Schedule', 'Enter the settings for this schedule item and click Update to submit changes.', 'divEdit_<%=i%>', 42, 15)">Edit</a>&nbsp;&nbsp;
	</td>
	<td>
		<a class="ShortCut" href="javascript:ConfirmLink('Are you sure you wish to delete this schedule item?','<%=InPage%>&cmd=ScheduleDelete&idSchedule=<%=row["idSchedule"]%>')">Delete</a>&nbsp;&nbsp;
	</td>
	<td>

<div id="divEdit_<%=i%>" style="display: none">
	<form id="frmEdit_<%=i%>" method=post action="<%=InPage%>">
		<input type="hidden" name="cmd" value="ScheduleUpdate">
		<input type="hidden" name="idSchedule" value="<%=i%>">
		<table class="StdTable" cellspacing=0>
		<tr class="StdRow">
			<td class="StdHdr">Priority</td>
			<td class="StdHdr">Begin Date</td>
			<td class="StdHdr">End Date</td>
			<td class="StdHdr">Days of Week</td>
			<td class="StdHdr">Start Time</td>
			<td class="StdHdr">End Time</td>
		</tr>
		<tr class="StdRow">
			<td class="StdCell" valign=top>
				<input type="text" size=5 maxlength=5 name="iPriority" value="<%=row["iPriority"]%>">
				<input type="hidden" name="label_iPriority" value="Priority">
				<input type="hidden" name="required_iPriority" value="1">
				<input type="hidden" name="valid_iPriority" value="nonnegLong">
			</td>
			<td class="StdCell" valign=top>
				<input type="text" size=10 maxlength=15 name="dtBegin" value="<%=Convert.ToDateTime(row["dtBegin"]).ToShortDateString()%>">
				<input type="hidden" name="label_dtBegin" value="Begin Date">
				<input type="hidden" name="required_dtBegin" value="1">
				<input type="hidden" name="valid_dtBegin" value="dateonly">
			</td>
			<td class="StdCell" valign=top>
				<input type="text" size=10 maxlength=15 name="dtEnd" value="<%=Convert.ToDateTime(row["dtEnd"]).ToShortDateString()%>">
				<input type="hidden" name="label_dtEnd" value="End Date">
				<input type="hidden" name="required_dtEnd" value="1">
				<input type="hidden" name="valid_dtEnd" value="dateonly">
			</td>			
			<td class="StdCell" valign=top>
				<table>
				<tr><td valign=top>
				<%=CheckBox(1,"Sunday",row)%><br>
				<%=CheckBox(2,"Monday",row)%><br>
				<%=CheckBox(3,"Tuesday",row)%><br>
				<%=CheckBox(4,"Wednesday",row)%>				
				</td>
				<td valign=top>
				<%=CheckBox(5,"Thursday",row)%><br>
				<%=CheckBox(6,"Friday",row)%><br>
				<%=CheckBox(7,"Saturday",row)%>
				</td></tr>
				</table>				
			</td>
			<td class="StdCell" valign=top>
				<input type="text" size=10 maxlength=15 name="dtTimeStart" value="<%=Convert.ToDateTime(row["dtTimeStart"]).ToString("t")%>">
				<input type="hidden" name="label_dtTimeStart" value="Start Time">
				<input type="hidden" name="required_dtTimeStart" value="1">
				<input type="hidden" name="valid_dtTimeStart" value="timeonly">
			</td>			
			<td class="StdCell" valign=top>
				<input type="text" size=10 maxlength=15 name="dtTimeEnd" value="<%=Convert.ToDateTime(row["dtTimeEnd"]).ToString("t")%>">
				<input type="hidden" name="label_dtTimeEnd" value="End Time">
				<input type="hidden" name="required_dtTimeEnd" value="1">
				<input type="hidden" name="valid_dtTimeEnd" value="timeonly">
			</td>			
		</tr>
		<tr>
			<td colspan=6 class="StdCell" align=center>
				<a class="ShortCut" href="#" onclick="SubmitAlertForm('frmEdit_<%=i%>');">Update</a>
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				<a class="ShortCut" href="#" onclick="DoClose(0)">Cancel</a>
			</td>
		</tr>
		</table>
	</form>
</div>
	</td>
<%		}  //ShowEditLinks %>
</tr>
<% } //foreach %>
</table>
<% } else { %>
<div style="padding-left: 10mm"><i>No Schedule Items</i></div>
<% } %>
<br>

	<a class="ShortCut" href="javascript:FormAlert('frmScheduleAdd','Add Schedule', 'Enter the settings for this schedule item and click Add to submit changes.', 'divScheduleAdd', 42, 15)">Add a new Schedule Item</a>

<div id="divScheduleAdd" style="display: none">
	<form id="frmScheduleAdd" method=post action="<%=InPage%>">
		<input type="hidden" name="cmd" value="ScheduleAdd">

		<table class="StdTable" cellspacing=0>
		<tr class="StdRow">
			<td class="StdHdr">Priority</td>
			<td class="StdHdr">Begin Date</td>
			<td class="StdHdr">End Date</td>
			<td class="StdHdr">Days of Week</td>
			<td class="StdHdr">Start Time</td>
			<td class="StdHdr">End Time</td>
		</tr>
		<tr class="StdRow">
			<td class="StdCell">
				<input type="text" size=5 maxlength=5 name="iPriority" value="">
				<input type="hidden" name="label_iPriority" value="Priority">
				<input type="hidden" name="required_iPriority" value="1">
				<input type="hidden" name="valid_iPriority" value="nonnegLong">
			</td>
			<td class="StdCell">
				<input type="text" size=10 maxlength=15 name="dtBegin" value="">
				<input type="hidden" name="label_dtBegin" value="Begin Date">
				<input type="hidden" name="required_dtBegin" value="1">
				<input type="hidden" name="valid_dtBegin" value="dateonly">
			</td>
			<td class="StdCell">
				<input type="text" size=10 maxlength=15 name="dtEnd" value="">
				<input type="hidden" name="label_dtEnd" value="End Date">
				<input type="hidden" name="required_dtEnd" value="1">
				<input type="hidden" name="valid_dtEnd" value="dateonly">
			</td>			
			<td class="StdCell">
				<table>
				<tr><td valign=top>
				<%=CheckBox(1,"Sunday",null)%><br>
				<%=CheckBox(2,"Monday",null)%><br>
				<%=CheckBox(3,"Tuesday",null)%><br>
				<%=CheckBox(4,"Wednesday",null)%>				
				</td>
				<td valign=top>
				<%=CheckBox(5,"Thursday",null)%><br>
				<%=CheckBox(6,"Friday",null)%><br>
				<%=CheckBox(7,"Saturday",null)%>
				</td></tr>
				</table>				
			</td>
			<td class="StdCell">
				<input type="text" size=10 maxlength=15 name="dtTimeStart" value="">
				<input type="hidden" name="label_dtTimeStart" value="Start Time">
				<input type="hidden" name="required_dtTimeStart" value="1">
				<input type="hidden" name="valid_dtTimeStart" value="timeonly">
			</td>			
			<td class="StdCell">
				<input type="text" size=10 maxlength=15 name="dtTimeEnd" value="">
				<input type="hidden" name="label_dtTimeEnd" value="End Time">
				<input type="hidden" name="required_dtTimeEnd" value="1">
				<input type="hidden" name="valid_dtTimeEnd" value="timeonly">
			</td>			
		</tr>
		<tr>
			<td colspan=6 class="StdCell" align=center>
				<a class="ShortCut" href="#" onclick="SubmitAlertForm('frmScheduleAdd');">Add</a>
				&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
				<a class="ShortCut" href="#" onclick="DoClose(0)">Cancel</a>
			</td>
		</tr>
		</table>
	</form>
</div>


<!-- End Manage Schedule -->
