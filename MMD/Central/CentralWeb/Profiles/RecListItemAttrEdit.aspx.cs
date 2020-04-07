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
	/// Summary description for RecListItemAttrEdit.
	/// </summary>
	public class RecListItemAttrEdit : System.Web.UI.Page
	{
		CSWContext _ctx;
		int _idRecList;
		int _idRecListItem;
		RecListItem _rli;
		RecListPersister _rlp;
		RecList _rl;
		string _sArtist;
		string _sTitle;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			_idRecList = _ctx.Params.Int("idRecList",0);
			_idRecListItem = _ctx.Params.Int("idRecListItem",0);
			_rlp = new RecListPersister(_ctx,false,Location.Staging);
			_rl = _rlp.GetRecListById(_idRecList);
			RecListItem[] rlis = _rl.RecListItems;
			foreach(RecListItem rli in rlis)
			{
				if(rli.idRecListItem == _idRecListItem)
					_rli = rli;
			}

			if(_rl.Type.BaseType.sName == "Product")
				GetTitleArtist(_rli.idRecListItem , out _sTitle, out _sArtist);

		}

		public string Artist { get { return _sArtist; } }
		public string Title { get { return _sTitle; } }

		public RecList recList { get { return _rl; } }
		public RecListItem recListItem { get { return _rli; } }

		public void GetTitleArtist(int idRecListItem, out string sTitle, out string sArtist)
		{
			sTitle = "";
			sArtist = "";
			//string sTitle;
			//string sArtist;
			DataSet ds = _ctx.Config.MainDatabase.execSQL_DS("select * from vwMMD_RecommendListItemStaging_lv where idRecListItem = " + idRecListItem,"Query");
			DataRow row = ds.Tables[0].Rows[0];
			sTitle = row["sTitle"].ToString();
			sArtist = row["sArtist"].ToString();
		}	


		public void DoListItemExtendedAttributes()
		{
			RecListItemAttribute[] rlias = _rli.RecListItemAttributes;
			int ixAttrCount = rlias.Length;
			if(ixAttrCount > 0)
			{
				int idx = 0;
				Response.Write("<tr><td colspan=\"4\">&nbsp;</td></tr>");
				Response.Write("<tr><td colspan=\"4\"><span class=\"SmallText\"><b>[" + _rl.Type.sName + "] List Item Extended Attributes</b></span></td></tr>");
				foreach(RecListItemAttribute rlia in rlias)
				{

					Response.Write("<input type=\"hidden\" name=\"ixAttr_" + idx + "\" value=\"" + rlia.idRecListItemAttribute + "\">");
					Response.Write("<tr><td colspan=\"4\"><span class=\"SmallText\"><b>" + rlia.TypeItemAttribute.sName + "</b> [" + rlia.TypeItemAttribute.AttributeType.sName +"]");

					// show if the field is required
					if(rlia.TypeItemAttribute.bRequired == true)
					{
						Response.Write("<span style=\" color: red;\">*</span></td></tr>");
						Response.Write(HTML.MakeHiddenInput("label_ixAttrValue_" + idx, rlia.TypeItemAttribute.sName));
						Response.Write(HTML.MakeHiddenInput("required_ixAttrValue_" + idx, "1"));
					}
					else
						Response.Write("</span></td></tr>");

					Response.Write("<tr><td colspan=\"4\">");
					// create the write input box for the different attribute types
					switch(rlia.TypeItemAttribute.AttributeType.sName)
					{
						case "Text":
							Response.Write("<textarea cols=\"50\" rows=\"3\" name=\"ixAttrValue_" + idx + "\">" + rlia.txValue + "</textarea>");
							break;
						case "Number":
							Response.Write("<input type=\"text\" size=\"20\" maxlength=\"255\" name=\"ixAttrValue_" + idx + "\" value=\"" + rlia.sValue + "\">");
							break;
						case "String":
							Response.Write("<input type=\"text\" size=\"50\" maxlength=\"255\" name=\"ixAttrValue_" + idx + "\" value=\"" + rlia.sValue + "\">");
							break;
						case "Binary":
							LookupTable lt = new LookupTable("1,Yes,0,No", ",");
							Response.Write(lt.GetHTMLComboBox("ixAttrValue_" + idx, rlia.sValue, true, true));
							break;
						case "URL":
							Response.Write("<input type=\"text\" size=\"50\" maxlength=\"255\" name=\"ixAttrValue_" + idx + "\" value=\"" + rlia.sValue + "\">");
							break;
						default:
							Response.Write("<span class=\"SmallText\"><b>ERROR: Unknown List Extended Attribute Type!</b>");
							break;
					}
					//Response.Write("");
					Response.Write("</span></td></tr>");

					Response.Write("<tr><td colspan=\"4\">&nbsp;</td></tr>");
					idx++;
				}
			}
			Response.Write(HTML.MakeHiddenInput("ixAttrCount", ixAttrCount.ToString()));
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
