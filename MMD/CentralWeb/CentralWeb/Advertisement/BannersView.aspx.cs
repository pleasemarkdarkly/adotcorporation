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
using MMD.Central.CentralLib.DataObjects;

namespace MMD.Central.CentralWeb.Advertisement
{
	/// <summary>
	/// Summary description for BannersView.
	/// </summary>
	public class BannersView : System.Web.UI.Page
	{
		BannerType _bt;

		CSWContext _ctx;

		public int idBannerType { get { return _bt._idBannerType; } }
		public string BannerTypeName { get { return _bt._sName; } }

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			int idBannerType = _ctx.Params.SessionInt("idBannerType");

			_bt = new BannerType(_ctx, idBannerType);
	
		}

		public string NameCallback(DataRow row, ColumnBase df, int iRec)
		{
			if(Convert.ToInt32(row["byStatus"]) != (int)StagingStatus.Deleted)
				return "/CentralWeb/Advertisement/BannerView.aspx?idBanner=" + row["idBanner"];
			else
				return "";
		}

		public string URLCallback(DataRow row, ColumnBase df, int iRec)
		{
			return "<img width=\"" + _bt._iWidth + "\" height=\"" + _bt._iHeight + "\" src=\"" + _ctx.Config.BannerWebPath(true) + "/" + row["sURL"] + "\">";
		}

		public string DeleteCallback(DataRow row, ColumnBase df, int iRec)
		{
			int iStatus = Convert.ToInt32(row["byStatus"]);
			if(iStatus == (int)StagingStatus.Unchanged  || iStatus == (int)StagingStatus.Changed)
				return "javascript:ConfirmLink('Are you sure you want to delete this banner?','/CentralWeb/Advertisement/BannersView.aspx?cmd=BannerDelete&idBanner=" + row["idBanner"] + "')";
			else 
				return "";
		}
		
		public string UndoCallback(DataRow row, ColumnBase df, int iRec)
		{
			if(Convert.ToInt32(row["byStatus"]) != (int)StagingStatus.Unchanged)
			{
				return "javascript:ConfirmLink('Are you sure you want to undo changes to this banner?','/CentralWeb/Advertisement/BannersView.aspx?cmd=BannerUndo&idBanner=" + row["idBanner"] + "')";
			}
			else
				return "";
		}

		public string SubmitCallback(DataRow row, ColumnBase df, int iRec)
		{
			if(Convert.ToInt32(row["byStatus"]) != (int)StagingStatus.Unchanged)
			{
				return "/CentralWeb/Advertisement/BannersView.aspx?cmd=BannerSubmit&idBanner=" + row["idBanner"];
			}
			else
				return "";
		}

		public void DoListView()
		{
			ListView lv = new ListView(_ctx, "BannersView","banners", _ctx.LoadListViewFromSession , false);
		
			ListViewCallback lvcbName = new ListViewCallback(this.NameCallback);
			lv.AddColumn(new FieldCallbackColumn(lv, "sName", "Name", lvcbName));

			//lv.AddColumn(new FieldColumn(lv, "idBannerType", "Type"));

			ListViewCallback lvcbURL = new ListViewCallback(this.URLCallback);
			lv.AddColumn(new ControlCallbackColumn(lv, "Banner", lvcbURL));

			lv.AddColumn(new FieldColumn(lv, "byStatus", "Status"));

			ListViewCallback lvcbDelete = new ListViewCallback(this.DeleteCallback);
			lv.AddColumn(new ShortcutCallbackColumn(lv, "Delete", lvcbDelete));

			ListViewCallback lvcbUndo = new ListViewCallback(this.UndoCallback);
			lv.AddColumn(new ShortcutCallbackColumn(lv, "Undo", lvcbUndo));
			
			ListViewCallback lvcbSubmit = new ListViewCallback(this.SubmitCallback);
			lv.AddColumn(new ShortcutCallbackColumn(lv, "Submit", lvcbSubmit));
			
			lv.AddDataField("sURL");
			lv.AddBaseFilter("idBannerType","eq",_bt._idBannerType.ToString());

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
