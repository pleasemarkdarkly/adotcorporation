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

namespace MMD.Central.CentralWeb.Admin
{
	/// <summary>
	/// Summary description for GroupTypeView.
	/// </summary>
	public class GroupTypeView : System.Web.UI.Page
	{
		CSWContext _ctx;
		DataSet _dsGroupType;

		public CSWContext CSWContext { get { return _ctx; } }
		public DataTable GroupType { get { return _dsGroupType.Tables["GroupType"]; } }
		
		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );

			_dsGroupType = _ctx.Config.MainDatabase.execSQL_DS("SELECT * FROM tblCSW_GroupType","GroupType");
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
