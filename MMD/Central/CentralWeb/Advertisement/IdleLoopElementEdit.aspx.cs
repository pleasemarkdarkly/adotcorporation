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

namespace MMD.Central.CentralWeb.Advertisement
{
	/// <summary>
	/// Summary description for IdleLoopElementEdit.
	/// </summary>
	public class IdleLoopElementEdit : System.Web.UI.Page
	{
		CSWContext _ctx;
		IdleLoop _idleloop = null;
		IdleLoopElement _idleLoopElement = null;

		public CSWContext CSWContext { get { return _ctx; } }
		public IdleLoopElement Element { get { return _idleLoopElement; } }
		public IdleLoop Loop { get { return _idleloop; } }
		
		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			int idIdleLoopElement = _ctx.Params.Int("idIdleLoopElement",0);
		
			if(idIdleLoopElement != 0)
			{
				_idleLoopElement = new IdleLoopElement(_ctx,idIdleLoopElement);
				_idleloop = new IdleLoop(_ctx,Element.idIdleLoop);
			}
			else
			{
				int idIdleLoop = _ctx.Params.Int("idIdleLoop",0);
				_idleloop = new IdleLoop(_ctx,idIdleLoop);
			}
		
		}

		public void MakeSequenceComboBox()
		{
			Response.Write(HTML.StartSelect("iSeq"));
			string sSelected = (Element != null) ? Element.iSeq.ToString() : "";

			for(int i=1;i<=Loop.Elements.Count;i++)
				Response.Write(HTML.MakeOption(i.ToString(),i.ToString() + " - " + Loop.Elements[i-1].sName,sSelected));
			
			if(Element == null)
			{
				int iNewSeq = Loop.Elements.Count + 1;
				Response.Write(HTML.MakeOption(iNewSeq.ToString(),"New",iNewSeq.ToString()));
			}
			
			Response.Write(HTML.EndSelect());
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