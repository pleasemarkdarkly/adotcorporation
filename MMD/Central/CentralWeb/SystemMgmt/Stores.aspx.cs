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
	/// Summary description for Stores.
	/// </summary>
	public class Stores : System.Web.UI.Page
	{
		CSWContext _ctx;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
		}


		public void DoListView()
		{
			ListView lv = new ListView(_ctx, "Stores","storeconfiguration", null, null, _ctx.LoadListViewFromSession , false);

			lv.AddColumn(new FieldColumn(lv, "idStore", "Store"));
			lv.AddColumn(new FieldColumn(lv, "idStoreConcept", "Store Concept"));

			lv.AddColumn(new FieldLinkColumn(lv, "sName", "Name", "StoreDevices.aspx?idStore=%idStore%"));
			lv.AddColumn(new FieldColumn(lv, "sCity", "City"));
			lv.AddColumn(new FieldColumn(lv, "sState", "State"));
			//lv.AddColumn(new FieldColumn(lv, "sZip", "Zip Code"));
			lv.AddColumn(new FieldColumn(lv, "cDevices", "# Devices"));
			lv.AddColumn(new FieldColumn(lv, "cWorking", "# Working"));
			lv.AddColumn(new FieldColumn(lv, "sHostName", "Host Name"));

			lv.AddColumn(new ControlColumn(lv, "Last Package<br>Installed", ""));
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
