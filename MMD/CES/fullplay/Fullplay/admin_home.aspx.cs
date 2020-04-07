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
	/// Summary description for admin_home.
	/// </summary>
	public class admin_home : System.Web.UI.Page
	{
		
		private string _sXML;
		private string _sReboot;
		private bool _bSetLocation;
		private int _lNewLocationID;
		private int _lSecsUntilIdle;
		private int _lDefaultVolume;
		private bool _bAddRebootToList;
		private MMDStyle _MMDStyle;
		private string _sLanguageCode;
		private int _lLocationID;

		public MMDStyle Style { get { return _MMDStyle; } }
		public string Reboot { get { return _sReboot; } }
		public string XML { get { return _sXML; } }
		public bool IsSetLocation { get { return _bSetLocation; } } 
		public bool AddRebootToList { get { return _bAddRebootToList; } }
		public int NewLocationID { get { return _lNewLocationID; } }
		public int SecsUntilIdle { get { return _lSecsUntilIdle; } }
		public int DefaultVolume { get { return _lDefaultVolume; } }
		public string LanguageCode { get { return _sLanguageCode; } }
		public int LocationID { get { return _lLocationID; } } 

		private void Page_Load(object sender, System.EventArgs e)
		{
			// Put user code to initialize the page here

			DeviceConfig MMDDevConfig;
			int mmdErr;
			System.Xml.XmlDocument xmld;
			string sMacAddress;
			_bSetLocation = false;
			string sUPC;
			DeviceState MMDDevState;
			SessionType tSessionType;
			DateTime SessionStartTime;
			DeviceStatus MMDDeviceStatus;
			string sDefaultScreen;

			WebContext ctx = new WebContext (this);
			
			MMDDevState = new DeviceState(ctx);
			string sSetLocation = ctx.Request.QueryString.Get("SetLocation");

			if (sSetLocation == null)
				_lNewLocationID = 0;
			else
				_lNewLocationID = int.Parse(sSetLocation);
			_lLocationID= int.Parse(ctx.Request.QueryString.Get("LocationID"));
			mmdErr = MMDDevState.GetLanguage(_lLocationID, out _sLanguageCode);
			MMDDevConfig = new DeviceConfig(ctx);
				
 
			_sReboot = (string) ctx.Request.QueryString.Get("Reboot");

			_bAddRebootToList = false;
			
			if (_lLocationID != 0)
			{
				_bSetLocation = false;
				mmdErr = MMDDevState.GetSessionInfo(_lLocationID,out tSessionType,out SessionStartTime,out MMDDeviceStatus);
				if (mmdErr == 0 && tSessionType == SessionType.AdminFull)
					_bAddRebootToList = true;
			}
			if (_lNewLocationID != 0)
			{
				if (_lLocationID != 0)
				{
					sUPC = ctx.Config.AdminLevelFull;
					mmdErr = MMDDevConfig.ChangeLocationID(_lLocationID, _lNewLocationID);
				}
				else
				{
					sMacAddress = Request.QueryString.Get("mac");
					sUPC = Request.QueryString.Get("Scan");
					mmdErr = MMDDevConfig.SetLocationID(_lNewLocationID, sMacAddress, sUPC);
					_bAddRebootToList = true;
					if (mmdErr != 0)
						ctx.ErrorMessage (mmdErr, "no", "no", _lNewLocationID);
				}

				mmdErr = MMDDevState.AdminLogin( _lNewLocationID, sUPC );
				if(mmdErr != 0)
					ctx.ErrorMessage (mmdErr, "no", "no", _lNewLocationID);

				_bSetLocation = true;
				_lLocationID = _lNewLocationID;
			}

			_MMDStyle = new MMDStyle(ctx);
			//MMDDevState.GetSessionType( _lNewLocationID, out tSessionType);

			int lDefaultBrightness;
			int lIdleLoopID;
			int lDefaultScreenID;

			if (_bSetLocation)
			{
				mmdErr = MMDDevState.GetMMDSettings(_lLocationID,out _lDefaultVolume,out lDefaultBrightness);
				mmdErr = MMDDevState.GetIdleInfo(_lLocationID,out lIdleLoopID,out _lSecsUntilIdle,out lDefaultScreenID, out sDefaultScreen);
			}

			mmdErr = MMDDevConfig.GetAdminEvents(_lLocationID, out xmld);

			XSLTransform xslt = new XSLTransform(ctx);
			_sXML = xslt.TransformToString(xmld, "xslt/admin_home.xsl");
			
			if (mmdErr != 0)
			{
				ctx.ErrorMessage (mmdErr, "no", "no", _lLocationID);
			}

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
