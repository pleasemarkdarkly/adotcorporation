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
	/// Summary description for PackageView.
	/// </summary>
	public class PackageView : System.Web.UI.Page
	{
		CSWContext _ctx;
		int _idPackage;
		int _idSourceLevel;
		int _idDestLevel;
		int _idSendEvent;
		int _idRecvEvent;
		string _sDesc;
		string _sTransportSubdir;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			_idPackage = _ctx.Params.SessionInt("idPackage",0);

			// grab all of the package's data
			DataSet ds = _ctx.Config.MainDatabase.execSQL_DS("select * from tblMMD_Package where idPackage = " + _idPackage,"Query");
			DataRow row = ds.Tables[0].Rows[0];

			_sDesc = row["sDesc"].ToString();
			_idSourceLevel = Convert.ToInt32(row["idSourceLevel"]);
			_idDestLevel = Convert.ToInt32(row["idDestLevel"]);
			if(row["idSendEvent"] != System.DBNull.Value)
				_idSendEvent = Convert.ToInt32(row["idSendEvent"]);
			else
				_idSendEvent = 0;
			if(row["idRecvEvent"] != System.DBNull.Value)
				_idRecvEvent = Convert.ToInt32(row["idRecvEvent"]);
			else
				_idRecvEvent = 0;
			_sTransportSubdir = row["sTransportSubdir"].ToString();

		}

		public int idPackage { get { return _idPackage; } }
		public string sDesc { get { return _sDesc; } }
		public string sTrasnportSubdir { get { return _sTransportSubdir; } }
		public string sSourceLevel { get { return _idSourceLevel.ToString(); } }
		public string sDestLevel { get { return _idDestLevel.ToString(); } }

		public string SendEventComboBox
		{
			get
			{
				LookupTable lt = new LookupTable(_ctx.Config.MainDatabase,"select idEventSchedule, sDesc from tblMMD_EventSchedule order by sDesc");
				return lt.GetHTMLComboBox("idSendEvent",_ctx.Params["idEventSchedule"],false,true);
			}
		}

		public string RecvEventComboBox
		{
			get
			{
				LookupTable lt = new LookupTable(_ctx.Config.MainDatabase,"select idEventSchedule, sDesc from tblMMD_EventSchedule order by sDesc");
				return lt.GetHTMLComboBox("idRecvEvent",_ctx.Params["idEventSchedule"],false,true);
			}
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
