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
using System.Data.OleDb;

namespace MMD.Central.CentralWeb.Marketing
{
	/// <summary>
	/// Summary description for OLAP.
	/// </summary>
	public class OLAP : System.Web.UI.Page
	{
		private void Page_Load(object sender, System.EventArgs e)
		{
			// Put user code to initialize the page here

			if(Request["post"] != null)
			{
				OleDbConnection conn = new OleDbConnection("Provider=msolap;Data Source=localhost;Initial Catalog=" + Request["Catalog"]);
				conn.Open();

				OleDbDataAdapter cmdMDX = new OleDbDataAdapter(Request["MDXQuery"],conn);
				DataSet ds = new DataSet();
				cmdMDX.Fill(ds, "MDXResults");
				Response.ContentType = "text/xml";

				ds.WriteXml(Response.OutputStream);

				Response.End();
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
