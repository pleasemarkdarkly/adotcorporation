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
using MMD.Central.CentralLib.ListView;

namespace MMD.Central.CentralWeb.Reports
{
	/// <summary>
	/// Summary description for SessionCountByTimeOfDay.
	/// </summary>
	public class SessionCountByTimeOfDay : System.Web.UI.Page
	{
		CSWContext _ctx;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
		}

		public void DoListView()
		{
			ListView lv = new ListView(_ctx, "SessionCountByTime","rpt_session_count_by_time_of_day", null, null, _ctx.LoadListViewFromSession , false);
			lv.StylePrefix = "RPT";

			lv.AddColumn(new FieldColumn(lv, "idStoreConcept", "Concept"));
			lv.AddColumn(new FieldColumn(lv, "idStore", "Store"));
			lv.AddColumn(new FieldColumn(lv, "idLocationMode", "Location Mode"));
			lv.AddColumn(new FieldColumn(lv, "idDevice", "Device"));
			lv.AddColumn(new FieldColumn(lv, "morning", "10am-1pm"));
			lv.AddColumn(new FieldColumn(lv, "lunch", "1pm-3pm"));
			lv.AddColumn(new FieldColumn(lv, "afternoon", "3pm-5pm"));
			lv.AddColumn(new FieldColumn(lv, "evening", "5pm-7pm"));
			lv.AddColumn(new FieldColumn(lv, "night", "7pm-9pm"));
									
			lv.AddColumn(new FieldColumn(lv, "total", "Total"));
			
			lv.ProcessPageCommand();
			lv.DoQuery();
			lv.SaveToSession();

			lv.EmitNavStart();
			lv.EmitLeftNav();
			lv.EmitRightNav();
			lv.EmitNavEnd();

			lv.EmitViewStart();
			lv.EmitHeaders();
			
			lv.EmitRows();
			lv.EmitViewEnd();
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
