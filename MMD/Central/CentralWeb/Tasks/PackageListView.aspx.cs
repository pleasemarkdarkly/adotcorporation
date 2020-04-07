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
using MMD.Central.CentralLib.XmlObjects;

namespace MMD.Central.CentralWeb.Tasks
{
	/// <summary>
	/// Summary description for PackageListView.
	/// </summary>
	public class PackageListView : System.Web.UI.Page
	{
		CSWContext _ctx;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
		}

		public string PackageNameCallback(DataRow row, ColumnBase df, int iRec)
		{
			return "/CentralWeb/Tasks/PackageView.aspx?idPackage=" + row["idPackage"];
		}

		public string SendEventCallback(DataRow row, ColumnBase df, int iRec)
		{
			return "/CentralWeb/Tasks/EventView.aspx?idEventSchedule=" + row["idSendEvent"];
		}

		public string RecvEventCallback(DataRow row, ColumnBase df, int iRec)
		{
			return "/CentralWeb/Tasks/EventView.aspx?idEventSchedule=" + row["idRecvEvent"];
		}

		public void DoListView()
		{
			ListView lv = new ListView(_ctx, "PackageListView","event_package_list", "sDesc", "asc", false, false);
			lv.StylePrefix = "LV";

			ListViewCallback lvcbPackageName = new ListViewCallback(this.PackageNameCallback);
			lv.AddColumn(new FieldCallbackColumn(lv, "sDesc", "Package Name", lvcbPackageName));

			lv.AddColumn(new FieldColumn(lv,"idSourceLevel","Source Level"));
			lv.AddColumn(new FieldColumn(lv,"idDestLevel","Destination Level"));

			ListViewCallback lvcbSendEvent = new ListViewCallback(this.SendEventCallback);
			lv.AddColumn(new FieldCallbackColumn(lv, "idSendEvent", "Send Event", lvcbSendEvent));

			ListViewCallback lvcbRecvEvent = new ListViewCallback(this.RecvEventCallback);
			lv.AddColumn(new FieldCallbackColumn(lv, "idRecvEvent", "Receive Event", lvcbRecvEvent));

			lv.AddColumn(new FieldColumn(lv,"sTransportSubdir","Transport Subdirectory"));

			lv.ProcessPageCommand();
			lv.SetPageRecs(true);
			lv.DoQuery();
			lv.SaveToSession();

			lv.EmitNavStart();
			lv.EmitLeftNav();
			lv.EmitRightNav();
			lv.EmitNavEnd();

			lv.EmitViewStart();
			lv.EmitHeaders();

			//lv.Write("<form>");
			//lv.Write("<table border=\"0\" width=\"100%\" cellpadding=\"0\" cellspacing=\"0\">");
			//lv.EmitColumnHeaders(false);
			lv.EmitRows();
			lv.EmitViewEnd();
			//lv.Write("</form>");
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
