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


namespace MMD.Central.CentralWeb.Profiles
{
	/// <summary>
	/// Summary description for FileEdit.
	/// </summary>
	public class FileEdit : System.Web.UI.Page
	{
		CSWContext _ctx;
		MMDFilePersister _fp;
		MMDFile _file;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			_fp = new MMDFilePersister(_ctx,false,Location.Staging );
			_file = _fp.GetFileById(_ctx.Params.Int("idFile",0));
			
		}

		public MMDFile File { get { return _file; } }

		public string FileHref()
		{
			return _file.URL(_ctx);
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
