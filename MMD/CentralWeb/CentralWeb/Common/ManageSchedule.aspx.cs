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
using MMD.Central.CentralLib.Utilities;

namespace MMD.Central.CentralWeb.Common
{
	/// <summary>
	/// Summary description for ManageSchedule.
	/// </summary>
	public class ManageSchedule : System.Web.UI.Page
	{
		CSWContext _ctx;
		string _sRelTable;
		string _sRelTableKey;
		string _sKeyValue;
		string _sPage;
		bool _bStaging = true;
		bool _bShowEditLinks = true;

		DataSet _ds;

		public CSWContext CSWContext { get { return _ctx; } }
		public string InPage { get { return _sPage; } }
		public DataRowCollection Rows { get { return _ds.Tables[0].Rows; } }
		public bool ShowEditLinks { get { return _bShowEditLinks; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );

			_sRelTable = _ctx.Params.SessionString("sched_in_reltable");
			_sRelTableKey = _ctx.Params.SessionString("sched_in_reltablekey");
			_sKeyValue = _ctx.Params.SessionString("sched_in_keyvalue");
			_sPage = _ctx.Params.SessionString("sched_in_page");
			_bShowEditLinks = _ctx.Params.Int("sched_in_showeditlinks",1) == 1;

			if(_sPage.IndexOf("?") < 0)
				_sPage += "?InSched=1";

			_bStaging = true;

			string sTable = (_bStaging) ? "tblMMD_ScheduleStaging" : "tblMMD_Schedule";
			//grab dataset
			string sSQL = "select S.* from " + sTable + " S JOIN " 
							+ _sRelTable + " J ON S.idSchedule = J.idSchedule" + 
							" WHERE J." + _sRelTableKey + " = " + _sKeyValue + 
							" ORDER BY S.iPriority DESC";

			_ds = _ctx.Config.MainDatabase.execSQL_DS(sSQL); 
		}

		public string CheckBox(int iDOW, string sLabel, DataRow row)
		{
			byte byDOW = (row != null) ? (byte)row["bfDayOfWeek"] : (byte)0;
			bool bChecked = Convert.ToBoolean(byDOW & (1 << iDOW));
			string sName = "dow_" + iDOW;
			string sId = (row != null) ? row["idSchedule"].ToString() : "new";
			sId += "_" + sName;
			return "<input type=\"checkbox\" value=\"1\" id=\"" + sId + "\" name=\"" + sName + "\" " + ( bChecked ? "CHECKED " : "" ) + ">"
					 + "<label for=\"" + sId + "\">" + sLabel + "</label>";
		}

		public string DaysOfWeek(DataRow row)
		{
			string[] arDOW = { "Su", "M", "Tu", "W", "Th", "F", "Sa" };
			bool bFirst = true;
			byte byDOW = (byte)row["bfDayOfWeek"];
			string s = "";
			
			if(byDOW == 254)
				s = "Every Day";
			else
				for(byte i=1;i<=7;i++)
				{
					if(Convert.ToBoolean(byDOW & (1 << i)))
					{
						if(bFirst) bFirst = false;
						else s+= ",";
						s += arDOW[i - 1];
					}
				}
			
			return s;
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
