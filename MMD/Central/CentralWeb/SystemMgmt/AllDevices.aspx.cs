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

namespace MMD.Central.CentralWeb.SystemMgmt
{
	/// <summary>
	/// Summary description for AllDevices.
	/// </summary>
	public class AllDevices : System.Web.UI.Page
	{
		CSWContext _ctx;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
		}


		public void DoListView()
		{
			ListView lv = new ListView(_ctx, "AllDevices","deviceconfiguration", null, null, _ctx.LoadListViewFromSession , false);

			lv.AddColumn(new FieldColumn(lv, "idStoreConcept", "Concept"));
			lv.AddColumn(new FieldColumn(lv, "idStore", "Store"));
			lv.AddColumn(new FieldColumn(lv, "idDevice", "Device Id"));
			lv.AddColumn(new FieldLinkColumn(lv, "sMacAddress", "MAC<br>Address","EditDevice.aspx?idStore=%idStore%&idDevice=%idDevice%"));
			lv.AddColumn(new FieldColumn(lv, "idLocationMode", "Location<br>Mode"));
			lv.AddColumn(new FieldColumn(lv, "idIdleLoop", "Attract<br>Loop"));
			lv.AddColumn(new FieldColumn(lv, "cSessionLength", "Usage<br>Today"));
			lv.AddColumn(new FieldColumn(lv, "cSessions", "# Sessions"));
			lv.AddColumn(new FieldColumn(lv, "cScans", "#Scans"));
			lv.AddColumn(new FieldColumn(lv, "dtLastUpdate", "Last Update<br>To Central"));

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
