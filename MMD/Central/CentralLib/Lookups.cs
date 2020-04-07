using System;
using System.Diagnostics;
using System.Data;
using System.Data.SqlClient;
using System.Reflection;
using System.Web;
using System.Web.UI;
using System.Web.SessionState;
using System.Web.Services;
using System.Xml;
using MMD.Central.CentralLib.XmlObjects;
using MMD.Central.CentralLib.ListView;

namespace MMD.Central.CentralLib
{

	public class CSWLookups : CSWObjectBase
	{
		public CSWLookups(CSWContext ctx) : base(ctx) {}

		LookupTable _ltStoreConcept;
		public LookupTable StoreConcept
		{
			get 
			{
				if(_ltStoreConcept == null)
					_ltStoreConcept = new LookupTable(Context.Config.MainDatabase,"select idStoreConcept, sName from tblMMD_StoreConcept order by sName");
				return _ltStoreConcept;
			}
		}

		LookupTable _ltFileMediaType;
		public LookupTable FileMediaType
		{
			get 
			{
				if(_ltFileMediaType == null)
					_ltFileMediaType = new LookupTable(MMD.Central.CentralLib.XmlObjects.FileMediaType.Image.GetType());
				return _ltFileMediaType;
			}
		}

		LookupTable _ltMediaType;
		public LookupTable MediaType
		{
			get
			{
				if(_ltMediaType == null)
					_ltMediaType = new LookupTable(Context.Config.MainDatabase,"select idMediaType, sName from tblMMD_MetaDataMediaType where bUseInDevice = 1 order by sName");
				return _ltMediaType;
			}
		}

		public LookupTable Genre
		{
			get
			{
				return new LookupTable(Context.Config.MainDatabase,"select idGenre, sDescription from tblMMD_MetaDataGenre where bUseInDevice = 1 order by sDescription");
			}
		}

		LookupTable _ltProductType;
		public LookupTable ProductType
		{
			get
			{
				if(_ltProductType == null)
					_ltProductType = new LookupTable(Context.Config.MainDatabase,"select idProductType, sName from tblMMD_MetaDataProductType where idProductType <> 2 order by sName");
				return _ltProductType;
			}
		}

		LookupTable _ltYesNo;
		public LookupTable YesNo
		{
			get 
			{
				if(_ltYesNo == null)
					_ltYesNo = new LookupTable("1,Yes,0,No", ",");
				return _ltYesNo;
			}
		}

		LookupTable _ltEventType;
		public LookupTable EventType
		{
			get 
			{
				if(_ltEventType == null)
					_ltEventType = new LookupTable("0,Periodic,1,Daily,2,Run Once", ",");
				return _ltEventType;
			}
		}

		LookupTable _ltEventStatus;
		public LookupTable EventStatus
		{
			get 
			{
				if(_ltEventStatus == null)
					_ltEventStatus = new LookupTable("0,Disabled,1,Ready,2,Running", ",");
				return _ltEventStatus;
			}
		}

		LookupTable _ltPackageLevel;
		public LookupTable PackageLevel
		{
			get 
			{
				if(_ltPackageLevel == null)
					_ltPackageLevel = new LookupTable("1,Store,2,Central,3,Master", ",");
				return _ltPackageLevel;
			}
		}

		LookupTable _ltPackageContentType;
		public LookupTable PackageContentType
		{
			get 
			{
				if(_ltPackageContentType == null)
					_ltPackageContentType = new LookupTable("1,Database Table,2,Directory Tree", ",");
				return _ltPackageContentType;
			}
		}

		LookupTable _ltPackageDeltaStyle;
		public LookupTable PackageDeltaStyle
		{
			get 
			{
				if(_ltPackageDeltaStyle == null)
					_ltPackageDeltaStyle = new LookupTable("1,Delta,2,Replace", ",");
				return _ltPackageDeltaStyle;
			}
		}

		public LookupTable RecListType(int idRecListBaseType)
		{
			return new LookupTable(Context.Config.MainDatabase,"select idRecListType, sName from tblMMD_RecommendListType where idRecListBaseType = " + idRecListBaseType + " order by sName");
		}

		public LookupTable RecListFiles(int idStoreConcept)
		{
			return new LookupTable(Context.Config.MainDatabase,"select idFile, sName + ' (' + sFileName + ')' from tblMMD_File where idStoreConcept in (0, " + idStoreConcept + " ) and iType = 2 order by sName");
		}


		public LookupTable LocationMode(int idStoreConcept)
		{
			return new LookupTable(Context.Config.MainDatabase,"select idLocationMode, sName from tblMMD_LocationMode where idStoreConcept in (0, " + idStoreConcept + ") order by sName");
		}

		public LookupTable AttractLoop(int idStoreConcept)
		{
			return new LookupTable(Context.Config.MainDatabase,"select idIdleLoop, sName from tblMMD_IdleLoop where idStoreConcept in (0, " + idStoreConcept + ") order by sName");
		}
	}

}