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
using MMD.Central.CentralLib.Data;
using MMD.Central.CentralLib.ListView;

namespace MMD.Central.CentralWeb.Profiles
{
	/// <summary>
	/// RecListTypeAdd enables a crafty user of MMD's System Management Web Interface to create new Recommend List Types
	/// </summary>
	public class RecListTypeAdd : System.Web.UI.Page
	{
		CSWContext _ctx;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );

		}

		public string RecListBaseTypeComboBox
		{
			get 
			{
				LookupTable lt = new LookupTable(_ctx.Config.MainDatabase,"select idRecListBaseType, sName from tblMMD_RecommendListBaseType where bSystem = 0 order by sName");
				return lt.GetHTMLComboBox("idRecListBaseType",_ctx.Params["idRecListBaseType"],false,true);
			}
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
