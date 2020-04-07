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
	/// Summary description for Config.
	/// </summary>
	public class Config : System.Web.UI.Page
	{
		private int _lLocationID;
		private int _lDefaultVolume;
		private int _lDefaultBrightness;
		private int _lIdleLoopID;
		private int _lSecsUntilIdle;
		private int _lDefaultScreenID;
		private string _sDefaultScreenURL;
		private bool _bDeviceInitialized;
		private bool _bDeviceCalibrated;
		private Hashtable _htCalibrations;


		private void Page_Load(object sender, System.EventArgs e)
		{
			
		
			//System.Diagnostics.Trace.Write(dc.ConnectionString);

			_bDeviceInitialized = false;
			_bDeviceCalibrated = false;
			_sDefaultScreenURL = "";
			_htCalibrations = new Hashtable();

			WebContext ctx = new WebContext( this );


			//System.Diagnostics.Trace.Write(ctx.Config.ConnectionString,"TEST"); 
			//System.Diagnostics.Trace.Write(ctx.Config.ConnectionString);
			//System.Diagnostics.Trace.Write(ctx.Config.LoggingConnectionString);



			string sMACAddress;
			int mmdErr;
			SessionType eSessionType;
			DateTime dtSessionStartTime;
			DeviceStatus eDeviceStatus;

			sMACAddress = Request.QueryString["MAC"];

			DeviceConfig MMDDevConfig = new DeviceConfig(ctx);
			mmdErr = MMDDevConfig.GetLocationID(sMACAddress, out _lLocationID);

			mmdErr = MMDDevConfig.GetCalibrationFlag(_lLocationID, out _bDeviceCalibrated);
			
			if (_bDeviceCalibrated)
			{
				
				MMDDevConfig.GetCalibration(_lLocationID, out _htCalibrations);

			}
			
			


			DeviceState MMDDevState = new DeviceState(ctx);
			mmdErr = MMDDevState.GetSessionInfo(_lLocationID, out eSessionType, out dtSessionStartTime, out eDeviceStatus);
			if(eSessionType != SessionType.None)
				mmdErr = MMDDevState.TerminateSession(_lLocationID);

			if(mmdErr == 0)
			{
				_bDeviceInitialized = true;
				mmdErr = MMDDevState.GetMMDSettings(_lLocationID, out _lDefaultVolume, out _lDefaultBrightness);
				mmdErr = MMDDevState.GetIdleInfo(_lLocationID, out _lIdleLoopID, out _lSecsUntilIdle, out _lDefaultScreenID, out _sDefaultScreenURL);
				
			}

		}
		public int LocationID { get { return _lLocationID; } }
		public int DefaultVolume { get { return _lDefaultVolume; } }
		public int DefaultBrightness { get { return _lDefaultBrightness; } }
		public int IdleLoopID { get { return _lIdleLoopID; } }
		public int SecsUntilIdle { get { return _lSecsUntilIdle; } }
		public int DefaultScreenID { get { return _lDefaultScreenID; } }
		public string DefaultScreenURL { get { return _sDefaultScreenURL; } }
		public bool IsInitialized { get { return _bDeviceInitialized; } }
		public bool IsCalibrated {get {return _bDeviceCalibrated;}}
		public Hashtable Calibrations { get {return _htCalibrations; }}

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
