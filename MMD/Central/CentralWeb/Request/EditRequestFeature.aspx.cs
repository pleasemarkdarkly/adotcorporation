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

namespace MMD.Central.CentralWeb.RequestFeature
{
	/// <summary>
	/// Summary description for EditFeatureRequest.
	/// </summary>
	public class EditRequestFeature : System.Web.UI.Page
	{
		CSWContext _ctx;
		RequestFeaturePersister _rfp;
		MMD.Central.CentralLib.XmlObjects.RequestFeature _rf;
		int _idRequestFeature;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			_rfp = new RequestFeaturePersister(_ctx,false);
			_idRequestFeature = _ctx.Params.Int("idRequestFeature",0);
			_rf = _rfp.GetRequestFeatureById(_idRequestFeature);

			if(_rf == null)
			{
				_rf = new MMD.Central.CentralLib.XmlObjects.RequestFeature();
				_rf.sChangedBy = _ctx.UserName;
			}

		}
		
		public MMD.Central.CentralLib.XmlObjects.RequestFeature rf { get { return _rf; } }

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
