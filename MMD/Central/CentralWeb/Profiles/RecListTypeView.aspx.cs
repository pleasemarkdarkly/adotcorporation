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
	/// Summary description for RecListTypeView.
	/// </summary>
	public class RecListTypeView : System.Web.UI.Page
	{
		CSWContext _ctx;
		RecListTypePersister _rltp;
		RecListType[] _rlts;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			_rltp = new RecListTypePersister(_ctx,false,Location.Staging);
			_rlts = _rltp.GetRecListTypes();
		}


		public RecListType[] GetRecListTypes()
		{
			return _rlts;
		}

		public string GetStatus(RecListType rlt)
		{
			int iStatus = Convert.ToInt32(rlt.byStatus);
			StagingStatus status = (StagingStatus)iStatus;
			return status.ToString();
		}
		
		public string Edit(RecListType rlt)
		{
			// check how many rec lists are of this type
			int cRecLists = _ctx.Config.MainDatabase.DoCountQuery("tblMMD_RecommendListStaging where idRecListType = " + rlt.idRecListType);

			// if count is zero & status is not deleted, you can edit
			if((cRecLists == 0) && (rlt.byStatus != StagingStatus.Deleted))
			{
				return "<a href=\"/CentralWeb/Profiles/RecListTypeEdit.aspx?idRecListType=" + rlt.idRecListType + "\">Edit</a>";
			}
			else
				return "";
		}

		public string Delete(RecListType rlt)
		{
			// check how many rec lists are of this type
			int cRecLists = _ctx.Config.MainDatabase.DoCountQuery("tblMMD_RecommendListStaging where idRecListType = " + rlt.idRecListType);
            int iStatus = Convert.ToInt32(rlt.byStatus);

			if(cRecLists == 0 && (iStatus == (int)StagingStatus.Unchanged  || iStatus == (int)StagingStatus.Changed))
				return "<a href=\"javascript:ConfirmLink('Are you sure you want to delete this recommendation list type?','/CentralWeb/Profiles/RecListTypeView.aspx?cmd=RecListTypeDelete&idRecListType=" + rlt.idRecListType + "')\">Delete</a>";
			else 
				return cRecLists + "&nbsp;Recommend&nbsp;Lists";
		}
		
		public string Undo(RecListType rlt)
		{
			if(Convert.ToInt32(rlt.byStatus) != (int)StagingStatus.Unchanged)
			{
				return "<a href=\"javascript:ConfirmLink('Are you sure you want to undo changes to this recommendation list type?  If this recommendation list type is New it will be permanently deleted.','/CentralWeb/Profiles/RecListTypeView.aspx?cmd=RecListTypeUndo&idRecListType=" + rlt.idRecListType + "')\">Undo</a>";
			}
			else
				return "";
		}

		public string Submit(RecListType rlt)
		{
			if(Convert.ToInt32(rlt.byStatus) != (int)StagingStatus.Unchanged)
			{
				return "/CentralWeb/Profiles/RecListTypeView.aspx?cmd=RecListTypeSubmit&idRecListType=" + rlt.idRecListType;
			}
			else
				return "";
		}

		public string SubmitAll()
		{
			return "<a href=\"javascript:ConfirmLink('Are you sure you want to submit all changes to the recommendation list types?','/CentralWeb/Profiles/RecListTypeView.aspx?cmd=RecListTypeSubmitAll')\">Submit All Changes</a>";
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
