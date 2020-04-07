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


namespace MMD.Central.CentralWeb.FeatureRequest
{
	/// <summary>
	/// Summary description for ViewFeatureRequests.
	/// </summary>
	public class ViewFeatureRequests : System.Web.UI.Page
	{
		CSWContext _ctx;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
		}

		public void DoListView()
		{
			ListView lv = new ListView(_ctx, "ViewFeatureRequests","feature_requests", _ctx.LoadListViewFromSession , false);
		
			lv.AddColumn(new FieldLinkColumn(lv, "sXref", "XREF", "/CentralWeb/FeatureRequest/EditFeatureRequest.aspx?idFeatureRequest=%idFeatureRequest%"));
			lv.AddColumn(new FieldColumn(lv, "sTitle", "Title"));
			lv.AddColumn(new FieldColumn(lv, "sArtist", "Artist/Director"));
			lv.AddColumn(new FieldColumn(lv, "sAuthor", "Author"));
			lv.AddColumn(new FieldColumn(lv, "dtChanged", "Updated"));
			lv.AddColumn(new FieldColumn(lv, "sChangedBy", "Updated By"));
			lv.AddColumn(new ShortcutColumn(lv, "Remove", "/CentralWeb/FeatureRequest/ViewFeatureRequests.aspx?cmd=FeatureRequestRemove&idFeatureRequest=%idFeatureRequest%"));
			
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
