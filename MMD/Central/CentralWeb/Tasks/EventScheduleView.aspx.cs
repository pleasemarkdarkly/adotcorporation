using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Web;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
using MMD.Central.CentralLib;
using MMD.Central.CentralLib.Data;
using MMD.Central.CentralLib.ListView;
using MMD.Central.CentralLib.XmlObjects;

namespace MMD.Central.CentralWeb.Tasks
{
	/// <summary>
	/// Summary description for EventScheduleView.
	/// </summary>
	public class EventScheduleView : System.Web.UI.Page
	{
		CSWContext _ctx;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
		}

		public string DateOfNextRunCallback(DataRow row, ColumnBase df, int iRec)
		{
			DateTime dtNext = DateTime.Now;
			DateTime dtLast = Convert.ToDateTime(row["dtLastRun"]);

			MMDEventScheduleType byType = (MMDEventScheduleType)Convert.ToInt32(row["byType"]);
			if(byType == MMDEventScheduleType.PeriodicInMinutes)
			{
				dtNext = dtLast.AddMinutes(Convert.ToInt32(row["iPeriodMinutes"]));
			}
			else if(byType == MMDEventScheduleType.Daily)
			{
				// pull out the days of the week this is scheduled to run
				bool bNextWeek = true;
				byte byDOW = (byte)row["bfDaysOfWeek"];
				TimeSpan tsTimeOfDay = Convert.ToDateTime(row["dtTimeOfDay"]).TimeOfDay;
				TimeSpan tsTimeDiff = tsTimeOfDay - dtNext.TimeOfDay;

				// go through this week and see if there's a day that is used
				double dDays = 0;
				if (dtNext.TimeOfDay > tsTimeOfDay)
					dDays = 1;

				for(byte i=(byte)((double)dtNext.DayOfWeek + dDays);i<7 && bNextWeek;i++,dDays++)
					if(Convert.ToBoolean(byDOW & (1 << i)))
					{
						dtNext = dtNext.AddDays(dDays);
						dtNext = dtNext.Add(tsTimeDiff);
						bNextWeek = false;
					}

				if(bNextWeek)
				{
					for(byte i=0;i<7;i++,dDays++)
						if(Convert.ToBoolean(byDOW & (1 << i)))
						{
							dtNext = dtNext.AddDays(dDays);
							dtNext = dtNext.Add(tsTimeDiff);
							bNextWeek = false;
						}
				}

				if(bNextWeek)
					return "???";
			}
			else if(byType == MMDEventScheduleType.RunOnce)
			{
				return Convert.ToDateTime(row["dtRunOnce"]).ToString();
			}

			return dtNext.ToString();
		}

		public string DaysOfWeek(DataRow row)
		{
			string[] arDOW = { "Su", "M", "Tu", "W", "Th", "F", "Sa" };
			bool bFirst = true;
			byte byDOW = (byte)row["bfDaysOfWeek"];
			string s = "";
			
			if(byDOW == 0)
				s = "Never";
			else if(byDOW == 127 || byDOW == 255)
				s = "Daily";
			else
				for(byte i=0;i<7;i++)
				{
					if(Convert.ToBoolean(byDOW & (1 << i)))
					{
						if(bFirst) bFirst = false;
						else s+= ",";
						s += arDOW[i];
					}
				}
			
			return s;
		}

		public string ScheduleDescriptionCallback(DataRow row, ColumnBase df, int iRec)
		{
			MMDEventScheduleType byType = (MMDEventScheduleType)Convert.ToInt32(row["byType"]);
			if(byType == MMDEventScheduleType.PeriodicInMinutes)
			{
				int iMinutes = Convert.ToInt32(row["iPeriodMinutes"]);
				if(iMinutes == 0)
					return "Run Once";
				else if(iMinutes == 1)
					return "Every Minute";
				else if(iMinutes < 60)
					return "Every " + iMinutes + " Minutes";
				else if(iMinutes == 60)
					return "Every Hour";
				else if(iMinutes == 61)
					return "Every 1 Hour and 1 Minute";
				else if(iMinutes < 120)
					return "Every 1 Hour and " + iMinutes%60 + " Minutes";
				else if(iMinutes%60 == 0)
					return "Every " + iMinutes/60 + " Hours";
				else if(iMinutes%60 == 1)
					return "Every " + iMinutes/60 + " Hours and 1 Minute";
				else
					return "Every " + iMinutes/60 + " Hours and " + iMinutes%60 + " Minutes";
			}
			else if(byType == MMDEventScheduleType.Daily)
			{
				return DaysOfWeek(row) + " At " + Convert.ToDateTime(row["dtTimeOfDay"]).TimeOfDay.ToString();
			}
			else if(byType == MMDEventScheduleType.RunOnce)
			{
				return "Once At " + Convert.ToDateTime(row["dtRunOnce"]).ToString();
			}
			else
				return "";
		}

		public string EventNameCallback(DataRow row, ColumnBase df, int iRec)
		{
			return "/CentralWeb/Tasks/EventView.aspx?idEventSchedule=" + row["idEventSchedule"];
		}

		public string RunNowCallback(DataRow row, ColumnBase df, int iRec)
		{
			if(Convert.ToBoolean(row["bRunNow"]))
				return "Running";
			else
				return "<a class=\"NavLink\" href=\"javascript:"+"ConfirmLink('Are you sure you want to run this event now?','/CentralWeb/Tasks/EventScheduleview.aspx?cmd=EventScheduleRunNow&idEventSchedule=" + row["idEventSchedule"] + "')\">Run Now</a>";
		}

		public void DoListView()
		{
			ListView lv = new ListView(_ctx, "EventScheduleView","event_schedule", "sDesc", "asc", false, false);
			lv.StylePrefix = "LV";

			lv.AddDataField("idEventSchedule");
			lv.AddDataField("byType");
			lv.AddDataField("iPeriodMinutes");
			lv.AddDataField("dtTimeOfDay");
			lv.AddDataField("bfDaysOfWeek");
			lv.AddDataField("dtRunOnce");
			lv.AddDataField("bRunNow");

			//lv.AddColumn(new FieldColumn(lv,"sDesc","Event Name"));
			ListViewCallback lvcbEventName = new ListViewCallback(this.EventNameCallback);
			lv.AddColumn(new FieldCallbackColumn(lv, "sDesc", "Event Name", lvcbEventName));

			ListViewCallback lvcbSched = new ListViewCallback(this.ScheduleDescriptionCallback);
			lv.AddColumn(new ControlCallbackColumn(lv,"Schedule Description", lvcbSched));

			lv.AddColumn(new FieldColumn(lv,"iPriority","Priority"));
			lv.AddColumn(new FieldColumn(lv,"byStatus","Status"));
			lv.AddColumn(new FieldColumn(lv,"dtLastRun","Date of Last Run"));
			
			ListViewCallback lvcbDtNext = new ListViewCallback(this.DateOfNextRunCallback);
			lv.AddColumn(new ControlCallbackColumn(lv,"Date Of Next Run", lvcbDtNext));

			ListViewCallback lvcbRunNow = new ListViewCallback(this.RunNowCallback);
			lv.AddColumn(new ControlCallbackColumn(lv, "Run Now", lvcbRunNow));

			lv.ProcessPageCommand();
			lv.SetPageRecs(true);
			lv.DoQuery();
			lv.SaveToSession();

			lv.EmitNavStart();
			lv.EmitLeftNav();
			lv.EmitRightNav();
			lv.EmitNavEnd();

			lv.EmitViewStart();
			lv.EmitHeaders();

			//lv.Write("<form>");
			//lv.Write("<table border=\"0\" width=\"100%\" cellpadding=\"0\" cellspacing=\"0\">");
			//lv.EmitColumnHeaders(false);
			lv.EmitRows();
			lv.EmitViewEnd();
			//lv.Write("</form>");
		}

		#region Web Form Designer generated code
		override protected void OnInit(EventArgs e)
		{
			//
			// CODEGEN: This call is required by the ASP.NET Web Form Designer.
			//
			InitializeComponent();
			base.OnInit(e);
		}
		
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{    
			this.Load += new System.EventHandler(this.Page_Load);
		}
		#endregion
	}
}
