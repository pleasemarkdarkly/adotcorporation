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
using System.IO;
using MMD.Central.CentralLib;
using MMD.Central.CentralLib.XmlObjects;
using MMD.Central.CentralLib.ServiceImpl;

namespace MMD.Central.CentralWeb.Advertisement
{
	/// <summary>
	/// Summary description for RegisterFile.
	/// </summary>
	public class RegisterFile : System.Web.UI.Page
	{
		CSWContext _ctx;
		MMDPostedFileType _fileType;

		public CSWContext CSWContext { get { return _ctx; } }
		public MMDPostedFileType Type { get { return _fileType; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			_fileType = (MMDPostedFileType)_ctx.Params.Int("iType",(int)MMDPostedFileType.IdleLoopElement);

			Advertising ad = new Advertising(_ctx);
			string sPath = _ctx.Config.GetLocalPath(_fileType,false);			
			string[] arFiles = Directory.GetFiles(sPath);
			MMDFile[] arRegFiles = ad.GetAllFiles();

			string[] arUnRegFiles = new string[0];

			for(int i=0;i<arFiles.Length;i++)
			{
				bool bRegistered = false;
				for(int j=0;j<arRegFiles.Length;j++)
					if(arRegFiles[j].FileType == _fileType && arRegFiles[j].sFileName == arFiles[i])
					{
						bRegistered = true;
						break;
					}
				
				if(!bRegistered)
					arUnRegFiles[arUnRegFiles.Length] = arFiles[i];
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
