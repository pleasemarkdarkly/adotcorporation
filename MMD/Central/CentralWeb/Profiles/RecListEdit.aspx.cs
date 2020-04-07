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
using MMD.Central.CentralLib.Utilities;
using MMD.Central.CentralLib.XmlObjects;

namespace MMD.Central.CentralWeb.Profiles
{
	/// <summary>
	/// Summary description for RecListEdit.
	/// </summary>
	public class RecListEdit : System.Web.UI.Page
	{
		CSWContext _ctx;
		int _idRecList;
		bool _bRLTItemsHaveExtAttr;
		RecListPersister _rlp;
		RecListType _rlt;
		RecList _rl;
		ListView _lv;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			_idRecList = _ctx.Params.SessionInt("idRecList");

			_rlp = new RecListPersister(_ctx,false,Location.Staging);
			_rl = _rlp.GetRecListById(_idRecList);
			_rlt = _rl.Type;

			
			// find out if the list items have extended attributes
			RecListTypeItemAttribute[] rltas = _rlt.RecListItemAttributes;
			_bRLTItemsHaveExtAttr = (rltas.Length > 0);
		}

		public int idRecList { get { return _idRecList; } }
		public bool RLTItemsHaveExtAttr() { return _bRLTItemsHaveExtAttr; }
		public RecListType RecListType { get { return _rlt; } }
		public RecList recList { get { return _rl; } }

		public int ExtendedAttributeCount()
		{
				RecListAttribute[] rlas = _rl.RecListAttributes;
				return rlas.Length;
		}

		public void DoListExtendedAttributes()
		{
			RecListAttribute[] rlas = _rl.RecListAttributes;
			int ixAttrCount = rlas.Length;
			if(ixAttrCount > 0)
			{
				int idx = 0;
				Response.Write("<tr><td colspan=\"3\">&nbsp;</td></tr>");
				Response.Write("<tr><td colspan=\"3\"><span class=\"SmallText\"><b>Recommend List Extended Attributes</b></span></td></tr>");
				foreach(RecListAttribute rla in rlas)
				{

					Response.Write("<input type=\"hidden\" name=\"ixAttr_" + idx + "\" value=\"" + rla.idRecListAttribute + "\">");
					Response.Write("<tr><td colspan=\"3\"><span class=\"SmallText\"><b>" + rla.TypeAttribute.sName + "</b> [" + rla.TypeAttribute.AttributeType.sName +"]");

					// show if the field is required
					if(rla.TypeAttribute.bRequired == true)
					{
						Response.Write("<span style=\" color: red;\"> *</span></span></td></tr>");
						Response.Write(HTML.MakeHiddenInput("label_ixAttrValue_" + idx, rla.TypeAttribute.sName));
						Response.Write(HTML.MakeHiddenInput("required_ixAttrValue_" + idx, "1"));
					}
					else
						Response.Write("</span></td></tr>");

					Response.Write("<tr><td colspan=\"3\">");
					// create the write input box for the different attribute types
					switch(rla.TypeAttribute.AttributeType.sName)
					{
						case "Text":
							Response.Write("<textarea cols=\"50\" rows=\"3\" name=\"ixAttrValue_" + idx + "\">" + rla.txValue + "</textarea>");
							break;
						case "Number":
							Response.Write("<input type=\"text\" size=\"20\" maxlength=\"255\" name=\"ixAttrValue_" + idx + "\" value=\"" + rla.sValue + "\">");
							break;
						case "String":
							Response.Write("<input type=\"text\" size=\"50\" maxlength=\"255\" name=\"ixAttrValue_" + idx + "\" value=\"" + rla.sValue + "\">");
							break;
						case "Binary":
							LookupTable lt = new LookupTable("1,Yes,0,No", ",");
							Response.Write(lt.GetHTMLComboBox("ixAttrValue_" + idx, rla.sValue, true, true));
							break;
						case "URL":
							Response.Write("<input type=\"text\" size=\"50\" maxlength=\"255\" name=\"ixAttrValue_" + idx + "\" value=\"" + rla.sValue + "\">");
							break;
						default:
							Response.Write("<span class=\"SmallText\"><b>ERROR: Unknown List Extended Attribute Type!</b>");
							break;
					}
					//Response.Write("");
					Response.Write("</span></td></tr>");

					Response.Write("<tr><td colspan=\"3\">&nbsp;</td></tr>");
					idx++;
				}
			}
			Response.Write(HTML.MakeHiddenInput("ixAttrCount", ixAttrCount.ToString()));
		}

		public string MoveUpCallback(DataRow row, ColumnBase df, int iRec)
		{
			return " ";
		}

		public string MoveDownCallback(DataRow row, ColumnBase df, int iRec)
		{
			return " ";
		}

		public string EditAttrItemCallback(DataRow row, ColumnBase df, int iRec)
		{
			// this target field is kind of a hack.
			return "/CentralWeb/Profiles/RecListItemAttrEdit.aspx?idRecList=" + _idRecList + "&idRecListItem=" + row["idRecListItem"];
		}

		public string RemoveItemCallback(DataRow row, ColumnBase df, int iRec)
		{
			return "javascript:" + "ConfirmLink('Are you sure you want to delete this item?','/CentralWeb/Profiles/RecListEdit.aspx?cmd=RecListItemDelete&idRecListItem=" + row["idRecListItem"] + "&idRecList=" + _idRecList + "')";
		}

		public void DoItemListViewStart()
		{
			_lv = new ListView(_ctx, "RecListEdit","reclistitemview", false, false);
			_lv.StylePrefix = "LV";

			string sItemLabel = (_rlt.BaseType.sName == "Product") ? "Item ID" : "Promotion ID";


			_lv.AddColumn(new ShortcutColumn(_lv, "Up", "RecListEdit.aspx?cmd=RecListItemMove&up=1&idRecListItem=%idRecListItem%"));
			_lv.AddColumn(new ShortcutColumn(_lv, "Down", "RecListEdit.aspx?cmd=RecListItemMove&up=0&idRecListItem=%idRecListItem%"));

			_lv.AddColumn(new FieldColumn(_lv, "iSeq", "Sequence"));
			_lv.AddColumn(new FieldColumn(_lv, "idItem", sItemLabel));

			if(_rlt.BaseType.sName == "Product")
			{
				_lv.AddColumn(new FieldColumn(_lv, "sTitle", "Title"));
				_lv.AddColumn(new FieldColumn(_lv, "sArtist", "Artist / Director"));
			}

			_lv.AddDataField("idRecList");
			_lv.AddBaseFilter("idRecList", "eq", _idRecList.ToString());

			ListViewCallback lvcbRemove = new ListViewCallback(this.RemoveItemCallback);

			_lv.AddColumn(new ShortcutCallbackColumn(_lv, "Remove", "Remove", lvcbRemove));
			//ListViewCallback lvcbMoveUp = new ListViewCallback(this.MoveUpCallback);
			//_lv.AddColumn(new ShortcutCallbackColumn(_lv, "", "Move Up", lvcbMoveUp));
			//ListViewCallback lvcbMoveDown = new ListViewCallback(this.MoveDownCallback);
			//_lv.AddColumn(new ShortcutCallbackColumn(_lv, "", "Move Down", lvcbMoveDown));

			// if this list type has list items with extended attributes, show an edit button
			if(_bRLTItemsHaveExtAttr)
			{
				ListViewCallback lvcbEditAttr = new ListViewCallback(this.EditAttrItemCallback);
				_lv.AddColumn(new ShortcutCallbackColumn(_lv, "Edit Attributes", "Edit Attributes", lvcbEditAttr));
			}

			//_lv.ProcessPageCommand();
			_lv.SetPageRecs(false);
			_lv.DoQuery();
			//_lv.SaveToSession();

			//_lv.EmitNavStart();
			//_lv.EmitLeftNav();
			//_lv.EmitRightNav();
			//_lv.EmitNavEnd();

			//_lv.EmitViewStart();
			_lv.Write("<table border=\"0\" width=\"100%\" cellpadding=\"0\" cellspacing=\"0\">");
			_lv.EmitColumnHeaders(false);
		}

		public void DoItemListViewRows()
		{
			_lv.EmitRows();
		}

		public void DoItemListViewEnd()
		{
			_lv.EmitViewEnd();
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
