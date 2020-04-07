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

namespace MMD.Central.CentralWeb.Admin
{
	/// <summary>
	/// Summary description for StoreConfig.
	/// </summary>
	public class StoreConfig : System.Web.UI.Page
	{
		CSWContext _ctx;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
		}

		public void DoListView()
		{
			ListView lv = new ListView(_ctx, "StoreConfig","deviceconfiguration", true, false);

			lv.AddColumn(new CheckboxColumn(lv, "Check", "test"));
			lv.AddColumn(new FieldColumn(lv, "idDevice", "Device Id"));
			lv.AddColumn(new FieldColumn(lv, "idLocationMode", "Location Mode"));
			lv.AddColumn(new FieldColumn(lv, "idIdleLoop", "Idle Loop"));
			lv.AddColumn(new FieldColumn(lv, "sLanguageDefault", "Default Language"));
			lv.AddColumn(new FieldColumn(lv, "sMacAddress", "MAC Address"));
			lv.AddColumn(new FieldColumn(lv, "iStatus", "Status"));
			lv.AddColumn(new FieldColumn(lv, "cSecsUntilIdle", "Secs Until Idle"));

			lv.ProcessPageCommand();
			lv.DoQuery();
			lv.SaveToSession();

			lv.EmitNavStart();
			lv.EmitLeftNav();
			lv.EmitRightNav();
			lv.EmitNavEnd();

			lv.EmitViewStart();
			lv.EmitHeaders();

			lv.Write("<form>");
			lv.EmitRows();
			lv.EmitViewEnd();
			lv.Write("</form>");

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
