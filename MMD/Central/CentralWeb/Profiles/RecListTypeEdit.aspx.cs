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

namespace MMD.Central.CentralWeb.Profiles
{
	/// <summary>
	/// Summary description for RecListTypeEdit.
	/// </summary>
	public class RecListTypeEdit : System.Web.UI.Page
	{
		CSWContext _ctx;
		int _idRecListType;
		string _idRecListBaseType;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			_idRecListType = _ctx.Params.Int("idRecListType",0);

			// look up the base type
			LookupTable lt = new LookupTable(_ctx.Config.MainDatabase,"select idRecListType, idRecListBaseType from tblMMD_RecommendListTypeStaging order by idRecListType");
			_idRecListBaseType = lt.GetValue(_idRecListType.ToString());
		}

		public int idRecListType
		{
			get
			{
				return _idRecListType;
			}
		}

		public string RecListNewTypeName
		{
			get
			{
				LookupTable lt = new LookupTable(_ctx.Config.MainDatabase,"select idRecListType, sName from tblMMD_RecommendListTypeStaging order by idRecListType");
				return lt.GetValue(_idRecListType.ToString());
			}
		}

		public string RecListNewTypeBaseTypeName
		{
			get
			{
				LookupTable lt = new LookupTable(_ctx.Config.MainDatabase,"select idRecListBaseType, sName from tblMMD_RecommendListBaseType order by idRecListBaseType");
				return lt.GetValue(_idRecListBaseType);
			}
		}

		public string RecListAttributeTypeComboBox
		{
			get
			{
				LookupTable lt = new LookupTable(_ctx.Config.MainDatabase,"select idRecListAttributeType, sName from tblMMD_RecommendListAttributeType order by idRecListAttributeType");
				return lt.GetHTMLComboBox("idRecListAttributeType",_ctx.Params["idRecListAttributeType"],false,true);
			}
		}

		public string RemoveAttributeCallback(DataRow row, ColumnBase df, int iRec)
		{
			return "javascript:" + "ConfirmLink('Are you sure you want to delete this attribute?','/CentralWeb/Profiles/RecListTypeEdit.aspx?cmd=RecListTypeAttributeDelete&idRecListTypeAttribute=" + row["idRecListTypeAttribute"] + "&idRecListType=" + _idRecListType + "')";
		}
		
		public string RemoveItemAttributeCallback(DataRow row, ColumnBase df, int iRec)
		{
			return "javascript:" + "ConfirmLink('Are you sure you want to delete this item attribute?','/CentralWeb/Profiles/RecListTypeEdit.aspx?cmd=RecListTypeItemAttributeDelete&idRecListTypeItemAttribute=" + row["idRecListTypeItemAttribute"] + "&idRecListType=" + _idRecListType + "')";
		}
		
		public void DoListAttributeListView()
		{
			ListView lv = new ListView(_ctx, "RecListTypeEdit","rec_list_type_attr", false, false);
			lv.StylePrefix = "LV";

			ColumnBase cb;
			lv.AddColumn(new FieldColumn(lv, "sName", "Recommend List Attribute Name"));
			cb = lv.AddColumn(new FieldColumn(lv, "idRecListAttributeType", "Attribute Type"));
			cb.Align = "center";

			cb = lv.AddColumn(new FieldColumn(lv, "bRequired", "Field Required"));
			cb.Align = "center";

			lv.AddFilter("idRecListType","eq",_idRecListType.ToString());

			ListViewCallback lvcbRemove = new ListViewCallback(this.RemoveAttributeCallback);
			lv.AddColumn(new ShortcutCallbackColumn(lv, "Remove", "Remove", lvcbRemove));

			//lv.ProcessPageCommand();
			lv.DoQuery();
			//lv.SaveToSession();

			//lv.EmitNavStart();
			//lv.EmitLeftNav();
			//lv.EmitRightNav();
			//lv.EmitNavEnd();

			//lv.EmitViewStart();
			lv.EmitHeaders(false,false);

			//lv.Write("<form>");
			lv.EmitRows();
			//lv.EmitViewEnd();
			//lv.Write("</form>");
		}

		public void DoItemAttributeListView()
		{
			// Show a list of rec list item attributes
			ListView lv = new ListView(_ctx, "RecListTypeEdit","rec_list_type_item_attr", false, false);
			lv.StylePrefix = "LV";

			ColumnBase cb;

			lv.AddColumn(new FieldColumn(lv, "sName", "Recommend List Attribute Name"));
			
			cb = lv.AddColumn(new FieldColumn(lv, "idRecListAttributeType", "Attribute Type"));
			cb.Align = "center";

			cb = lv.AddColumn(new FieldColumn(lv, "bRequired", "Field Required"));
			cb.Align = "center";

			lv.AddFilter("idRecListType","eq",_idRecListType.ToString());

			ListViewCallback lvcbRemove = new ListViewCallback(this.RemoveItemAttributeCallback);
			lv.AddColumn(new ShortcutCallbackColumn(lv, "Remove", "Remove", lvcbRemove));

			//lv.ProcessPageCommand();
			lv.DoQuery();
			//lv.SaveToSession();

			//lv.EmitNavStart();
			//lv.EmitLeftNav();
			//lv.EmitRightNav();
			//lv.EmitNavEnd();

			//lv.EmitViewStart();
			lv.EmitHeaders(false,false);

			//lv.Write("<form>");
			lv.EmitRows();
			//lv.EmitViewEnd();
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
