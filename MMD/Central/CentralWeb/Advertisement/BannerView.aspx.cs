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
using MMD.Central.CentralLib.DataObjects;

namespace MMD.Central.CentralWeb.Advertisement
{
	/// <summary>
	/// Summary description for BannerView.
	/// </summary>
	public class BannerView : System.Web.UI.Page
	{
		CSWContext _ctx;
		Banner _banner;

		public CSWContext CSWContext { get { return _ctx; } }
		public Banner Banner { get { return _banner; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			int idBanner = _ctx.Params.SessionInt("idBanner");

			//scheduling command processed that added/updated/deleted a schedule for this item
			if(_ctx.Params["sched_out_updated"] != null)
			{
				_ctx.Config.MainDatabase.execSQL("update tblMMD_BannerStaging set byStatus = " + 
					(int)StagingStatus.Changed + " where idBanner = " + idBanner + 
					" and byStatus = " + (int)StagingStatus.Unchanged);
			}

			_banner = new Banner(_ctx, idBanner);
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
