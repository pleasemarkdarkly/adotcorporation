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

//using MMD.Store.StoreLib.Utilities;
//using MMD.Store.StoreLib.Commands;
using Fullplay.Lib;
using MMD.Store.StoreLib;

using Maverick.Ctl;



namespace Fullplay.Ctl
{
	/// <summary>
	/// Summary description for getReclist.
	/// </summary>
	public class getReclist : Command
	{

		int mmdErr;
		XmlDocument xmldList;
		int iProductType;
		int iGenre;
		int iReclistType;
		
		//Suggestions MMDSuggestions;

		

		
		public XmlDocument getRecListXml() {return xmldList;}
		public getReclist() {}

		public override string  Perform()
		{
			xmldList = null;
			mmdErr = MMDError.Success;

			try 
			{
				iProductType = int.Parse(ControllerContext.HttpContext.Request.QueryString["idproducttype"]);
				//iGenre = int.Parse(ControllerContext.HttpContext.Request.QueryString["idgenre"]);
				iReclistType = int.Parse(ControllerContext.HttpContext.Request.QueryString["idreclisttype"]);
				model.ProductID = iProductType;
				model.ReclistID = iReclistType;

			
			}
			catch (Exception e)
			{
				Context.Log.Error(Context.LocationID, e);
				return ERROR;
			}
			try 
			{
				Reclist r = new Reclist(Context);
				xmldList = r.getRecomendations(iReclistType, Context.Config.StoreID, iProductType);
				model.Reclist = xmldList;
				
				ControllerContext.Model = model;
				return SUCCESS;

//				if (iReclistType == 3)
//				{
//					switch (iProductType)
//					{
//						case (1): return "NewReleases";
//						case (3): return "NewReleases_Movies";
//						case (4): return "NewReleases_Games";
//
//						default: break;
//					}
//				
//					
//				}
//				if (iReclistType == 81)
//				{
//					switch (iProductType)
//					{
//						case (1):
//							return "On_Sale";
//						case (3): return "On_Sale_Movies";
//						case (4): return "On_Sale_Games";
//						default: break;
//					}
//				}
//
//				if (iReclistType == 1)
//				{
//					switch (iProductType)
//					{
//						case (1):
//							return "Top_Sellers";
//						case (3): return "Top_Sellers_Movies";
//						case (4): return "Top_Sellers_Games";
//						default: break;
//	
//					}
//				}

//				if (iReclistType == 81)
//				{
//					switch (iProductType)
//					{
//						case (1):
//							return "On_Sale";
//						case (3): return "On_Sale_Movies";
//						case (4): return "On_Sale_Games";
//						default: break;
//	
//					}
//				}

				//return ERROR;
	
			}
			catch (Exception e )
			{
				Context.Log.Error(Context.LocationID, e);	
				return ERROR;
			
			}
			
		}

	}
}
		
	
	

