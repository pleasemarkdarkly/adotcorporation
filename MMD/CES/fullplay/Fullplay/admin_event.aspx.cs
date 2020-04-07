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
using System.Xml;


using MMD.Store.StoreLib;


namespace Fullplay
{
	/// <summary>
	/// Summary description for admin_event.
	/// </summary>
	public class admin_event : System.Web.UI.Page
	{
		public MMDStyle Style { get { return _MMDStyle; } }
		public string History { get { return _sHistory; } }
		public string PropertySet { get { return _sPropertySet; } }
		public string ListMode { get { return _sListMode; } }
		public string Heading { get { return _sHeading; } }
		public string Description { get { return _sDescription; } }
		public string XML { get { return _sXML; } }
		public int EventID { get { return _lEventID; } }
        
		private MMDStyle _MMDStyle;
		private string _sHistory;
		private string _sPropertySet;
		private string _sListMode;
		private string _sHeading;
		private string _sDescription;
		private string _sXML;
		private int _lEventID;

		private void Page_Load(object sender, System.EventArgs e)
		{
			DeviceConfig MMDConfig;
			int mmdErr;
			int lLocationID;

			_lEventID = int.Parse(Request.QueryString["EventID"]);

			WebContext ctx = new WebContext(this);
			lLocationID = ctx.LocationID;

			MMDConfig = new DeviceConfig(ctx);
			_MMDStyle = new MMDStyle(ctx);

			switch (_lEventID) 
			{
				case 1:
					//Make Recommendations
					Server.Transfer("admin_staffpicks.aspx");
					break;
				case 2:
					//Create Featured List
					Server.Transfer("admin_endcap.aspx");
					break;
				case 3:
					_sHeading = "View Device Properties";
					_sDescription = "Hit Back to Continue| ";
					_sListMode = "";
					break;
				case 4:
					_sHeading = "Configure Device";
					_sDescription = "Select from the following list of|configuration options";
					_sListMode = "select=\"single\"";
					break;
				case 5:
					Server.Transfer("admin_setlocation.aspx");
					break;
				default:
					_sHeading = "Event " + _lEventID;
					_sDescription = "";
					_sListMode = "select=\"single\"";
					break;
			}

			XmlDocument xmld;
			mmdErr = MMDConfig.GetProperties(lLocationID,_lEventID,out xmld);

			switch (mmdErr) 
			{
				case 0:
					break;
				default:
					ctx.ErrorMessage(mmdErr, "no", "no", lLocationID);
					break;
			}

			XSLTransform xsl = new XSLTransform(ctx);
	
			if (Request.QueryString["PropID"] != null) 
			{

				xsl.SetParam("PropID",Request.QueryString["PropID"]);
				// todo: verify still needed, not sure what this does.
				_sHistory = "pop";  // we just set a property, so lets pop the admin_prop from the history
				
				//Check if we set a device property (default volume or seconds until idle)
				switch (Request.QueryString["PropID"].Trim())
				{
					case "44": // default volume
						_sPropertySet = "<set_property name=\"default_volume\" value=\"" + Request.QueryString["Values"] + "\"/>";
						break;
					case "45": // default brightness
						_sPropertySet = "<set_property name=\"default_brightness\" value=\"" + Request.QueryString["Values"] + "\"/>";
						break;
					case "32": // seconds until idle
						_sPropertySet = "<set_property name=\"default_idle_timeout\" value=\"" + Request.QueryString["Values"] + "\"/>";
						break;
					default:
						_sPropertySet = "";
						break;
				}
			} 
			else 
			{
				_sHistory = "add";
			}

			ctx.WriteXmlHeader();			
			_sXML = xsl.TransformToString( xmld, "xslt/admin_event.xsl");
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
