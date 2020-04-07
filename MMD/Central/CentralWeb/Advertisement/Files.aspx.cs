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
using MMD.Central.CentralLib.ListView;
using MMD.Central.CentralLib.XmlObjects;

namespace MMD.Central.CentralWeb.Advertisement
{
	/// <summary>
	/// Summary description for Files.
	/// </summary>
	public class Files : System.Web.UI.Page
	{
		CSWContext _ctx;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
		}

		public string FileNameCallback(DataRow row, ColumnBase df, int iRec)
		{
			df.Target = "_blank";
			MMDPostedFileType type = (MMDPostedFileType)Convert.ToInt32(row["iType"]);
			string sFileName = Convert.ToString(row["sFileName"]);
			switch(type)
			{
				case MMDPostedFileType.Banner : return _ctx.Config.BannerWebPath(false,sFileName);
				case MMDPostedFileType.IdleLoopElement : return _ctx.Config.IdleLoopWebPath(false,sFileName);
				case MMDPostedFileType.SpecialOffer : return _ctx.Config.OfferWebPath(false,sFileName);
			}
			return null;
		}

		public void DoListView()
		{
			ListView lv = new ListView(_ctx, "Files","ad_files", _ctx.LoadListViewFromSession , false);
		
			//ListViewCallback lvcbName = new ListViewCallback(this.FileNameCallback);
			//lv.AddColumn(new FieldCallbackColumn(lv, "sFileName", "File", lvcbName));

			lv.AddColumn(new FieldLinkColumn(lv, "sFileName", "File", "/CentralWeb/Advertisement/FileView.aspx?idFile=%idFile%"));

			lv.AddColumn(new FieldColumn(lv, "iType", "Type"));

			lv.AddColumn(new FieldColumn(lv, "cItems", "File<br>Assoc."));

			lv.AddColumn(new FieldColumn(lv, "sMIMEType", "MIME Type"));

			lv.AddColumn(new FieldColumn(lv, "dtChanged", "Last Updated"));
			lv.AddColumn(new FieldColumn(lv, "sChangedBy", "Last Updated By"));

			lv.AddColumn(new ShortcutColumn(lv, "Delete", "/CentralWeb/Advertisement/Files.aspx?cmd=FileDelete&idFile=%idFile%" ));
			
			lv.ProcessPageCommand();
			lv.DoQuery();
			lv.SaveToSession();

			lv.EmitNavStart();
			lv.EmitLeftNav();
			lv.EmitRightNav();
			lv.EmitNavEnd();

			lv.EmitViewStart();
			lv.EmitHeaders();
			
			lv.EmitRows();
			lv.EmitViewEnd();
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
