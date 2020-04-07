<%@ Page language="c#" Codebehind="EventView.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Tasks.EventView" %>
<% CSWContext.PageTitle = "Delta Package Propagation > View Event"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<%
	string sPeriodicStyle = "display: " + ((sType == "0") ? "block" : "none");
	string sDailyStyle = "display: " + ((sType == "1") ? "block" : "none");
	string sRunOnceStyle = "display: " + ((sType == "2") ? "block" : "none");
	string sPeriodicSelected = ((sType == "0") ? "selected" : "");
	string sDailySelected = ((sType == "1") ? "selected" : "");
	string sRunOnceSelected = ((sType == "2") ? "selected" : "");
%>

<script>

function SwitchTypeView()
{
	var ele = document.forms["frmEventScheduleEdit"].elements["byType"];
	var ele_periodic = document.getElementById("type_periodic");
	var ele_daily = document.getElementById("type_daily");
	var ele_daily2 = document.getElementById("type_daily2");
	var ele_runonce = document.getElementById("type_runonce");
	if(ele.value == 0)
	{
		ele_periodic.style.display = "block";
		ele_daily.style.display = "none";
		ele_daily2.style.display = "none";
		ele_runonce.style.display = "none";
	}
	else if(ele.value == 1)
	{
		ele_periodic.style.display = "none";
		ele_daily.style.display = "block";
		ele_daily2.style.display = "block";
		ele_runonce.style.display = "none";
	}
	else
	{
		ele_periodic.style.display = "none";
		ele_daily.style.display = "none";
		ele_daily2.style.display = "none";
		ele_runonce.style.display = "block";
	}
}

</script>


<form name="frmEventScheduleEdit" method="post" action="/CentralWeb/Tasks/EventView.aspx">
	<table class="StdTable" cellspacing="5">
		<tr class="StdRow">
			<td class="SmallText"><b>Event Name</b></td>
			<td class="SmallText" colspan="7">
				<input size="40" maxlength="100" name="sDesc" value="<%=sDesc%>">
				<input type="hidden" name="label_sDesc" value="Event Name">
				<input type="hidden" name="required_sDesc" value="1">
			</td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText"><b>Priority</b></td>
			<td class="SmallText" colspan="7">
				<input size="5" maxlength="3" name="iPriority" value="<%=iPriority%>">
				<input type="hidden" name="label_iPriority" value="Priority">
				<input type="hidden" name="required_iPriority" value="1">
			</td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText"><b>Status</b></td>
			<td class="SmallText" colspan="7"><%=CSWContext.Lookups.EventStatus.GetHTMLComboBox("byStatus",sStatus,false,true)%></td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText"><b>Date of Last Run</b></td>
			<td class="SmallText" colspan="7"><%=sLastRun%></td>
				<input type="hidden" name="sLastRun" value="<%=sLastRun%>">
		</tr>
		<tr class="StdRow">
			<td class="SmallText"><b>Date of Next Run</b></td>
			<td class="SmallText" colspan="7"><%=DateOfNextRun()%></td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText" colspan="10"><hr></td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText"><b>Event Schedule Description</b></td>
			<td class="SmallText" colspan="7"><%=ScheduleDescription()%></td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText"><b>Event Type</b></td>
			<td class="SmallText" colspan="7">
				<select name="byType" onchange="javascript:SwitchTypeView()">
					<option value="0" <%=sPeriodicSelected%>>Periodic</option>
					<option value="1" <%=sDailySelected%>>Daily</option>
					<option value="2" <%=sRunOnceSelected%>>Run Once</option>
				</select>
			</td>
		</tr>
		<tr class="StdRow" id="type_periodic" style="<%=sPeriodicStyle%>">
			<td class="SmallText"><b>Minutes Between Events</b></td>
			<td class="SmallText" colspan="7"><input size="5" maxlength="4" name="iPeriodMinutes" value="<%=iPeriodMinutes%>"></td>
		</tr>
		<tr class="StdRow" id="type_daily" style="<%=sDailyStyle%>">
			<td class="SmallText"><b>Time of Day</b></td>
			<td class="SmallText" colspan="7">
				<input size="8" maxlength="11" name="sTimeOfDay" value="<%=sTimeOfDay%>"> (00:00:00)
				<input type="hidden" name="label_sTimeOfDay" value="Time Of Day">
				<input type="hidden" name="valid_sTimeOfDay" value="timeonly">
			</td>
		</tr>
		<tr class="StdRow" id="type_daily2" style="<%=sDailyStyle%>">
			<td class="SmallText"><b>Days of the Week</b></td>
			<td class="SmallText">Su<input type="checkbox" value="1" name="dow_0" <%=DayOfWeekChecked(0)%>></td>
			<td class="SmallText">M<input type="checkbox" value="1" name="dow_1" <%=DayOfWeekChecked(1)%>></td>
			<td class="SmallText">Tu<input type="checkbox" value="1" name="dow_2" <%=DayOfWeekChecked(2)%>></td>
			<td class="SmallText">W<input type="checkbox" value="1" name="dow_3" <%=DayOfWeekChecked(3)%>></td>
			<td class="SmallText">Th<input type="checkbox" value="1" name="dow_4" <%=DayOfWeekChecked(4)%>></td>
			<td class="SmallText">F<input type="checkbox" value="1" name="dow_5" <%=DayOfWeekChecked(5)%>></td>
			<td class="SmallText">Sa<input type="checkbox" value="1" name="dow_6" <%=DayOfWeekChecked(6)%>></td>
		</tr>
		<tr class="StdRow" id="type_runonce" style="<%=sRunOnceStyle%>">
			<td class="SmallText"><b>Date and Time</b></td>
			<td class="SmallText" colspan="7">
				<input size="40" maxlength="40" name="sRunOnceDateTime" value="<%=sRunOnceDateTime%>">
				<input type="hidden" name="label_sRunOnceDateTime" value="Date and Time">
				<input type="hidden" name="valid_sRunOnceDateTime" value="date">
			</td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText" colspan="10"><hr></td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText" colspan="2">
				<b><a class="NavLink" href="javascript:SubmitForm('frmEventScheduleEdit','')">Submit Changes</a></b>
				<input type="hidden" name="cmd" value="EventScheduleEdit">
				<input type="hidden" name="idEventSchedule" value="<%=idEventSchedule%>">
				<input type="hidden" name="bRunNow" value="<%=sRunNow%>">
			</td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText" colspan="2">
				<b><a class="NavLink" href="/CentralWeb/Tasks/EventView.aspx?idEventSchedule=<%=idEventSchedule%>">Undo Changes</a></b>
			</td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText" colspan="2">
				<b><a class="NavLink" href="/CentralWeb/Tasks/EventCommandHistoryView.aspx?idEventSchedule=<%=idEventSchedule%>">View Event History</a></b>
			</td>
		</tr>
		<tr class="StdRow">
			<td class="SmallText" colspan="2">
				<b><a class="NavLink" href="/CentralWeb/Tasks/EventScheduleView.aspx">Back to View Event Scheduling</a></b>
			</td>
		</tr>
	</table>
</form>
<br>

<p>&nbsp;</p>
      
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