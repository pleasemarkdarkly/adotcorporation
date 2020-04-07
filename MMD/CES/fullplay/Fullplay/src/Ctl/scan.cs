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
//using MMD.Store.StoreLib.Commands;
using MMD.Store.StoreLib;

using Maverick.Ctl;

namespace Fullplay.Ctl
{
	/// <summary>
	/// Summary description for scan.
	/// </summary>
	public class scan : Command
	{

		public const string  ADMIN = "admin";
		public const string LOGIN = "login";
		public const string ITEM_DETAIL = "itemdetail";


		WebContext ctx;
		DeviceState MMDDevState;
		public scan() 
		{
			
		}

		public override string  Perform()
		{
			ctx = new WebContext(this.ControllerContext.HttpContext);

			MMDDevState = new DeviceState(ctx);
				
			Hashtable model = new Hashtable();
			UPCClass eUPCClass;
			//set the model with an IDictionary for redirect view
			foreach (string s in ControllerContext.HttpContext.Request.QueryString.AllKeys)
			{
				model.Add(s, ControllerContext.HttpContext.Request.QueryString[s]);
			}
			ControllerContext.Model = model;

			string sUPC = ControllerContext.HttpContext.Request.QueryString["Scan"];

			int mmdErr = this.MMDDevState.ClassifyUPC(sUPC,out eUPCClass);
			switch(eUPCClass)
			{
				case UPCClass.Customer :
					return LOGIN;
				case UPCClass.Product :
					//if (MMDDevState.LocationType == LocationType.Hardsell)
						//return HARDSELL_ITEMDETAIL;
					return ITEM_DETAIL;	
				case UPCClass.Admin :

					SessionType eSessionType, eAdminLevel;
					DateTime dtSessionStartTime;
					DeviceStatus eDeviceStatus;
					mmdErr = MMDDevState.GetSessionInfo(Context.LocationID,out eSessionType,out dtSessionStartTime,out eDeviceStatus);
					mmdErr = MMDDevState.VerifyAdminScan(sUPC, out eAdminLevel);
					if(eSessionType == SessionType.AdminScan || eSessionType == SessionType.AdminFull)
					{
						mmdErr = MMDDevState.TerminateSession(Context.LocationID);
						if(eSessionType != eAdminLevel)
							mmdErr = MMDDevState.AdminLogin(Context.LocationID, sUPC);
						else
							mmdErr = MMDDevState.AdminLogin(Context.LocationID, sUPC);
						return ADMIN;
					}
					else
						mmdErr = MMDDevState.AdminLogin(Context.LocationID,sUPC);

					if(mmdErr == 1002 && Context.LocationID == 0) //bad LocationID
						mmdErr = 0;
						
					if(mmdErr == 0)
						return ADMIN;
					else
						return ERROR;

					
				default:
					model.Clear();
					return ERROR;

		
			}

		}
	}
}

		