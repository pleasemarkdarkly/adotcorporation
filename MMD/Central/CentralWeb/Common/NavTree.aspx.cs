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
using System.Xml;
using MMD.Central.CentralLib;
using MMD.Central.CentralLib.Utilities;

namespace MMD.Central.CentralWeb.Common
{
	/// <summary>
	/// Summary description for NavTree.
	/// </summary>
	public class NavTree : System.Web.UI.Page
	{
		CSWContext _ctx;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
		}

		public void DoNavTree()
		{
			XmlDocument xmld = new XmlDocument();
			xmld.Load(Server.MapPath("/CentralWeb/Common/Xml/navtree.xml"));

			DisplayPage(xmld.DocumentElement);
		}

		public void DisplayPage(XmlElement xmlePage)
		{
			string sTag = CentralLib.Utilities.Xml.GetAttribute(xmlePage,"tag","");
			string sDisplay = CentralLib.Utilities.Xml.GetAttribute(xmlePage,"display","");
			string sURL = CentralLib.Utilities.Xml.GetAttribute(xmlePage,"url","");
			bool bHasChildren = xmlePage.FirstChild != null;

			Response.Write("<div class=\"NavLink\">");
			if(sURL != "")
			{
				Response.Write("<a href=\"" + sURL + "\">" + sDisplay + "</a>");
			}
			else
				Response.Write(sDisplay);
			
			Response.Write("</div>");
			
			if(bHasChildren)
			{
				Response.Write("<div class=\"NavChildren\">");
				foreach(XmlNode xmln in xmlePage.ChildNodes)
					DisplayPage((XmlElement)xmln);
				Response.Write("</div>");
			}
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
