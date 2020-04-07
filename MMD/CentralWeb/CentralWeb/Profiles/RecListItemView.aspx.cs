using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Data.SqlTypes;
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
	/// Summary description for RecListItemView.
	/// </summary>
	public class RecListItemView : System.Web.UI.Page
	{
		CSWContext _ctx;
		int _idRecList;
		string _sRecListType;
		string _sGenre;
		string _sProductType;
		string _sConcept;
		int _idProductType;
		bool _bHasGenre;
		bool _bHasConcept;
		int _idGenre;

		public CSWContext CSWContext { get { return _ctx; } }
		public string RecListType { get { return _sRecListType; } }
		public string Genre { get { return _sGenre; } }
		public string ProductType { get { return _sProductType; } }
		public string Title 
		{ 
			get { return RecListType + " - " + Concept + " - " + ProductType + ((HasGenre) ? (" - " + Genre) : " - All Genres" ); }
		}
		public string Concept
		{
			get { return (HasConcept) ? _sConcept : "All Concepts"; }
		}
		public int idProductType { get { return _idProductType; } }
		public bool HasGenre { get { return _bHasGenre; } }
		public bool HasConcept { get { return _bHasConcept; } }
		public int idGenre { get { return _idGenre; } }
		public int idRecList { get { return _idRecList; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );

			_idRecList = _ctx.Params.SessionInt("idRecList");

			//fetch genre and media type and rec list type for display
			DataSet ds = _ctx.Config.MainDatabase.execSQL_DS("select * from vwCSW_RecListStagingInfo where idRecList = " + _idRecList,"Query");
			DataRow row = ds.Tables[0].Rows[0];
			_sRecListType = row["sRecListType"].ToString();
			_sGenre = row["sGenre"].ToString();
			_sProductType = row["sProductType"].ToString();
			_idProductType = Convert.ToInt32(row["idProductType"]);

			_sConcept = Convert.ToString(row["sStoreConcept"]);
			_bHasConcept = row["idStoreConcept"] != System.DBNull.Value;
			_bHasGenre = row["idGenre"] != System.DBNull.Value; 
			//SqlInt32.NotEquals((SqlInt32)row["idGenre"],SqlInt32.Null).IsTrue;
			if(_bHasGenre)
				_idGenre = Convert.ToInt32(row["idGenre"]);
		}

		public void DoListView()
		{
			ListView lv = new ListView(_ctx, "RecListItemView","reclistitemview", "iSeq", "asc", _ctx.LoadListViewFromSession , false);
		
			lv.AddColumn(new CheckboxColumn(lv, "Checked", "%idRecListItem%"));
			lv.AddColumn(new FieldColumn(lv, "iSeq", "Seq"));
			lv.AddColumn(new FieldColumn(lv, "sTitle", "Title"));
			lv.AddColumn(new FieldColumn(lv, "sArtist", "Artist/<br>Director"));
			//lv.AddColumn(new FieldColumn(lv, "idProductType", "Product Type"));
			if(!HasGenre) lv.AddColumn(new FieldColumn(lv, "idGenre", "Genre"));
			lv.AddColumn(new ShortcutColumn(lv, "Insert", "javascript:SubmitCommand('RecListItemInsert',%idRecListItem%)"));
			lv.AddColumn(new ShortcutColumn(lv, "Delete", "javascript:if(CacheConfirm('RecListItemDelete','','Are you sure you wish to remove this item from this recommendation list?')) SubmitCommand('RecListItemDelete',%idRecListItem%)"));
			lv.AddBaseFilter("idRecList","eq",_idRecList.ToString());

			lv.ProcessPageCommand();
			lv.DoQuery();
			lv.SaveToSession();

			lv.EmitNavStart();
			lv.EmitLeftNav();
			lv.EmitRightNav();
			lv.EmitNavEnd();

			lv.EmitViewStart();
			lv.EmitHeaders();

			lv.Write("<form id=\"frmMain\" method=\"post\" action=\"/CentralWeb/Profiles/RecListItemView.aspx\">");
			
			lv.EmitRows();
			lv.EmitViewEnd();
			lv.Write("<input type=\"hidden\" name=\"idRecList\" value=\"" + _idRecList + "\" >");
			lv.Write("<input id=\"CommandInput\" type=\"hidden\" name=\"cmd\" value=\"\" >");
			lv.Write("<input id=\"CommandId\" type=\"hidden\" name=\"idRecListItem\" value=\"\" >");


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
