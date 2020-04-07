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

using StoreLib = MMD.Store.StoreLib;
using MMD.Store.StoreLib.Data;
using MMD.Store.StoreLib.Utilities;

using Fullplay.Ctl;


namespace Fullplay.Lib
{
	/// <summary>
	/// Summary description for Browse.
	/// </summary>
	public class Browse : Command
	{

		new StoreLib.WebContext Context;
		
		public Browse(StoreLib.WebContext ctx)
		{
			Context = ctx;
		}


		public XmlDocument getGames(int idGenre, int index)
		{
			XmlDocument xmld = null;
			StoreLib.Data.NamedFieldReader nfr = null;
			StoreLib.Data.Parameters cParams = new StoreLib.Data.Parameters();
			cParams.Add("@idGenre", DbType.Int32, ParameterDirection.Input, idGenre);
			cParams.Add("@idProdcutType", DbType.Int32, ParameterDirection.Input, 4);
			cParams.Add("@index", DbType.Int32, ParameterDirection.Input, index);
			nfr = GetDataConnection().execSP_NFR("spr_MMDGetGamesByGenre", cParams);

			if (nfr.Read())
			{
				xmld = BuildGameXml(nfr);
				return xmld;
			}
			return null;

		
		}



		
		public XmlDocument getGenres(int iMediaType)
		{
			XmlDocument xmld = null;
			StoreLib.Data.NamedFieldReader nfr = null;

			StoreLib.Data.Parameters cParams = new StoreLib.Data.Parameters();
			cParams.Add("@idMediaType", DbType.Int32, ParameterDirection.Input, iMediaType);
			nfr = GetDataConnection().execSP_NFR("spr_MMDGetGenreByMediaType", cParams);
			
			if (nfr.Read())
			{
				xmld = BuildGenreXml(nfr);
				return xmld;
			}
			
			else
				return null;

		}



		public XmlDocument getArtists(int idGenre, int idMediaType,int index)
		{
			XmlDocument xmld = null;
			StoreLib.Data.NamedFieldReader nfr = null;

			StoreLib.Data.Parameters cParams = new StoreLib.Data.Parameters();
			cParams.Add("@idGenre", DbType.Int32, ParameterDirection.Input, idGenre );
			cParams.Add("@idMediaType", DbType.Int32, ParameterDirection.Input, idMediaType);
			cParams.Add("@index", DbType.Int32, ParameterDirection.Input, index);

			nfr = GetDataConnection().execSP_NFR("spr_MMDGetContributorNameByGenreMedia",cParams);

			if (nfr.Read())
			{
				xmld = BuildArtistXml(nfr, index, idGenre);
				return xmld;
			
			}
			
			else 
				xmld = new XmlDocument();
				XmlElement xmle = (XmlElement) xmld.AppendChild(xmld.CreateElement("artists"));
				xmle.SetAttribute("genreid", idGenre.ToString());
				xmle.SetAttribute("index", index.ToString());
				return xmld;

		}



		public XmlDocument getDiscography(string sArtistName)
		{
			XmlDocument xmld = null;
			StoreLib.Data.NamedFieldReader nfr = null;
			StoreLib.Data.Parameters cParams = new StoreLib.Data.Parameters();
			cParams.Add("@sArtistName", DbType.String, ParameterDirection.Input, sArtistName );
			nfr = GetDataConnection().execSP_NFR("spr_MMDGetDiscoByArtistName",cParams);
			if (nfr.Read())
			{
				xmld = BuildDiscoXml(nfr);
				return xmld;
			}
			
			else 
				return null;
		
		
		}

		
		private MMD.Store.StoreLib.Data.Connection GetDataConnection()
		{
			return this.Context.Config.StoreDataConnection;
		}

		private XmlDocument BuildDiscoXml(StoreLib.Data.NamedFieldReader nfr)
		{
			XmlDocument xmld = new XmlDocument();
			XmlElement xmle = (XmlElement) xmld.AppendChild(xmld.CreateElement("discography"));
			do
			{
				//StoreLib.Utilities.Xml.AppendChildElementWithValue(xmld, xmle, "item", nfr.SafeGetString("sContributorName"));
				XmlElement item = StoreLib.Utilities.Xml.AppendChildElement(xmld, xmle, "item");
				item.SetAttribute("id", nfr.SafeGetInt32("idItem", -1).ToString());
				StoreLib.Utilities.Xml.AppendChildElementWithValue(xmld, item, "sTitle", nfr.SafeGetString("sTitle"));
				StoreLib.Utilities.Xml.AppendChildElementWithValue(xmld, item, "coverartsmallurl", nfr.SafeGetString("sCoverArtURLSmall",Context.Config.DefaultMusicImageSmall));
				


			} while (nfr.Read());
			
			
			return xmld;



		}


		private XmlDocument BuildArtistXml(StoreLib.Data.NamedFieldReader nfr, int index, int genre)
		{
		
			XmlDocument xmld = new XmlDocument();
			XmlElement xmle = (XmlElement) xmld.AppendChild(xmld.CreateElement("artists"));
			xmle.SetAttribute("genreid", genre.ToString());
			xmle.SetAttribute("index", index.ToString());
			
			do
			{
				StoreLib.Utilities.Xml.AppendChildElementWithValue(xmld, xmle, "artistname", nfr.SafeGetString("sContributorName"));
			
			} while (nfr.Read());
			
			
			return xmld;


		
		}
		private XmlDocument BuildGenreXml(StoreLib.Data.NamedFieldReader nfr)
		{
			XmlDocument xmld = new XmlDocument();
			XmlElement xmle = (XmlElement) xmld.AppendChild(xmld.CreateElement("genres"));
			do 
			{
				XmlElement xl = StoreLib.Utilities.Xml.AppendChildElementWithValue(xmld, xmle, "genre", nfr.SafeGetString("sGenre"));
				xl.SetAttribute("idMediaType", nfr.SafeGetInt("idMediaType", -1).ToString());
				xl.SetAttribute("idGenre", nfr.SafeGetInt32("idGenre",-1).ToString());
				xl.SetAttribute("idProductType", nfr.SafeGetInt32("idProductType",-1).ToString());
				

			} while (nfr.Read());


			return xmld;

		
		}

		private XmlDocument BuildGameXml(StoreLib.Data.NamedFieldReader nfr)
		{
			XmlDocument xmld = new XmlDocument();
			XmlElement xmle = (XmlElement) xmld.AppendChild(xmld.CreateElement("genres"));
			do 
			{
				XmlElement xl = StoreLib.Utilities.Xml.AppendChildElementWithValue(xmld, xmle, "sTitle", nfr.SafeGetString("sTitle"));
				StoreLib.Utilities.Xml.AppendChildElementWithValue(xmld, xmle, "idItem", nfr.SafeGetInt32("idItem", -1).ToString());		

			} while (nfr.Read());


			return xmld;
		
		}


	}
}
