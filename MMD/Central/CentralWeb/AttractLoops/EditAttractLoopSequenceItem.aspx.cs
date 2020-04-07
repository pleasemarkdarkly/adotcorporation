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
using MMD.Central.CentralLib.XmlObjects;
using MMD.Central.CentralLib.ListView;

namespace MMD.Central.CentralWeb.AttractLoops
{
	/// <summary>
	/// Summary description for EditAttractLoopSequenceItem.
	/// </summary>
	public class EditAttractLoopSequenceItem : System.Web.UI.Page
	{
		CSWContext _ctx;
		IdleLoopPersister _ilp;
		IdleLoopElement _ile;
		IdleLoop _il;

		int _idIdleLoop;
		int _idIdleLoopElement;
		int _iFrameType;
		int _idFile;

		public CSWContext CSWContext { get { return _ctx; } }

		private void Page_Load(object sender, System.EventArgs e)
		{
			_ctx = CSWContext.Get( this, this.Context );
			
			_idIdleLoop = _ctx.Params.SessionInt("idIdleLoop");
			_idIdleLoopElement = _ctx.Params.Int("idIdleLoopElement",0);
			_iFrameType = _ctx.Params.Int("iFrameType",0);
			_idFile = _ctx.Params.Int("idFile",0);

			_ilp = new IdleLoopPersister(_ctx,false);
			_il = _ilp.GetIdleLoopById(_idIdleLoop);

			if(_idIdleLoopElement == 0)
			{
				_ile = IdleLoopElement.CreateNewInstance((FrameType)_iFrameType);
				
				if(_idFile != 0)
				{
					_ile.Files = new MMDFile[1];
					DataRow row = _ilp.DataSet.Tables["File"].Rows.Find(_idFile);
					_ile.Files[0] = new MMDFile(row,_ilp.DataSet,_ctx);
				}
			}
			else
			{
				_ile = _ilp.GetIdleLoopElementById(_idIdleLoopElement);
			}
		}

		

		public IdleLoopElement idleLoopElement 	{ get { return _ile; 	} 	}
		public IdleLoopElement ile { get { return _ile; } }

		public IdleLoop idleLoop { get { return _il; } }
		public int idFile { get { return _idFile; } }
        public IdleLoop il { get { return _il; } }

		public string LoopItemName
		{
			get
			{
				if(ile.idIdleLoopElement != 0)
					return ile.sName;
				else if(ile is BaseRecListFrame)
					return "";
				else if(ile.Files != null && ile.Files.Length != 0)
					return ile.Files[0].sName;
				else
					return "New Attract Loop Item";
			}
		}

		public string FileLink(string sFileName)
		{
			return _ctx.Config.GetWebPath(_il.idStoreConcept,MMDPostedFileType.IdleLoopElement,sFileName);
		}

		public string SafeString(object o, object oDefault)
		{
			return (o != null) ? o.ToString() : ((oDefault != null) ? oDefault.ToString() : "");
		}

		public string RecListTypeComboBox(BaseRecListFrame rlFrame)
		{
			string sSQL = "";
			string sSelectedKey = "";
			if(rlFrame is RecListFrame)
			{
				
				sSQL = "select id, display from vwCSW_AttractLoopItemRecList where idRecListBaseType = 1 and idStoreConcept in (0, " + _il.idStoreConcept + ")";
				sSelectedKey = rlFrame.idRecListType.ToString() + "," + ((RecListFrame)rlFrame).idProductType;
			}
			else if(rlFrame is PromotionListFrame)
			{
				sSQL = "select id, display from vwCSW_AttractLoopItemRecList where idRecListBaseType = 2 and idStoreConcept in (0, " + _il.idStoreConcept + ")";
				sSelectedKey = rlFrame.idRecListType.ToString();
			}
			LookupTable lt = new LookupTable(_ctx.Config.MainDatabase,sSQL);
            return lt.GetHTMLComboBox("idRecListType",sSelectedKey,true,true);
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
