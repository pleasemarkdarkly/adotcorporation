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

namespace MMD.Central.CentralWeb.AttractLoops
{
	/// <summary>
	/// Summary description for ViewAttractLoops.
	/// </summary>
	public class ViewAttractLoops : System.Web.UI.Page
	{
		CSWContext _ctx;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
		}


		public string NameCallback(DataRow row, ColumnBase df, int iRec)
		{
			if(Convert.ToInt32(row["byStatus"]) != (int)StagingStatus.Deleted)
				return "/CentralWeb/AttractLoops/EditAttractLoopSequence.aspx?idIdleLoop=" + row["idIdleLoop"];
			else
				return "";
		}

		public string DeleteCallback(DataRow row, ColumnBase df, int iRec)
		{
			int iStatus = Convert.ToInt32(row["byStatus"]);
			int cAssoc = Convert.ToInt32(row["cAssociations"]);
			if(cAssoc == 0 && (iStatus == (int)StagingStatus.Unchanged ))
				return "javascript:ConfirmLink('Are you sure you want to delete this attract loop?','/CentralWeb/AttractLoops/ViewAttractLoops.aspx?cmd=AttractLoopDelete&idIdleLoop=" + row["idIdleLoop"] + "')";
			else 
				return "";
		}
		
		public string UndoCallback(DataRow row, ColumnBase df, int iRec)
		{
			if(Convert.ToInt32(row["byStatus"]) != (int)StagingStatus.Unchanged)
			{
				return "javascript:ConfirmLink('Are you sure you want to undo changes to this attract loop?','/CentralWeb/AttractLoops/ViewAttractLoops.aspx?cmd=AttractLoopUndo&idIdleLoop=" + row["idIdleLoop"] + "&sName=" + Server.UrlEncode(row["sName"].ToString()) + "')";
			}
			else
				return "";
		}

		public string SubmitCallback(DataRow row, ColumnBase df, int iRec)
		{
			if(Convert.ToInt32(row["byStatus"]) != (int)StagingStatus.Unchanged)
			{
				return "/CentralWeb/AttractLoops/ViewAttractLoops.aspx?cmd=AttractLoopSubmit&idIdleLoop=" + row["idIdleLoop"];
			}
			else
				return "";
		}

		public void DoListView()
		{
			ListView lv = new ListView(_ctx, "ViewAttractLoops","idleloops", _ctx.LoadListViewFromSession , false);
		
			ListViewCallback lvcbName = new ListViewCallback(this.NameCallback);
			lv.AddColumn(new FieldColumn(lv, "idStoreConcept", "Concept"));
			lv.AddColumn(new FieldCallbackColumn(lv, "sName", "Name", lvcbName));

			lv.AddColumn(new FieldColumn(lv, "cItems", "Frames"));
			lv.AddColumn(new FieldColumn(lv, "cAssociations", "Assoc."));

			lv.AddColumn(new FieldColumn(lv, "byStatus", "Status"));

			lv.AddColumn(new FieldColumn(lv, "dtChanged", "Date<br>Changed"));
			lv.AddColumn(new FieldColumn(lv, "sChangedBy", "Changed By"));

			ListViewCallback lvcbDelete = new ListViewCallback(this.DeleteCallback);
			lv.AddColumn(new ShortcutCallbackColumn(lv, "Delete", lvcbDelete));

			ListViewCallback lvcbUndo = new ListViewCallback(this.UndoCallback);
			lv.AddColumn(new ShortcutCallbackColumn(lv, "Undo", lvcbUndo));
			
			ListViewCallback lvcbSubmit = new ListViewCallback(this.SubmitCallback);
			lv.AddColumn(new ShortcutCallbackColumn(lv, "Submit", lvcbSubmit));
			
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
