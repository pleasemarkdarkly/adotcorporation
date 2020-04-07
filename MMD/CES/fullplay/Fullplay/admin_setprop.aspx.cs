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
	/// Summary description for admin_setprop.
	/// </summary>
	public class admin_setprop : System.Web.UI.Page
	{
		private void Page_Load(object sender, System.EventArgs e)
		{
		
			// Put user code to initialize the page here
			int mmdErr;
			DeviceConfig MMDConfig;
			int lPropID;
			string sValue;
			
			lPropID = int.Parse(Request.QueryString["PropID"]);
			sValue = Request.QueryString["Values"];
			WebContext ctx = new WebContext(this);
			
			MMDConfig = new DeviceConfig(ctx);

			mmdErr = MMDConfig.SetPropertyValue(ctx.LocationID ,lPropID,sValue.Replace("," , "|"));
			
			switch (mmdErr) 
			{
				case 0:
					break;
				default:
					ctx.ErrorMessage(mmdErr, "no", "no", ctx.LocationID );
					break;
			}
			
			Server.Transfer("admin_event.aspx");
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

