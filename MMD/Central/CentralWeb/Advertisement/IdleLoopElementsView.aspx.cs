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
using MMD.Central.CentralLib.Utilities;
using MMD.Central.CentralLib.DataObjects;
using MMD.Central.CentralLib.ListView;

namespace MMD.Central.CentralWeb.Advertisement
{
	/// <summary>
	/// Summary description for IdleLoopElementsView.
	/// </summary>
	public class IdleLoopElementsView : System.Web.UI.Page
	{
		CSWContext _ctx;
		IdleLoop _idleloop;

		public CSWContext CSWContext { get { return _ctx; } }
		public IdleLoop IdleLoop { get { return _idleloop; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			int idIdleLoop = _ctx.Params.SessionInt("idIdleLoop");
			_idleloop = new IdleLoop(_ctx,idIdleLoop);

		}

		public void DoListView()
		{
			ListView lv = new ListView(_ctx, "IdleLoopElementsView","idleloopelements", "iSeq", "asc", _ctx.LoadListViewFromSession , false);
					
			lv.AddColumn(new CheckboxColumn(lv, "Checked", "%idIdleLoopElement%"));
			lv.AddColumn(new FieldColumn(lv, "iSeq", "Seq"));
			lv.AddColumn(new FieldLinkColumn(lv, "sName", "Name", "/CentralWeb/Advertisement/IdleLoopElementView.aspx?idIdleLoopElement=%idIdleLoopElement%"));
			lv.AddColumn(new FieldColumn(lv, "sMIMEType", "MIME Type"));
			//asset info - path
			lv.AddColumn(new FieldColumn(lv, "cSecsPlayingTime", "Display Time"));
			lv.AddColumn(new FieldColumn(lv, "dtChanged", "Last Changed"));
			lv.AddColumn(new FieldColumn(lv, "sChangedBy", "Changed By"));

			lv.AddColumn(new ShortcutColumn(lv, "Edit", "/CentralWeb/Advertisement/IdleLoopElementEdit.aspx?idIdleLoopElement=%idIdleLoopElement%"));
			lv.AddColumn(new ShortcutColumn(lv, "Insert", "javascript:SubmitCommand('IdleLoopElementInsert',%idIdleLoopElement%)"));
			lv.AddColumn(new ShortcutColumn(lv, "Delete", "javascript:if(CacheConfirm('IdleLoopElementDelete','','Are you sure you wish to remove this frame from this idle loop?')) SubmitCommand('IdleLoopElementDelete',%idIdleLoopElement%)"));
			
			lv.AddDataField("idIdleLoop");
			
			lv.AddBaseFilter("idIdleLoop","eq",IdleLoop.idIdleLoop.ToString());
			
			lv.ProcessPageCommand();
			lv.DoQuery();
			lv.SaveToSession();

			lv.EmitNavStart();
			lv.EmitLeftNav();
			lv.EmitRightNav();
			lv.EmitNavEnd();

			lv.EmitViewStart();
			lv.EmitHeaders();

			lv.Write("<form id=\"frmMain\" method=\"post\" action=\"/CentralWeb/Advertisement/IdleLoopElementsView.aspx\">");
			
			lv.EmitRows();
			lv.EmitViewEnd();
			lv.Write("<input type=\"hidden\" name=\"idIdleLoop\" value=\"" + IdleLoop.idIdleLoop + "\" >");
			lv.Write("<input id=\"CommandInput\" type=\"hidden\" name=\"cmd\" value=\"\" >");
			lv.Write("<input id=\"CommandId\" type=\"hidden\" name=\"idIdleLoopElement\" value=\"\" >");

			lv.Write("</form>");

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
