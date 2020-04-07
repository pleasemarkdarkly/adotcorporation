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
	/// Summary description for EventCommandHistoryView.
	/// </summary>
	public class EventCommandHistoryView : System.Web.UI.Page
	{
		CSWContext _ctx;
		int _idEventSchedule;
		string _sDesc;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			_idEventSchedule = _ctx.Params.SessionInt("idEventSchedule",0);

			// grab all of the data
			DataSet ds = _ctx.Config.MainDatabase.execSQL_DS("select * from tblMMD_EventSchedule where idEventSchedule = " + _idEventSchedule,"Query");
			DataRow row = ds.Tables[0].Rows[0];
			_sDesc = row["sDesc"].ToString();
		}
		
		public int idEventSchedule { get { return _idEventSchedule; } }
		public string sDesc { get { return _sDesc; } }

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
