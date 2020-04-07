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
	/// Summary description for GroupView.
	/// </summary>
	public class GroupView : System.Web.UI.Page
	{
		CSWContext _ctx;
		DataSet _dsGroup;
		int _idGroupType;
		string _sGroupTypeName;

		public string GroupTypeName { get { return _sGroupTypeName; } }
		public int GroupType { get { return _idGroupType; } }
        public DataTable Group { get { return _dsGroup.Tables["Group"]; } }
		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			_idGroupType = Convert.ToInt32(_ctx.Params["idGroupType"]);
			_dsGroup = _ctx.Config.MainDatabase.execSQL_DS("SELECT * FROM tblCSW_Group WHERE idGroupType = " + GroupType,"Group");

			DataSet ds = _ctx.Config.MainDatabase.execSQL_DS("SELECT * FROM tblCSW_GroupType WHERE idGroupType = " + GroupType, "GroupType");
			_sGroupTypeName = (string)ds.Tables["GroupType"].Rows[0]["sTypeName"];
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
