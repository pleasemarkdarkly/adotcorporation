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

namespace MMD.Central.CentralWeb.FeatureRequest
{
	/// <summary>
	/// Summary description for EditFeatureRequest.
	/// </summary>
	public class EditFeatureRequest : System.Web.UI.Page
	{
		CSWContext _ctx;
		FeatureRequestPersister _flp;
		MMD.Central.CentralLib.XmlObjects.FeatureRequest _fr;
		int _idFeatureRequest;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			_flp = new FeatureRequestPersister(_ctx,false);
			_idFeatureRequest = _ctx.Params.Int("idFeatureRequest",0);
			_fr = _flp.GetFeatureRequestById(_idFeatureRequest);

			if(_fr == null)
			{
				_fr = new MMD.Central.CentralLib.XmlObjects.FeatureRequest();
				_fr.sChangedBy = _ctx.UserName;
			}

		}
		
		public MMD.Central.CentralLib.XmlObjects.FeatureRequest fr { get { return _fr; } }

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
