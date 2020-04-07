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
	/// Summary description for admin_prop.
	/// </summary>
	public class admin_prop : System.Web.UI.Page
	{
		public string ListMode { get { return _sListMode; } }
		public string Heading { get { return _sHeading; } }
		public string Description { get { return _sDescription; } }
		public MMDStyle Style { get { return _MMDStyle; } }

		public void Transform()
		{
			_xslt.TransformToResponse( _xmld, "xslt/admin_prop.xsl" );	
		}
		
		private string _sListMode;
		private string _sHeading;
		private string _sDescription;
		private MMDStyle _MMDStyle;
		private XmlDocument _xmld;
		private XSLTransform _xslt;

		private void Page_Load(object sender, System.EventArgs e)
		{
			// Put user code to initialize the page here

			DeviceConfig MMDConfig;
			DeviceState MMDDevState;
			int mmdErr;
			int lLocationID;
			LocationType eLocationType;
			string sLocationTypeName;
			int lPropID;
			string strProp;

			lLocationID = int.Parse(Request.QueryString["LocationID"]);
			lPropID = int.Parse(Request.QueryString["PropID"]);

			WebContext ctx = new WebContext(this);
			MMDConfig = new DeviceConfig(ctx);
			mmdErr = MMDConfig.GetPropertyValues(lLocationID,lPropID,out _xmld);
			//MMDConfig = null

			_MMDStyle = new MMDStyle( ctx );

			switch (mmdErr) 
			{
				case 0:
					break;
				default:
					ctx.ErrorMessage(mmdErr, "no", "no", lLocationID);
					break;
			}
			strProp = _xmld.SelectSingleNode("//mmd-properties/mmd-property").Attributes.GetNamedItem("name").Value;
			_sHeading = "Set " + strProp;
			if (lPropID == 42) 
			{
				MMDDevState = new DeviceState(ctx);
				mmdErr = MMDDevState.GetLocationType(lLocationID, out eLocationType, out sLocationTypeName);
				MMDDevState = null;
			
				_sDescription = "Set the media type by choosing from the list|below. Press Select when done.";
			
			} 
			else if ( lPropID == 43) 
			{
				MMDDevState = new DeviceState(ctx);
				mmdErr = MMDDevState.GetLocationType(lLocationID, out eLocationType, out sLocationTypeName);
				MMDDevState = null;
				
				_sDescription = "Set the media type by choosing from the list|below. Press Select when done.";
				
				
			} 
			else 
			{
				_sDescription = "Set the " + strProp + " type by choosing from the list|below. Once chosen, press the select button.";
			}

			if (_xmld.SelectSingleNode("//mmd-properties/mmd-property/@multiselect").Value == "True") 
			{
				_sListMode = "select=\"single\"";
			} 
			else 
			{
				_sListMode = "select=\"single\"";
			}


			_xslt = new XSLTransform( ctx );
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

