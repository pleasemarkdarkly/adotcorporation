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
	/// Summary description for Command.
	/// </summary>
	/// 
	public class getItemDetail : Command
	{
		public getItemDetail() {}
		public override string Perform() 
		{
			
			try
			{
				string _sScan;
				int _lExplicitItemID;
				XmlDocument _xmldMeta;
				//MMDModel model = new MMDModel(this.Context, this.ControllerContext);
				//bool _bIsExplicit = false;

				_sScan = Context.Request.QueryString["Scan"];
				if( _sScan == "" )	
					_sScan = null;
				int mmdErr;
			
				SampleInfo MMDSampleInfo = new SampleInfo( Context );
				if( _sScan != null )
				{
					//we've got a media scan
					mmdErr = MMDSampleInfo.GetMetaData( Context.LocationID, _sScan,  out _xmldMeta );
				}

				else
				{
					_lExplicitItemID = int.Parse( Context.Request.QueryString["ItemID"] );
					mmdErr = MMDSampleInfo.GetMetaData( Context.LocationID, _lExplicitItemID , false,  out _xmldMeta );
				}

				switch (mmdErr)
				{

					case 2302 : goto case 0;
					case 2303 : goto case 0;
					case 0 : break;
					case 1318 : goto case 1319;
					case 1319 : 
						Context.Log.Error(Context.LocationID, mmdErr, "called from getItemDetail");
						return "nomedia";
					case 2304: goto case 0;
						//MMDDevState.GetExplicitFlag( Context.LocationID, out _bIsExplicit );
						//break;
					default:
						Context.Log.Error(Context.LocationID, mmdErr, "called from getItemDetail");
						return ERROR;

					
				}
				XmlAttribute xma = _xmldMeta.SelectSingleNode("item/@type") as XmlAttribute;
				string sItemType = xma.Value;
				

				model.Item = _xmldMeta;


				this.ControllerContext.Model = model;

				
				if (sItemType == "game")
					return sItemType;
				if (sItemType == "video")
					return sItemType;

				string review = Context.Request.QueryString["review"];
				if (review == "true")
					return "review";
				else
					return SUCCESS;
			}
			catch (Exception e)
			{
				Context.Log.Error(Context.LocationID, e);
				return ERROR;
			
			}
		}

	}
	
}
