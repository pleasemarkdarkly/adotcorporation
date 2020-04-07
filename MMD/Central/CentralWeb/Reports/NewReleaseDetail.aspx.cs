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
	/// Summary description for NewReleaseDetail.
	/// </summary>
	public class NewReleaseDetail : System.Web.UI.Page
	{
		CSWContext _ctx;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
		}

		public void DoListView()
		{
			ListView lv = new ListView(_ctx, "NewReleaseDetail","rpt_new_release_detail", null, null, _ctx.LoadListViewFromSession , false);
			lv.StylePrefix = "RPT";

			lv.AddColumn(new FieldColumn(lv, "genre", "Genre/Platform"));
			lv.AddColumn(new FieldColumn(lv, "sTitle", "Product Title"));
			lv.AddColumn(new FieldColumn(lv, "sArtist", "Artist"));
			lv.AddColumn(new FieldColumn(lv, "sUPC", "UPC"));
			lv.AddColumn(new FieldColumn(lv, "cSampled", "Samples"));

			lv.AddColumn(new FieldColumn(lv, "dtRelease", "Release<br>Date"));
			lv.AddColumn(new FieldColumn(lv, "cScans", "Scans"));
			lv.AddColumn(new FieldColumn(lv, "cSelects", "Views"));


			
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
