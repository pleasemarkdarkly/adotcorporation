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
	/// Summary description for admin_setlocation.
	/// </summary>
	public class admin_setlocation : System.Web.UI.Page
	{
		private MMDStyle _Style;
	
		private string _sMacAddress;
		private string _sUPC;
		private string _sXML;
		private int _iLocationID;
		private string _sDescription;

		

		public MMDStyle Style { get { return _Style; } }
		public string Description {get {return _sDescription;}}
		public string XML {get {return _sXML;}}

		private void Page_Load(object sender, System.EventArgs e)
		{
			WebContext ctx = new WebContext(this);
			_Style = new MMDStyle(ctx);

			XmlDocument xmldoc = new XmlDocument();

			SessionType eSessionType;
			DateTime dtSessionStartTime;
			DeviceStatus eDeviceStatus;
			
			_iLocationID = ctx.LocationID;

			_sUPC = ctx.Request.QueryString["Scan"];      
			_sMacAddress = ctx.Request.QueryString["MAC"];

			if( _sUPC != null && _sMacAddress != null)
			{
				//we're configuring an unconfigured device.
				_Style.SessionType = SessionType.AdminFull;
			}

			MMD.Store.StoreLib.DeviceConfig MMDDeviceConf = new MMD.Store.StoreLib.DeviceConfig(ctx);
			int mmdErr = MMDDeviceConf.GetAvailableLocations(_iLocationID, out xmldoc);

			switch (mmdErr)
			{
					//case 0: its all good yo!
				case 0:
					break;

				case 1610:
					if (!_iLocationID.Equals("")) 
					{
						MMD.Store.StoreLib.DeviceState MMDDevState = new MMD.Store.StoreLib.DeviceState(ctx);
						mmdErr = MMDDevState.GetSessionInfo(_iLocationID, out eSessionType, 
							out dtSessionStartTime, out eDeviceStatus);
						if (eSessionType == SessionType.Customer) 
							ctx.ErrorMessage(9001, "no","no",_iLocationID);
						else 
							ctx.ErrorMessage(9000, "no", "no", _iLocationID);
						break;
					}
					else {ctx.ErrorMessage(9000, "no", "no", _iLocationID);}
					break;

				default:
					ctx.ErrorMessage(mmdErr,"no","no",_iLocationID);
					break;

			}
			//added the null to make it behave like the current admin_setlocation.asp
			//basically, if no parameters are supplied the page will think you are at location 0
			if (_sMacAddress == "" || _sMacAddress == null) 
				_sDescription = 
					"Select the location ID for this device.|" +
					"The device's current location ID is " + _iLocationID + ".";
			else 
				_sDescription =
					"This device does not have an associated location ID.|" +
					"Select an ID for this device from the list below.";

			//specific xml created for page request
			XmlDocument xmld = new XmlDocument();
			XmlElement xmli = xmld.CreateElement("info");
			XmlElement xmlm = xmld.CreateElement("mac");
			xmlm.SetAttribute("address", "&mac="+_sMacAddress+"&Scan="+_sUPC);
				
			XmlNode xmlRoot = xmld.AppendChild(xmli);
			xmlRoot.AppendChild(xmlm);

			//adds the xml from xmldoc to xmld -- i.e. the xml 
			//from GetAvailableLocations and the xml from this page 
			foreach (XmlNode xnl in xmldoc.ChildNodes) 
			{
				XmlNode xn = xmld.ImportNode(xnl,true);
				xmlRoot.AppendChild(xn);
			}

			XSLTransform xslt = new XSLTransform(ctx);
			_sXML = xslt.TransformToString(xmld, "xslt/admin_setlocation.xsl");
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
