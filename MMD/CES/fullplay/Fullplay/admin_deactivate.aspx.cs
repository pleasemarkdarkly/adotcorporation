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

using MMD.Store.StoreLib;


namespace Fullplay
{
	/// <summary>
	/// Summary description for admin_deactivate.
	/// </summary>
	public class admin_deactivate : System.Web.UI.Page
	{
		private MMDStyle _Style;
		public MMDStyle Style { get { return _Style; } }
		
		private void Page_Load(object sender, System.EventArgs e)
		{
			WebContext ctx = new WebContext(this);
			_Style = new MMDStyle(ctx);

			DeviceConfig MMDConfig;
			int mmdErr;
			int lLocationID;

			lLocationID = ctx.LocationID;
			MMDConfig = new DeviceConfig(ctx);
			mmdErr = MMDConfig.DeactivateLocation(lLocationID);
			
			ctx.WriteXmlHeader();
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
