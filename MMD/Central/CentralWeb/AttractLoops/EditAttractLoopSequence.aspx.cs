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
	/// Summary description for EditAttractLoopSequence.
	/// </summary>
	public class EditAttractLoopSequence : System.Web.UI.Page
	{
		CSWContext _ctx;
		IdleLoopPersister _ilp;
		IdleLoop _il;
		
		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			_ilp = new IdleLoopPersister(_ctx,false);
			
			_il = _ilp.GetIdleLoopById(_ctx.Params.SessionInt("idIdleLoop"));
		}

		public IdleLoop idleLoop { get { return _il; } }
	
		public int IdleLoopItems { get { return (idleLoop.Frames != null) ? idleLoop.Frames.Length : 0; } }



		public void DoAttractLoopItemsListView()
		{
			
			ListView lv = new ListView(_ctx, "EditAttractLoopSequence","attract_loop_items", "iSeq", null);

			lv.AddColumn(new ShortcutColumn(lv, "Up", "EditAttractLoopSequence.aspx?cmd=AttractLoopItemMove&up=1&idIdleLoopElement=%idIdleLoopElement%"));
			lv.AddColumn(new ShortcutColumn(lv, "Down", "EditAttractLoopSequence.aspx?cmd=AttractLoopItemMove&up=0&idIdleLoopElement=%idIdleLoopElement%"));

			lv.AddColumn(new FieldColumn(lv, "iSeq", "Sequence"));
			lv.AddColumn(new FieldLinkColumn(lv, "sName", "Loop Item Name", "EditAttractLoopSequenceItem.aspx?idIdleLoopElement=%idIdleLoopElement%"));
			lv.AddColumn(new FieldColumn(lv, "iFrameType", "Frame Type"));
			
			lv.AddColumn(new ShortcutColumn(lv, "Remove", "javascript:ConfirmLink('Are you sure you want to remove this item from the attract loop?','EditAttractLoopSequence.aspx?cmd=AttractLoopItemDelete&idIdleLoopElement=%idIdleLoopElement%')"));

			/*
			ListViewCallback lvcbFile = new ListViewCallback(this.FileCallback);
			lv.AddColumn(new FieldCallbackColumn(lv, "sFileName", "File", lvcbFile )).Target = "_blank";
			lv.AddColumn(new FieldColumn(lv,"cAssociations", "Assoc."));
			lv.AddColumn(new FieldColumn(lv, "dtChanged", "Change Date"));
			lv.AddColumn(new FieldColumn(lv, "sChangedBy", "Changed By"));
			*/
			
			lv.AddDataField("idIdleLoop");
			lv.AddBaseFilter("idIdleLoop","eq", _il.idIdleLoop.ToString());
			lv.SetPageRecs(false);

			//lv.ProcessPageCommand();
			lv.DoQuery();
			lv.SaveToSession();

			//lv.EmitNavStart();
			//lv.EmitLeftNav();
			//lv.EmitRightNav();
			//lv.EmitNavEnd();

			//lv.EmitViewStart();
			//lv.EmitHeaders();
			lv.EmitColumnHeaders(false);
			
			lv.EmitRows();
			//lv.EmitViewEnd();

			
		}


		public string FileCallback(DataRow row, ColumnBase df, int iRec)
		{
			return _ctx.Config.GetWebPath(Convert.ToInt32(row["idStoreConcept"]),MMDPostedFileType.IdleLoopElement,row["sFileName"].ToString());
		}


		public void DoAttractLoopItemFileListView()
		{
			// are these paramaters just names, or do they actually point to something?
			
			ListView lv = new ListView(_ctx, "EditAttractLoopSequence","registered_files", false, false);

			lv.AddColumn(new FieldColumn(lv, "sName", "Loop Item Name"));
			lv.AddColumn(new FieldColumn(lv, "iFileMediaType", "Type"));
			
			ListViewCallback lvcbFile = new ListViewCallback(this.FileCallback);
			lv.AddColumn(new FieldCallbackColumn(lv, "sFileName", "File", lvcbFile )).Target = "_blank";
			lv.AddColumn(new FieldColumn(lv,"cAssociations", "Assoc."));
			lv.AddColumn(new FieldColumn(lv, "dtChanged", "Change Date"));
			lv.AddColumn(new FieldColumn(lv, "sChangedBy", "Changed By"));
			
			lv.AddColumn(new ShortcutColumn(lv, "Add", "/CentralWeb/AttractLoops/EditAttractLoopSequenceItem.aspx?idFile=%idFile%&iFrameType=%iFileMediaType%"));

			lv.AddDataField("iType");
			lv.AddDataField("idStoreConcept");
			lv.AddBaseFilter("iType","eq",((int)MMDPostedFileType.IdleLoopElement).ToString());
			lv.AddBaseFilter("idStoreConcept", "in", "0," + _il.idStoreConcept);

			//lv.ProcessPageCommand();
			lv.SetPageRecs(false);
			lv.DoQuery();
			lv.SaveToSession();

/*			lv.EmitNavStart();
			lv.EmitLeftNav();
			lv.EmitRightNav();
			lv.EmitNavEnd();

			lv.EmitViewStart();
			lv.EmitHeaders();
*/
			lv.EmitColumnHeaders(false);
			lv.EmitRows();
			//lv.EmitViewEnd();
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
