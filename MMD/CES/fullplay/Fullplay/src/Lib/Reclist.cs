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
using MMDData = MMD.Store.StoreLib.Data;
using MMD.Store.StoreLib.Utilities;


namespace Fullplay.Lib
{
	/// <summary>
	/// Summary description for Reclist.
	/// </summary>
	public class Reclist : StoreLib.WebObjectBase
	{

		StoreLib.WebContext context;
		public Reclist(StoreLib.WebContext context): base(context) 
		{ 
			this.context = context; 
		}


		public XmlDocument getRecomendations(int iReclistType, int idStoreConcept, int idProductType)
		{

			XmlDocument xmlItems = getRecommendationItems(iReclistType, idStoreConcept, idProductType);
			XmlDocument xmlAttributes = getRecommendationAttributes(iReclistType, idStoreConcept, idProductType);

			if (xmlAttributes != null)

			
			{

				XmlNode AttributeRoot = xmlAttributes.DocumentElement;
				XmlNode AttributeNode = xmlItems.ImportNode(AttributeRoot.SelectSingleNode("/reclist//Attributes"), true);
				xmlItems.DocumentElement.InsertBefore(AttributeNode, xmlItems.DocumentElement.FirstChild);
				XmlNodeList items = xmlItems.SelectNodes("//item");
				IEnumerator i = items.GetEnumerator();
				while (i.MoveNext())
				{
					XmlNode xn = (XmlNode)i.Current;
					XmlAttribute xmlaIdReclist = (XmlAttribute)xn.SelectSingleNode("@idReclistItem");
					string sReclist = xmlaIdReclist.Value;
					try 
					{
						if (AttributeRoot.SelectSingleNode("//Item[@id="+sReclist+"]/Attributes") != null && 
							AttributeRoot.SelectSingleNode("//Item[@id="+sReclist+"]/Attributes").HasChildNodes)
							xn.InsertBefore(xmlItems.ImportNode(AttributeRoot.SelectSingleNode("//Item[@id="+sReclist+"]/Attributes"), true), xn.FirstChild);
					}
						//the node is null; shouldnt import anyway - but we can eat it for now
					catch (InvalidOperationException e) {Context.Log.Error(Context.LocationID, e);}
				}
			}

			return xmlItems;


		}

		XmlDocument getRecommendationItems(int iReclist, int idStoreConcept, int idProductType)
		{
			MMDData.Parameters cParams = new MMDData.Parameters();
			cParams.Add("@idRecListType", DbType.Int32, ParameterDirection.Input, iReclist);
			cParams.Add("@idStoreConcept", DbType.Int32, ParameterDirection.Input, idStoreConcept);
			cParams.Add("@idProductType", DbType.Int32, ParameterDirection.Input, idProductType);		
			MMDData.NamedFieldReader nfr = GetDataConnection().execSP_NFR("spr_MMDGetPopMetaDataByReclist", cParams);
			XmlDocument xmldList = new XmlDocument();
			XmlElement xmleItemList = (XmlElement) xmldList.AppendChild( xmldList.CreateElement("itemlist") );

			xmleItemList.SetAttribute("idReclist", iReclist.ToString());
			xmleItemList.SetAttribute("idProductType",idProductType.ToString());

			if (nfr.Read())
				return 	BuildResultsXml(nfr, xmldList, xmleItemList);
			else
				return null;




		
		}

		XmlDocument getRecommendationAttributes(int iReclist, int idStoreConcept, int idProductType)
		{
			MMDData.NamedFieldReader nfr = null;
			try 
			{
				MMDData.Parameters cParams = new MMDData.Parameters();
				cParams.Add("@idRecListType", DbType.Int32, ParameterDirection.Input, iReclist);
				cParams.Add("@idStoreConcept", DbType.Int32, ParameterDirection.Input, idStoreConcept);
				cParams.Add("@idProductType", DbType.Int32, ParameterDirection.Input, idProductType);		
				nfr = GetDataConnection().execSP_NFR("spr_MMDGetRecommendationListAttributes", cParams);

				if (nfr.Read())
				{
					return 	BuildAttributeXml(nfr);
				}
				else
					return null;
			}
			
			
			catch (Exception)
			{
				return null;

			}

			finally 
			{
				if (nfr != null)
				{
					nfr.Close();
				}
			}

		
		}

		private XmlDocument BuildAttributeXml(MMDData.NamedFieldReader nfr)
		{

			

			XmlDocument xmld = null;
			xmld = new XmlDocument();
			XmlElement xmleList = (XmlElement)xmld.AppendChild(xmld.CreateElement("reclist"));
			XmlElement xmleAttrs = Xml.AppendChildElement(xmld, xmleList, "Attributes");
			XmlElement xmleItems = Xml.AppendChildElement(xmld, xmleList, "Items");
			XmlElement xmleItemAttrbutes = null; 
			XmlElement xmlAttr = null;

			do 
			{
				if (nfr.IsDBNull("idReclistItem"))
				{
					xmlAttr = Xml.AppendChildElement(xmld, xmleAttrs, "Attribute");
					xmlAttr.SetAttribute("name", nfr.SafeGetString("AttrName"));
					xmlAttr.InnerText = nfr.SafeGetString("AttrValue");
				}
			} while (nfr.Read() && nfr.IsDBNull("idReclistItem"));

			
			do
			{
				
				XmlElement xmlItem = Xml.AppendChildElement(xmld, xmleItems, "Item");
				xmlItem.SetAttribute("id", nfr.GetInt32("idReclistItem").ToString());
				xmleItemAttrbutes  = Xml.AppendChildElement(xmld, xmlItem, "Attributes");
				XmlElement xmlItemAttrs = Xml.AppendChildElement(xmld, xmleItemAttrbutes, "Attribute");
				xmlItemAttrs.SetAttribute("name", nfr.SafeGetString("AttrName"));
				xmlItemAttrs.InnerText = nfr.SafeGetString("AttrValue");
					
			}  while (nfr.Read());

			if (nfr != null)
				nfr.Close();
			
			return xmld;

		
		
		}

		private XmlDocument BuildResultsXml(MMDData.NamedFieldReader dr, XmlDocument xmldList, XmlElement xmleItemList)
		{
			int lItemID = 0;
			XmlElement xmleItem = null;

			do 
			{
				if(lItemID != dr.GetInt32("idItem") )
				{
					
					StoreLib.ProductType eProductType = (StoreLib.ProductType) dr.GetInt32("iType");
					string sProductType = "";
					switch(eProductType)
					{
						case StoreLib.ProductType.Pop : sProductType = "pop"; break;
						case StoreLib.ProductType.Classical : sProductType = "classical"; break;
						case StoreLib.ProductType.Video : sProductType = "video"; break;
						case StoreLib.ProductType.Game : sProductType = "game"; break;
					}


					lItemID = dr.GetInt32("idItem");
					xmleItem = (XmlElement) xmleItemList.AppendChild( xmldList.CreateElement("item") );
					xmleItem.SetAttribute("idReclistItem", dr.SafeGetInt32("idReclistItem",-1).ToString());
					xmleItem.SetAttribute("idItem", lItemID.ToString() );
					xmleItem.SetAttribute("idProduct", dr.SafeGetInt("idProduct",-1).ToString());
					xmleItem.SetAttribute("mediatype", dr.SafeGetString("sMediaType"));
					


					//fix later
					Xml.AppendChildElementWithValue( xmldList, xmleItem, "artistinfo", dr.SafeGetString("a_sArtistName") );
					Xml.AppendChildElementWithValue( xmldList, xmleItem, "director", dr.SafeGetString("v_sDirector") );
				
					

					Xml.AppendChildElementWithValue( xmldList, xmleItem, "producttype", sProductType);
					Xml.AppendChildElementWithValue( xmldList, xmleItem, "platform", dr.SafeGetString("g_sPlatform","") );
						

					switch(eProductType)
					{
						case StoreLib.ProductType.Pop : goto case StoreLib.ProductType.Classical;
						case StoreLib.ProductType.Classical :
							Xml.AppendChildElementWithValue( xmldList, xmleItem, "coverartsmallurl", dr.SafeGetString("sCoverArtURLSmall",context.Config.DefaultMusicImageSmall) );
							Xml.AppendChildElementWithValue( xmldList, xmleItem, "coverartlargeurl", dr.SafeGetString("scoverArtURLLarge",context.Config.DefaultMusicImageLarge) );
							break;
						case StoreLib.ProductType.Video : sProductType = "video"; 
							Xml.AppendChildElementWithValue( xmldList, xmleItem, "coverartsmallurl", dr.SafeGetString("sCoverArtURLSmall",context.Config.DefaultVideoImageSmall) );
							Xml.AppendChildElementWithValue( xmldList, xmleItem, "coverartlargeurl", dr.SafeGetString("scoverArtURLLarge",context.Config.DefaultVideoImageLarge) );
							break;
						case StoreLib.ProductType.Game : sProductType = "game"; 
							Xml.AppendChildElementWithValue( xmldList, xmleItem, "coverartsmallurl", dr.SafeGetString("sCoverArtURLSmall",context.Config.DefaultGameImageSmall) );
							Xml.AppendChildElementWithValue( xmldList, xmleItem, "coverartlargeurl", dr.SafeGetString("scoverArtURLLarge",context.Config.DefaultGameImageSmall) );
							break;
					}
					//Xml.AppendChildElementWithValue( xmldList, xmleItem, "coverartsmallurl", dr.SafeGetString("sCoverArtURLSmall","no_image_found.bmp") );
					//Xml.AppendChildElementWithValue( xmldList, xmleItem, "coverartlargeurl", dr.SafeGetString("scoverArtURLLarge","no_image_found.bmp") );
					Xml.AppendChildElementWithValue( xmldList, xmleItem, "title", dr.SafeGetString("sTitle") );

					//TODO - change from tblMMD_MetaDataItem.sFormat to tblMMD_MetaDataMediaType.sName, and do so for the following sprocs
					//	spr_MMDGetWishList, spr_MMDGetFeaturedList, spr_MMDGetRecommendList, spr_MMDGetAffinityList
					//Xml.AppendChildElementWithValue( xmldList, xmleItem, "config", dr.SafeGetString("sFormat") );

					Xml.AppendChildElementWithValue( xmldList, xmleItem, "saleflag", "false" );
					Xml.AppendChildElementWithValue( xmldList, xmleItem, "price", "" );
					Xml.AppendChildElementWithValue( xmldList, xmleItem, "releasedate", dr.IsDBNull("dtRelease")?"":dr.GetDateTime("dtRelease").ToShortDateString() );
					Xml.AppendChildElementWithValue( xmldList, xmleItem, "UPC", dr.SafeGetString("sUPC") );
				}
				else if(xmleItem != null)
				{
					Xml.AppendChildElementWithValue( xmldList, xmleItem, "UPC", dr.SafeGetString("sUPC") );
				}
			} while(dr.Read());
			
			if (dr != null)
				dr.Close();


			return xmldList;

		
		}


		private MMDData.Connection GetDataConnection()
		{
			return context.Config.StoreDataConnection;
		}
		
	}
}
