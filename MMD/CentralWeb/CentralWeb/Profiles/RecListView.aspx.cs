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

namespace MMD.Central.CentralWeb.Profiles
{
	/// <summary>
	/// Summary description for RecListView.
	/// </summary>
	public class RecListView : System.Web.UI.Page
	{
		CSWContext _ctx;
		int _idRecListBaseType;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			_idRecListBaseType = _ctx.Params.SessionInt("idRecListBaseType",0);
		}

		public int idRecListBaseType
		{
			get
			{
				return _idRecListBaseType;
			}
		}

		public string ListTypeCallback(DataRow row, ColumnBase df, int iRec)
		{
			if(Convert.ToInt32(row["byStatus"]) != (int)StagingStatus.Deleted)
				return "/CentralWeb/Profiles/RecListItemView.aspx?idRecList=" + row["idRecList"];
			else
				return "";
		}

		public string EditCallback(DataRow row, ColumnBase df, int iRec)
		{
			if(Convert.ToInt32(row["byStatus"]) != (int)StagingStatus.Deleted)
				return "/CentralWeb/Profiles/RecListEdit.aspx?idRecList=" + row["idRecList"];
			else 
				return "";
		}
		
		public string DeleteCallback(DataRow row, ColumnBase df, int iRec)
		{
			int iStatus = Convert.ToInt32(row["byStatus"]);
			if(iStatus == (int)StagingStatus.Unchanged  || iStatus == (int)StagingStatus.Changed)
				return "javascript:ConfirmLink('Are you sure you want to delete this recommendation list?','/CentralWeb/Profiles/RecListView.aspx?cmd=RecListDelete&idRecList=" + row["idRecList"] + "')";
			else 
				return "";
		}
		
		public string UndoCallback(DataRow row, ColumnBase df, int iRec)
		{
			if(Convert.ToInt32(row["byStatus"]) != (int)StagingStatus.Unchanged)
			{
				return "javascript:ConfirmLink('Are you sure you want to undo changes to this recommendation list?','/CentralWeb/Profiles/RecListView.aspx?cmd=RecListUndo&idRecList=" + row["idRecList"] + "')";
			}
			else
				return "";
		}

		public string SubmitCallback(DataRow row, ColumnBase df, int iRec)
		{
			if(Convert.ToInt32(row["byStatus"]) != (int)StagingStatus.Unchanged)
			{
				return "/CentralWeb/Profiles/RecListView.aspx?cmd=RecListSubmit&idRecList=" + row["idRecList"];
			}
			else
				return "";
		}

		public void DoListView()
		{
			ListView lv = new ListView(_ctx, "RecListView","reclistview", false, false);
			lv.StylePrefix = "LV";
			
			ListViewCallback lvcbListType = new ListViewCallback(this.ListTypeCallback);
			lv.AddColumn(new FieldColumn(lv,"idStoreConcept","Concept"));
			lv.AddColumn(new FieldColumn(lv, "idRecListType", "List Type"));
			if(_idRecListBaseType == 1)
				lv.AddColumn(new FieldColumn(lv, "idProductType", "Product Type"));
			lv.AddColumn(new FieldColumn(lv, "cItems", "Size"));
			lv.AddColumn(new FieldColumn(lv, "dtChanged", "Date Changed"));
			lv.AddColumn(new FieldColumn(lv, "byStatus", "Status"));

			lv.AddDataField("bSystem");

			ListViewCallback lvcbEdit = new ListViewCallback(this.EditCallback);
			lv.AddColumn(new ShortcutCallbackColumn(lv, "&nbsp;", "Edit", lvcbEdit));

			ListViewCallback lvcbDelete = new ListViewCallback(this.DeleteCallback);
			lv.AddColumn(new ShortcutCallbackColumn(lv, "&nbsp;", "Delete", lvcbDelete));

			ListViewCallback lvcbUndo = new ListViewCallback(this.UndoCallback);
			lv.AddColumn(new ShortcutCallbackColumn(lv, "&nbsp;", "Undo", lvcbUndo));
			
			ListViewCallback lvcbSubmit = new ListViewCallback(this.SubmitCallback);
			lv.AddColumn(new ShortcutCallbackColumn(lv, "&nbsp;", "Submit", lvcbSubmit));

			if(_idRecListBaseType != 0)
			{
				lv.AddDataField("idRecListBaseType");
				lv.AddBaseFilter("idRecListBaseType", "eq", _idRecListBaseType.ToString());
			}

			//lv.AddBaseFilter("bSystem", "eq", "0");

			//lv.ProcessPageCommand();
			lv.SetPageRecs(false);
			lv.DoQuery();
			//lv.SaveToSession();

			//lv.EmitNavStart();
			//lv.EmitLeftNav();
			//lv.EmitRightNav();
			//lv.EmitNavEnd();

			//lv.EmitViewStart();
			//lv.EmitHeaders();

			//lv.Write("<form>");
			lv.Write("<table border=\"0\" width=\"100%\" cellpadding=\"0\" cellspacing=\"0\">");
			lv.EmitColumnHeaders(false);
			lv.EmitRows();
			lv.EmitViewEnd();
			//lv.Write("</form>");

		}

/*		public string ProductTypeComboBox
		{
			get
			{
				LookupTable lt = new LookupTable(_ctx.Config.MainDatabase,"select idProductType, sName from tblMMD_MetaDataProductType where idProductType <> 2 order by sName");
				return lt.GetHTMLComboBox("idProductType",_ctx.Params["idRecListType"],true,true);
			}
		}

		public string RecListTypeComboBox
		{
			get 
			{
				LookupTable lt = new LookupTable(_ctx.Config.MainDatabase,"select idRecListType, sName from tblMMD_RecommendListType where idRecListBaseType = " + _idRecListBaseType + " order by sName");
				return lt.GetHTMLComboBox("idRecListType",_ctx.Params["idProductType"],true,true);
			}
		}

		public string StoreConceptComboBox
		{
			get
			{
				LookupTable lt = new LookupTable(_ctx.Config.MainDatabase,"select idStoreConcept, sName from tblMMD_StoreConcept order by idStoreConcept");
				return lt.GetHTMLComboBox("idStoreConcept",_ctx.Params["idStoreConcept"],true,true);
			}
		}
*/
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
