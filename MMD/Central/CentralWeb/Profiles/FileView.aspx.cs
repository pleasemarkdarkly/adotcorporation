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

namespace MMD.Central.CentralWeb.Profiles
{
	/// <summary>
	/// Summary description for FileView.
	/// </summary>
	public class FileView : System.Web.UI.Page
	{
		CSWContext _ctx;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
		}

		public string FileCallback(DataRow row, ColumnBase df, int iRec)
		{
			return _ctx.Config.GetWebPath(Convert.ToInt32(row["idStoreConcept"]),MMDPostedFileType.RecListAttribute,row["sFileName"].ToString());
		}

		public string EditCallback(DataRow row, ColumnBase df, int iRec)
		{
			if(Convert.ToInt32(row["cAssociations"]) == 0)
				return "/CentralWeb/Profiles/FileEdit.aspx?idFile=" + row["idFile"];
			else
				return "";
		}

		public string RemoveCallback(DataRow row, ColumnBase df, int iRec)
		{
			if(Convert.ToInt32(row["cAssociations"]) == 0)
				return "/CentralWeb/Profiles/FileView.aspx?cmd=FileRemove&idFile=" + row["idFile"];
			else
				return "";
		}

		public void DoListView()
		{
			// are these paramaters just names, or do they actually point to something?
			
			ListView lv = new ListView(_ctx, "FileView","registered_files", false, false);

			lv.AddColumn(new FieldColumn(lv, "idStoreConcept", "Concept"));
			lv.AddColumn(new FieldColumn(lv, "sName", "Loop Item Name"));
			//lv.AddColumn(new FieldColumn(lv, "sMIMEType", "MIME Type"));
			lv.AddColumn(new FieldColumn(lv, "iFileMediaType", "Type"));
			ListViewCallback lvcbFile = new ListViewCallback(this.FileCallback);
			lv.AddColumn(new FieldCallbackColumn(lv, "sFileName", "File", lvcbFile )).Target = "_blank";
			lv.AddColumn(new FieldColumn(lv,"cAssociations", "Assoc."));
			lv.AddColumn(new FieldColumn(lv, "dtChanged", "Change Date"));
			lv.AddColumn(new FieldColumn(lv, "sChangedBy", "Changed By"));

			ListViewCallback lvcbEdit = new ListViewCallback(this.EditCallback);
			lv.AddColumn(new ShortcutCallbackColumn(lv, "Edit", lvcbEdit));

			ListViewCallback lvcbRemove = new ListViewCallback(this.RemoveCallback);
			lv.AddColumn(new ShortcutCallbackColumn(lv, "Remove", lvcbRemove));
			
			lv.AddDataField("iType");
			lv.AddBaseFilter("iType","eq",((int)MMDPostedFileType.RecListAttribute).ToString());

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
