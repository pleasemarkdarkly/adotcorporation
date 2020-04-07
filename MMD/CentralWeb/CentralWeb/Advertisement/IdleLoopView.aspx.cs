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
	/// Summary description for IdleLoopView.
	/// </summary>
	public class IdleLoopView : System.Web.UI.Page
	{
		CSWContext _ctx;
		IdleLoop _idleloop;

		public CSWContext CSWContext { get { return _ctx; } }
		public IdleLoop IdleLoop { get { return _idleloop; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			int idIdleLoop = _ctx.Params.SessionInt("idIdleLoop");

			//scheduling command processed that added/updated/deleted a schedule for this item
			if(_ctx.Params["sched_out_updated"] != null)
			{
				_ctx.Config.MainDatabase.execSQL("update tblMMD_IdleLoopStaging set byStatus = " + 
					(int)StagingStatus.Changed + " where idIdleLoop = " + idIdleLoop + 
					" and byStatus = " + (int)StagingStatus.Unchanged);
			}			
			
			_idleloop = new IdleLoop(_ctx,idIdleLoop);



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
