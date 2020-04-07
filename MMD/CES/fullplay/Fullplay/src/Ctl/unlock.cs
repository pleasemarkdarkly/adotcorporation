using System;
using System.Collections;
using System.Data;
using System.Data.SqlClient;
using System.Reflection;
using System.Web;
using System.Web.UI;
using System.Web.SessionState;
using System.Xml;
using System.Xml.Serialization;
using System.Xml.XPath;
using System.Xml.Xsl;
using MMD.Store.StoreLib.Utilities;



using MMD.Store.StoreLib;
using Maverick.Ctl;

namespace Fullplay.Ctl
{
	/// <summary>
	/// Summary description for unlock.
	/// </summary>
	public class unlock : Command
	{
		string _sErrorMessage = null;
		public unlock() {}
		public override string Perform ()
		{
			
			SessionType eAdminLevel;
			int mmdErr;

			DeviceState MMDDevState = new DeviceState( Context );
			MMDDevState.TerminateSession( Context.LocationID );
			mmdErr = MMDDevState.VerifyAdminScan( Context.Request.QueryString["Scan"], out eAdminLevel);

			//set the model with an IDictionary for redirect view
			Hashtable model = new Hashtable();
			//UPCClass eUPCClass;

			foreach (string s in ControllerContext.HttpContext.Request.QueryString.AllKeys)
			{
				model.Add(s, ControllerContext.HttpContext.Request.QueryString[s]);
			}
			ControllerContext.Model = model;

			switch(eAdminLevel)
			{
				
				

				case SessionType.AdminFull :
					return SUCCESS;
				case SessionType.AdminScan :
					return SUCCESS;
				default:
					_sErrorMessage = Msg.Unlock_Cur_Unavailable;
					break;
			}
			return ERROR;
		}
	}
}
