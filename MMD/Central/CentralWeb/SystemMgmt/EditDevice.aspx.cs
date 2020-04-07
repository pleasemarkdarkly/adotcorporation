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
using MMD.Central.CentralLib.XmlObjects;

namespace MMD.Central.CentralWeb.SystemMgmt
{
	/// <summary>
	/// Summary description for EditDevice.
	/// </summary>
	public class EditDevice : System.Web.UI.Page
	{
		CSWContext _ctx;
		int _idDevice;
		int _idStore;
		Store _store;
		Device _device;
		StorePersister _sp;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			_idStore = _ctx.Params.Int("idStore",0);
			_idDevice = _ctx.Params.Int("idDevice",0);
			_sp = new StorePersister(_ctx,false);
			_device = _sp.GetDeviceById(_idStore,_idDevice);
			_store = _sp.GetStoreById(_device.idStore);
			
		}

		public Device device { get { return _device; } }
		public Store store { get { return _store; } }

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
