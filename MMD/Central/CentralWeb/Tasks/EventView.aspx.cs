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
	/// Summary description for EventView.
	/// </summary>
	public class EventView : System.Web.UI.Page
	{
		CSWContext _ctx;
		int _idEventSchedule;
		string _sDesc;
		byte _byType;
		int _idSchedule;
		int _iPeriodMinutes;
		DateTime _dtTimeOfDay;
		DateTime _dtRunOnce;
		bool _bRunNow;
		byte _bfDaysOfWeek;
		int _iPriority;
		byte _byStatus;
		DateTime _dtLastRun;
		DateTime _dtDelta;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			_idEventSchedule = _ctx.Params.SessionInt("idEventSchedule",0);

			// grab all of the pertinent data
			DataSet ds = _ctx.Config.MainDatabase.execSQL_DS("select * from tblMMD_EventSchedule where idEventSchedule = " + _idEventSchedule,"Query");
			DataRow row = ds.Tables[0].Rows[0];
			_sDesc = row["sDesc"].ToString();
			_byType = Convert.ToByte(row["byType"]);
            if(row["idSchedule"] != System.DBNull.Value)
				_idSchedule = Convert.ToInt32(row["idSchedule"]);
			else
				_idSchedule = 0;
			if(row["iPeriodMinutes"] != System.DBNull.Value)
				_iPeriodMinutes = Convert.ToInt32(row["iPeriodMinutes"]);
			else
				_iPeriodMinutes = 0;
			if(row["dtTimeOfDay"] != System.DBNull.Value)
				_dtTimeOfDay = Convert.ToDateTime(row["dtTimeOfDay"]);
			else
				_dtTimeOfDay = Convert.ToDateTime("1/1/1900 12:00:00 AM");
			if(row["dtRunOnce"] != System.DBNull.Value)
				_dtRunOnce = Convert.ToDateTime(row["dtRunOnce"]);
			else
				_dtRunOnce = Convert.ToDateTime("1/1/1900 12:00:00 AM");
			_bRunNow = Convert.ToBoolean(row["bRunNow"]);
			if(row["bfDaysOfWeek"] != System.DBNull.Value)
				_bfDaysOfWeek = Convert.ToByte(row["bfDaysOfWeek"]);
			else
				_bfDaysOfWeek = 0;
			_iPriority = Convert.ToInt32(row["iPriority"]);
			if(row["byStatus"] != System.DBNull.Value)
				_byStatus = Convert.ToByte(row["byStatus"]);
			else
				_byStatus = 0;
			if(row["dtLastRun"] != System.DBNull.Value)
				_dtLastRun = Convert.ToDateTime(row["dtLastRun"]);
			else
				_dtLastRun = Convert.ToDateTime("1/1/1900 12:00:00 AM");
			_dtDelta = Convert.ToDateTime(row["dtDelta"]);
		}

		public int idEventSchedule { get { return _idEventSchedule; } }
		public string sDesc { get { return _sDesc; } }
		public string sType { get { return _byType.ToString(); } }
		public int idSchedule { get { return _idSchedule; } }
		public int iPeriodMinutes { get { return _iPeriodMinutes; } }
		public string sTimeOfDay { get { return _dtTimeOfDay.TimeOfDay.ToString(); } }
		public string sRunOnceDateTime { get { return _dtRunOnce.ToString(); } }
		public string sRunNow { get { return _bRunNow.ToString(); } }
		public string sDaysOfWeek { get { return DaysOfWeek(_bfDaysOfWeek); } }
		public int iPriority { get { return _iPriority; } }
		public string sStatus { get { return _byStatus.ToString(); } }
		public string sLastRun { get { return _dtLastRun.ToString(); } }
		public string sDelta { get { return _dtDelta.ToString(); } }

		public string DateOfNextRun()
		{
			DateTime dtNext = DateTime.Now;
			DateTime dtLast = _dtLastRun;
	
			MMDEventScheduleType byType = (MMDEventScheduleType)_byType;
			if(byType == MMDEventScheduleType.PeriodicInMinutes)
			{
				dtNext = dtLast.AddMinutes(_iPeriodMinutes);
			}
			else if(byType == MMDEventScheduleType.Daily)
			{
				// pull out the days of the week this is scheduled to run
				bool bNextWeek = true;
				byte byDOW = _bfDaysOfWeek;
				TimeSpan tsTimeOfDay = _dtTimeOfDay.TimeOfDay;
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
				dtNext = _dtRunOnce;
			}

			return dtNext.ToString();
		}

		public string DaysOfWeek(byte byDOW)
		{
			string[] arDOW = { "Su", "M", "Tu", "W", "Th", "F", "Sa" };
			bool bFirst = true;
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

		public string DayOfWeekChecked(int iDay)
		{
			if(Convert.ToBoolean(_bfDaysOfWeek & (1 << iDay)))
                return "checked";
			else
				return "";
		}

		public string ScheduleDescription()
		{
			MMDEventScheduleType byType = (MMDEventScheduleType)_byType;
			if(byType == MMDEventScheduleType.PeriodicInMinutes)
			{
				int iMinutes = _iPeriodMinutes;
				if(iMinutes == 0)
					return "Run Manually";
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
				return DaysOfWeek(_bfDaysOfWeek) + " At " + _dtTimeOfDay.TimeOfDay.ToString();
			}
			else if(byType == MMDEventScheduleType.RunOnce)
			{
				return "Once At " + _dtRunOnce.ToString();
			}
			else
				return "";
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
