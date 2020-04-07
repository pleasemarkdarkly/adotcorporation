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

using MMD.Store.StoreLib;
using MMD.Store.StoreLib.Data;
using MMD.Store.StoreLib.Utilities;



using Maverick.Ctl;

namespace Fullplay.Ctl
{
	/// <summary>
	/// Summary description for Search.
	/// </summary>
	public class Search : Command
	{
		public Search() {}
		public override string Perform()
		{
			/*
			 * get data connection
			 * perform search 
			 * build search results xml
			 * add search results xmld to model 
			 * return happy
			 */
		
		
//					int idMediaType = int.Parse(Context.Request.QueryString["mediatype"]) ;
//					string sSearchval = Context.Request.QueryString["searchval"];
//
//					bool bFilterArtist = bool.Parse(Context.Request.QueryString["artist"]);
//					bool bFilterAlbum  = bool.Parse(Context.Request.QueryString["album"]);
//					bool bFilterTrack  = bool.Parse(Context.Request.QueryString["track"]);

		
				
		

			XmlDocument xmldSearch = new XmlDocument(); 

			int iStartSeq;
			int iCount;
			try 
			{
				iStartSeq = int.Parse(Context.Request.QueryString["startseq"]);
				iCount = int.Parse(Context.Request.QueryString["count"]) ;
			}
			catch (ArgumentNullException)
			{
				iStartSeq=1;
				iCount = 0;
			}
			

			try 
			{
				int idMediaType = int.Parse(Context.Request.QueryString["mediatype"]) ;
				string sSearchval = Context.Request.QueryString["searchval"];

				bool bFilterArtist = bool.Parse(Context.Request.QueryString["artist"]);
				bool bFilterAlbum  = bool.Parse(Context.Request.QueryString["album"]);
				bool bFilterTrack  = bool.Parse(Context.Request.QueryString["track"]);


				int idSearchCache = getSearchCacheID(idMediaType, sSearchval, bFilterAlbum, bFilterArtist, bFilterTrack );
				getResultsFromCache(idSearchCache, out xmldSearch, iCount , iStartSeq);

				MMDModel model = new MMDModel(this.Context, this.ControllerContext);
				model.Search = xmldSearch;
				ControllerContext.Model = model;

				if (Context.Request.QueryString["content"]!=null &&Context.Request.QueryString["content"] == "true")
					return "content";

				return SUCCESS;
			}
			catch (Exception)
			{
				ControllerContext.Model = this.model;
				return SUCCESS;

			}

		}
		private int  getSearchCacheID(int idMediaType, string sSearchVal, bool bFilterAlbum, bool bFilterArtist, bool bFilterTrack)
		{
		
			
			int idSearchCache;
			MMD.Store.StoreLib.Data.Parameters  cParams = new MMD.Store.StoreLib.Data.Parameters();

			cParams.Add("@idMediaType", DbType.Int32, ParameterDirection.Input, idMediaType);
			cParams.Add("@bFilterAlbum", DbType.Boolean, ParameterDirection.Input, bFilterAlbum);
			cParams.Add("@bFilterArtist", DbType.Boolean, ParameterDirection.Input, bFilterArtist);
			cParams.Add("@bFilterTrack", DbType.Boolean, ParameterDirection.Input, bFilterTrack);
			cParams.Add("@sSubString", DbType.String, ParameterDirection.Input, sSearchVal);
			cParams.Add("@idSearchCache", DbType.Int32, ParameterDirection.InputOutput, 0 );



			cParams = GetDataConnection().execSP_OutputParams("spr_MMDSearchCheckForCache",cParams);
			idSearchCache = (int)cParams[5].Value;

			
			
			return idSearchCache;

		}
		private void getResultsFromCache(int idSearchCache, out XmlDocument xmldResults, int iCount, int iStartSeq)
		{
			MMD.Store.StoreLib.Data.Parameters  cParams = new MMD.Store.StoreLib.Data.Parameters();
			cParams.Add("@iSearchCache", DbType.Int32, ParameterDirection.Input, idSearchCache);
			cParams.Add("@iCount", DbType.Int32, ParameterDirection.Input, iCount);
			cParams.Add("@SeqStart", DbType.Int32, ParameterDirection.Input, iStartSeq);
			MMD.Store.StoreLib.Data.NamedFieldReader nfr = GetDataConnection().execSP_NFR("spr_MMDSearchGetCachedItems", cParams);
			
			xmldResults = new XmlDocument();
			XmlElement xmleSearchList = (XmlElement) xmldResults.AppendChild(xmldResults.CreateElement("searchlist"));
			if (nfr.Read())
			{
				SearchList.ImportDataReader(xmldResults, xmleSearchList, nfr, this.Context);
			}

		}
		
		

		private MMD.Store.StoreLib.Data.Connection GetDataConnection()
		{
			return Context.Config.StoreDataConnection;
		}

	}
}
