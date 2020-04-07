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
using MMD.Central.CentralLib.Utilities;
using MMD.Central.CentralLib.DataObjects;

namespace MMD.Central.CentralWeb.Advertisement
{
	/// <summary>
	/// Summary description for IdleLoopElementView.
	/// </summary>
	public class IdleLoopElementView : System.Web.UI.Page
	{
		CSWContext _ctx;
		IdleLoop _idleloop;
		IdleLoopElement _idleLoopElement;

		public CSWContext CSWContext { get { return _ctx; } }
		public IdleLoopElement Element { get { return _idleLoopElement; } }
		public IdleLoop Loop { get { return _idleloop; } }
		
		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			int idIdleLoopElement = _ctx.Params.Int("idIdleLoopElement",0);
			if(idIdleLoopElement == 0)
				Server.Transfer("/CentralWeb/Advertisement/IdleLoopElementEdit.aspx");

			_idleLoopElement = new IdleLoopElement(_ctx,idIdleLoopElement);
			_idleloop = new IdleLoop(_ctx,Element.idIdleLoop,true);
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
