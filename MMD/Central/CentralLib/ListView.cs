using System;
using System.Collections;
using System.Data;
using System.Data.SqlClient;
using System.Text;
using System.Text.RegularExpressions;
using System.Xml;
using MMD.Central.CentralLib;
using MMD.Central.CentralLib.Data;
using MMD.Central.CentralLib.Utilities;

namespace MMD.Central.CentralLib.ListView
{
	public class LVMissingFieldException : CSWException 
	{
		public LVMissingFieldException(string sField, string sEntity) : base("Missing Field '" + sField + "' for Entity '" + sEntity + "'") {}
	}

	public class LVMissingJoinException : CSWException 
	{
		public LVMissingJoinException(string sJoin, string sEntity) : base("Missing Join '" + sJoin + "' for Entity '" + sEntity + "'") {}
	}

	public class LookupTable
	{
		Hashtable m_coll = new Hashtable();

		public LookupTable()
		{
			m_coll = new Hashtable();
		}

		public LookupTable(XmlNodeList xmlns)  { InitFromNodeList(xmlns); }
		public LookupTable(string sDelimitedValues, string sDelimiter)  { InitFromString(sDelimitedValues, sDelimiter); }
		public LookupTable(Connection oConn, string sSQL)  { InitFromQuery(oConn, sSQL); }
		public LookupTable(Type enumType) 
		{
			InitFromEnumType(enumType);
		}

		public LookupTable(string sEnumType)
		{
			InitFromEnumType(Type.GetType(sEnumType));
		}

		public void InitFromEnumType(Type enumType)
		{
			m_coll.Clear();
			if(enumType != null && enumType.IsEnum)
			{
				foreach(object oValue in Enum.GetValues(enumType))
				{
					string s = Enum.GetName(enumType, oValue);
					s = Regex.Replace(s,"([a-z])([A-Z])","$1 $2");
					m_coll[((int)oValue).ToString()] = s;
				}
			}

		}

		public void InitFromNodeList(XmlNodeList xmlns)
		{
			m_coll.Clear();
			foreach(XmlNode xmln in xmlns)
				m_coll[xmln.Attributes["key"].Value] = xmln.Attributes["value"].Value;
			
		}

		public void InitFromString(string sDelimitedValues,string sDelimiter)
		{
			if(sDelimiter == null) sDelimiter = ",";
			m_coll.Clear();

			string[] arr = sDelimitedValues.Split(sDelimiter.ToCharArray());
			for(int i=0;i<arr.Length;i += 2)
				m_coll[arr[i]] = arr[i+1];
		}

		public void InitFromQuery(Connection oConn, string sSQL)
		{
			m_coll.Clear();
			DataSet ds = oConn.execSQL_DS(sSQL,"Query");

			foreach(DataRow row in ds.Tables["Query"].Rows)
				m_coll[ row[0].ToString() ] = row[1].ToString();
		}

		public string GetHTMLComboBox(string sComboBoxName) { return GetHTMLComboBox(sComboBoxName, "", true, true); }
		public string GetHTMLComboBox(string sComboBoxName, string sSelectedKey) { return GetHTMLComboBox(sComboBoxName, sSelectedKey, true, true); }

		public string GetHTMLComboBox(string sComboBoxName, object selectedKey)
		{
			return GetHTMLComboBox(sComboBoxName,Convert.ToString(selectedKey));
		}
		
		public string GetHTMLComboBox(string sComboBoxName, string sSelectedKey, bool bFirstEmpty, bool bIncludeSelect)
		{
			string sComboBox = "";
			if(bIncludeSelect) sComboBox = HTML.StartSelect(sComboBoxName);
			if(bFirstEmpty) sComboBox += HTML.MakeOption("","",sSelectedKey);
			foreach(object oKey in m_coll.Keys)
				sComboBox += HTML.MakeOption(oKey.ToString(), m_coll[oKey].ToString(), sSelectedKey);
			if(bIncludeSelect) sComboBox += HTML.EndSelect();
			return sComboBox;
		}

		public string GetValue(string sKey)
		{
			object o = m_coll[sKey];
			return (o != null) ? o.ToString() : "";
		}

		public string GetValue(object key)	{ return GetValue((key != null) ? key.ToString() : null);	}
	}



	public delegate string ListViewCallback(DataRow row, ColumnBase df, int iRec);
	public delegate void LVEmitFormCallback(ListView lv, DataRow row, int iRec);

	public enum PageMode
	{
		All = 0,
		Paged = 1
	}

	public class ListView : CSWObjectBase 
	{
		public static string FindFirstCheckboxValue(CSWContext ctx)
		{
			//find the first checkbox in the listview
			int cCount = ctx.Params.Int("lv_RecCount",0);
			string sValue;
			for(int i=1;i<=cCount;i++)
			{
				sValue = ctx.Params["lv_Check_" + i];
				if(sValue != null && sValue != "")
				{
					return sValue;
				}
			}
			
			return null;
		}
		
		
		const string METADATA_FILENAME = "/CentralWeb/Common/Xml/md.xml";
		
		string m_sPage;
		string m_sEntity;
		string m_sRawId;
		string m_sId;
		Hashtable m_aaBaseFilters;
		Hashtable m_aaFilters;
		Hashtable m_aaSortFields;
		ArrayList m_arSortFields;
		int m_cMaxSortFields = 3;
		ArrayList m_arColumns;
		Hashtable m_aaDataFields;
		XmlDocument m_xmldMetaData;
		XmlNode m_xmln = null;
		string m_sTarget = null;
		string m_sTblMain = "";
		int m_iRec = 0;
		bool m_bExcelLink = false;
		string m_sTitle;
		DataSet m_ds = null;
		bool m_bPaged = true;
		FieldSort m_sortDefault = null;
		int m_cRecsPerPage = 25;
		int m_cRecs;
		int m_cRecsTotal;
		int m_iPage;
		string m_sStylePrefix = "LV";

		public ListView(CSWContext ctx, string sId, string sEntity, string sDefaultSort, string sOrder) : base(ctx)
		{
			m_sPage = sId;
			m_sEntity = sEntity.ToLower();
			m_sRawId = sId;
			m_sId = sId + "_" + m_sEntity;
			m_aaBaseFilters = new Hashtable();
			m_aaFilters = new Hashtable();
			m_aaSortFields = new Hashtable();
			m_arSortFields = new ArrayList();
			m_arColumns = new ArrayList();
			m_aaDataFields = new Hashtable();
			m_xmldMetaData = new XmlDocument();
			m_sTitle = sId;
			
			if(sDefaultSort != null)
			{
				if(sOrder == null)
					sOrder = "asc";

				m_sortDefault = new FieldSort(sDefaultSort,sOrder);
			}

			ResetPageState(false);
			InitMetaData();
			ClearSortFields(true);

		}

		public ListView(CSWContext ctx, string sId, string sEntity) : this(ctx, sId, sEntity, null, null) {}

		public ListView(CSWContext ctx, string sId, string sEntity, string sDefaultSort, string sOrder, bool bLoadFromSession, bool bResetCounts) 
			: this(ctx, sId, sEntity, sDefaultSort, sOrder)
		{
			if(bLoadFromSession)
			{
				LoadFromSession(bResetCounts);
			}
		}

		public ListView(CSWContext ctx, string sId, string sEntity, bool bLoadFromSession, bool bResetCounts) 
			: this(ctx, sId, sEntity, null, null, bLoadFromSession, bResetCounts) {}

		public void Trace(string sFormat, params object[] args)
		{
			Context.Trace(sFormat, args);
		}

		public string Title { set { m_sTitle = Context.Server.HtmlEncode(value); } }
		public bool ShowExcelLink { set { m_bExcelLink = value; } }

		public string TargetPage 
		{
			get { return Context.Page.TemplateSourceDirectory + "/" + m_sPage + ".aspx"; }
		}

		public string Target { set { m_sTarget = value; } }
		public string AnchorTarget { get { return (m_sTarget != null) ? " TARGET=\"" + m_sTarget + "\" " : ""; } }

		public Data.Connection Connection { get { return Context.Config.MainDatabase; } }

		public string Entity { get { return m_sEntity; } }
		public Hashtable SortFields { get { return m_aaSortFields; } }
		public Hashtable Filters { get { return m_aaFilters; } }

		public void SetPageRecs(bool bPaged)
		{
			m_cRecsPerPage = 25; //ignore if bPaged = true in DoQuery
			m_bPaged = bPaged;
			if(bPaged == true)
				m_iPage = 0;
		}

		public void ResetPageState(bool bKeepPageMode)
		{
			m_cRecs = -1;
			m_cRecsTotal = -1;
			m_iPage = 0;
			if(!bKeepPageMode)
				m_bPaged = true;
			SetPageRecs(m_bPaged);
		}

		public int TotalRecs { get { return m_cRecsTotal; } }
		public int ColumnCount { get { return m_arColumns.Count; } }
		public int RecCount { get { return m_iRec; } }

		public string StylePrefix { get { return m_sStylePrefix; } set { m_sStylePrefix = value; } }

		protected void InitMetaData()
		{
			//map m_sEntity to field metadata
			try
			{
				m_xmldMetaData.Load(Context.Server.MapPath(METADATA_FILENAME));
				m_xmln = m_xmldMetaData.SelectSingleNode("//view[@name='" + m_sEntity + "']");
				if(m_xmln == null)
					throw new Exception(); //TODO : make listview exceptions more friendly
				m_sTblMain = m_xmln.Attributes["table"].Value;

			}
			catch(Exception)
			{
				Trace("metadata file missing, or view missing: " + Context.Server.MapPath(METADATA_FILENAME) + "; view=" + m_sEntity);
				//System.Diagnostics.Debugger.Break();
				//TODO :throw
			}

		}

		private FieldFilter CreateFieldFilter(string sField, string sOp, string sValue)
		{
			if(sOp == "" || sOp == null)
			{
				XmlNode xmlnFilter = m_xmln.SelectSingleNode("fields/field[@name='" + sField + "']/filter");
				sOp = xmlnFilter.Attributes["op"].Value;
				if(sOp == "" || sOp == null)
					sOp = "eq";
			}
			return new FieldFilter(sField, sOp, sValue);
		}

		public void AddBaseFilter(string sField, string sOp, string sValue)
		{
			m_aaBaseFilters[sField] = CreateFieldFilter(sField, sOp, sValue);
		}

		public void AddFilter(string sField, string sOp, string sValue)
		{
			m_aaFilters[sField] = CreateFieldFilter(sField, sOp, sValue);
		}
		
		protected void RemoveSortField(string sField)
		{
			ArrayList ar = new ArrayList();
			Hashtable  aa = new Hashtable();

			foreach(FieldSort fs in m_arSortFields)
			{
				if(fs.Field != sField)
				{
					ar.Add(fs);
					aa[fs.Field] = fs;
				}
			}
			m_arSortFields = ar;
			m_aaSortFields = aa;
		}

		protected int GetSortIndex(string sField)
		{
			for(int i=0;i<m_arSortFields.Count;i++)
				if(((FieldSort)m_arSortFields[i]).Field == sField)
					return i + 1;
			return 0;
		}

		public void AddSortField(string sField, string sOrder)
		{
			FieldSort fs;
			if(sOrder == "none")
			{
				RemoveSortField(sField);
				return;
			}

			//normal case
			FieldSort fsOld = (FieldSort)m_aaSortFields[sField];
			if(fsOld != null)
				fsOld.Order = sOrder;
			else
			{
				fs = new FieldSort(sField, sOrder);
				m_aaSortFields[sField] = fs;
				m_arSortFields.Add(fs);
			}

			if(m_arSortFields.Count > m_cMaxSortFields)
				RemoveSortField(((FieldSort)m_arSortFields[0]).Field);
		}

		protected int GetIntNode(XmlDocument xmld, string sNode, int iDefault)
		{
			int iResult = iDefault;
			try
			{
				XmlNode xmln = xmld.SelectSingleNode(sNode);
				if(xmln != null)
					iResult = int.Parse(xmln.InnerText);
			}
			catch(Exception e)
			{
				Trace("ListView.GetIntNode: {0}", e.Message);
			}
			return iResult;
		}

		public void ClearFilters()
		{
			m_aaFilters.Clear();
		}

		public void ClearSortFields(bool bAddDefault)
		{
			m_aaSortFields.Clear();
			m_arSortFields.Clear();
			if(bAddDefault && m_sortDefault != null)
				AddSortField(m_sortDefault.Field, m_sortDefault.Order);
		}

		public void LoadFromSession() { LoadFromSession(false, false); }
		public void LoadFromSession(bool bResetCounts) { LoadFromSession(bResetCounts,false); }
		public void LoadFromSession(bool bResetCounts, bool bLoadFields)
		{
			string sXML = (string) Context.Session["_lv_" + m_sId];
			if(sXML == null || sXML == "") return;

			XmlDocument xmld = new XmlDocument();
			xmld.LoadXml(sXML);

			m_sPage = xmld.DocumentElement.Attributes["page"].Value;
			m_sTarget = Xml.GetAttribute(xmld.DocumentElement,"target",null);
			m_sTitle = xmld.DocumentElement.Attributes["title"].Value;

			//page state (cur pg, page mode)
			m_iPage = GetIntNode(xmld, "//PageCur", 0);
			m_bPaged = GetIntNode(xmld, "//PageMode", (int)PageMode.Paged) == (int)PageMode.Paged;

			
			//filters
			XmlNodeList xmlns = xmld.SelectNodes("//Filter");
			if(xmlns.Count > 0)
				ClearFilters(); //kill all default filters if there are saved filters

			foreach(XmlNode xmln in xmlns)
				AddFilter(xmln.Attributes["field"].Value, xmln.Attributes["op"].Value, xmln.Attributes["value"].Value);

			//sorts
			ClearSortFields(false);
			xmlns = xmld.SelectNodes("//SortField");
			foreach(XmlNode xmln in xmlns)
				AddSortField(xmln.Attributes["field"].Value, xmln.Attributes["order"].Value);

			if(bLoadFields)
			{
				xmlns = xmld.SelectNodes("//FieldColumn");
				foreach(XmlNode xmln in xmlns)
					AddColumn(new FieldColumn(this,xmln.Attributes["field"].Value, xmln.Attributes["label"].Value));
			}
			//count (cur)
			if(bResetCounts)
			{
				m_cRecs = -1;
				m_cRecsTotal = -1;
			}
			else
			{
				m_cRecs = GetIntNode(xmld, "//CRecs", -1);
				m_cRecsTotal = GetIntNode(xmld, "//CRecsTotal", -1);
			}
			


			SetPageRecs(m_bPaged);
		}

		public void SaveToSession()
		{
			XmlDocument xmld = new XmlDocument();
			SaveToXml(xmld);
			Context.Session["_lv_" + m_sId] = xmld.OuterXml;
		}

		public void SaveToXml(XmlDocument xmld)
		{
			XmlElement xmlnTop = (XmlElement)xmld.AppendChild(xmld.CreateElement("ListView"));
			xmlnTop.SetAttribute("id",m_sId);
			xmlnTop.SetAttribute("page",m_sPage);
			if(m_sTarget != null)
				xmlnTop.SetAttribute("target",m_sTarget);
			xmlnTop.SetAttribute("title",m_sTitle);

			XmlElement xmle;
			XmlElement xmlnParent = Xml.AppendChildElement(xmld, xmlnTop, "Filters");
			foreach(object sKey in m_aaFilters.Keys)
			{
				FieldFilter ff = (FieldFilter)m_aaFilters[sKey];
				xmle = Xml.AppendChildElement(xmld, xmlnParent, "Filter");
				xmle.SetAttribute("field",ff.Field);
				xmle.SetAttribute("op",ff.Op);
				xmle.SetAttribute("value",ff.Value);
			}

			//sorts
			xmlnParent = Xml.AppendChildElement(xmld, xmlnTop, "SortFields");
			foreach(FieldSort fs in m_arSortFields)
			{
				xmle = Xml.AppendChildElement(xmld, xmlnParent, "SortField");
				xmle.SetAttribute("field",fs.Field);
				xmle.SetAttribute("order",fs.Order);
			}

			//fields for excel
			xmlnParent = Xml.AppendChildElement(xmld, xmlnTop, "DisplayFields");
			foreach(ColumnBase db in m_arColumns)
			{
				if(db.IsField())
				{
					FieldColumn df = (FieldColumn)db;
					xmle = Xml.AppendChildElement(xmld,xmlnParent,"FieldColumn");
					xmle.SetAttribute("field",df.Field);
					xmle.SetAttribute("label",df.Label);
				}
			}

			//count (cur)
			Xml.AppendChildElementWithValue(xmld, xmlnTop, "CRecs", m_cRecs);
			Xml.AppendChildElementWithValue(xmld, xmlnTop, "CRecsTotal", m_cRecsTotal);
			Xml.AppendChildElementWithValue(xmld, xmlnTop, "PageCur", m_iPage);
			Xml.AppendChildElementWithValue(xmld, xmlnTop, "PageMode", (m_bPaged ? (int)PageMode.Paged : (int)PageMode.All));
		}

		public ColumnBase AddColumn( ColumnBase df )
		{
			m_arColumns.Add( df );
			return df;
		}

		public ColumnBase AddDataField( string sField )
		{
			FieldColumn df = new FieldColumn(this, sField, sField);
			m_aaDataFields[sField] = df;
			return df;
		}

		public void ProcessPageCommand()
		{
			//each command may reset current count.  if it does so, it should reset page to 0
			//no command should reset total count, though sometimes we may want to refetch -- so maybe on clear all

			string sPgCmd = Context.Params["pgCmd"];
			if(sPgCmd == "" || sPgCmd == null)
				return;

			string sPgLV = Context.Params["pgLV"];
			if(sPgLV == "" || sPgLV == null || sPgLV != m_sId)
				return;

			bool bResetCurrentCount = false;
			bool bResetTotalCounts = false;
			bool bResetPage = false; //to zero

			int iParam;
			int cParams;
			string sValue;

			switch(sPgCmd)
			{
				case "SetFilter": //set to a set of key/value pairs
					bResetCurrentCount = true;
					bResetPage = true;
					ClearFilters();
					cParams = Context.Params.Int("pgParamCount",0);
					for(iParam = 1; iParam <= cParams; iParam++)
					{
						sValue = Context.Params["pgFilterValue_" + iParam];
						if(sValue != "" && sValue != null)
							AddFilter(Context.Params["pgFilterKey_" + iParam], Context.Params["pgFilterOp_" + iParam], sValue);
					}
					break;
				case "AddFilter":
					bResetPage = true;
					bResetCurrentCount = true;
					sValue = Context.Params["pgFilterValue_0"];
					if(sValue != "" && sValue != null)
						AddFilter(Context.Params["pgFilterKey_0"],Context.Params["pgFilterOp_0"],sValue);
					break;
				case "ClearFilters":
					//bResetTotalCounts = true; //why not??
					bResetCurrentCount = true;
					bResetPage = true;
					ClearFilters();
					break;
				case "AddSort":
					bResetPage = true;
					AddSortField(Context.Params["pgSortKey_0"],Context.Params["pgSortOrder_0"]);
					break;
				case "ClearSort":
					bResetPage = true;
					ClearSortFields(true);
					break;
				case "SetSort":
					bResetPage = true;
					ClearSortFields(false);
					cParams = Context.Params.Int("pgParamCount",0);
					for(iParam = 0; iParam < cParams; iParam++)
						AddSortField(Context.Params["pgSortKey_" + iParam], Context.Params["pgSortOrder_" + iParam]);
					break;
				case "SetPage":
					m_iPage = Context.Params.Int("pgParam",0);
					break;
				case "SetPageMode":
					bResetPage = true;
					m_bPaged = Context.Params.Int("pgParam",(int)PageMode.Paged) == (int)PageMode.Paged;
					SetPageRecs(m_bPaged);
					break;

			}

			if(bResetTotalCounts)
				m_cRecsTotal = -1;
			if(bResetCurrentCount)
				m_cRecs = -1;
			if(bResetPage)
				m_iPage = 0;
			/*
						//rationalize page
						if(m_cRecsTotal == -1 || m_cRecs == -1) //must be on page zero if we don't know how many there are
							m_iPage = 0;
						if(m_iPage != 0 && m_iPage >= PageCount()) //ipage is zero based
							m_iPage = PageCount() - 1;
			*/
		}

		public int PageCount()
		{
			if(m_cRecs <= 0) return 1; //always one page, even if empty
			if(!m_bPaged) return 1;

			return ((m_cRecs - 1)/m_cRecsPerPage) + 1;
		}

		public void SanitizePage()
		{
			//m_cRecs is valid
			if(m_iPage >= PageCount())
				m_iPage = PageCount() - 1;
		}

		protected void DoDisplayQuery(string sHits, int cRecsTotal, int cRecs)
		{
			string sSQL = "";
			try
			{
				string sOrder = "";
				string sSelect = "";
				string sKey = m_xmln.Attributes["key"].Value;

				m_cRecsTotal = cRecsTotal;
				m_cRecs = cRecs;

				string sJoin = GetJoinClause(false, true, true);
				sSelect = GetSelectClause(sKey);
				sOrder = GetOrderByClause(); //include ORDER BY or is totally blank

				//current records: select select list from main_table (select joins) where key in (keylist) order by sort order
				sSQL = "select " + sSelect + " from " + sJoin + " where " + m_sTblMain + "." + sKey + " in (" + sHits + ") " + sOrder;

				//to do joins: go through each list of fields in query (base where, filter where, select, sort) and get tables and mode for each
				//there will be much overlap, so need to use associative array
				//go through list and build inner joins or outer joins.  on conditions built as you go: each pair (main table and other) has well known on conditions, or they are in MD

				m_ds = Connection.execSQL_DS(sSQL,"LVQuery");
				
			}
			catch(Exception e)
			{
				Trace("Error in ListView.DoDisplayQuery: " + e.Message + ": " + sSQL);
				//swallow
			}
		}

		public void DoQuery()
		{
			string sSQL = "";
			try
			{
				string sKey = m_xmln.Attributes["key"].Value;
				string sJoin = m_sTblMain;
				string sWhere = "(1=1) " + GetBaseWhereClause();
				string sOrder = "";

				//assume key in main table; sort fields in select tables

				//sql pieces: main table (md), key field (md), select list (columns); select tables and joins (columns + md); base where conditions (md); filter where conditions (filters + md); base joins, filter joins, sort order

				//four steps: total count, current count, current actual hits (given page and count per page)

				//total count : select count(*) from main_table (base joins) where base condiitons : exec
				//maybe TODO : add base condiions to where and corresponding joins to join
				if(m_cRecsTotal == -1)
					m_cRecsTotal = Connection.DoCountQuery(sJoin + " where " + sWhere);

				//current count: select count(*) from main_table (base joins) (where joins) where base conditions and where conditions :exec
				//todo : add filter conditions to where and corresponding joins to joins
				sJoin = GetJoinClause(true, false, false); //main table, filters, sort fields, columns
				Trace("JOIN: " + sJoin);
				sWhere = "(1=1) " + GetWhereClause(); //always get from filters + base
				Trace("WHERE: " + sWhere);

				if(m_cRecs == -1)
					m_cRecs = Connection.DoCountQuery(sJoin + " where " + sWhere);

				SanitizePage();

				//current hits: select key from main_table (base joins) (where joins) (sort joins) where base conditions and where conditiions order by sort order
				//pass this to a sproc with index and cout to get. this gets hit list as rs -- build keylist from that
				sJoin = GetJoinClause(true, true, false); //main table, filters, sortfields, columns
				sOrder = GetOrderByClause(); //inlcude ORDER BY or is totally blank

				sSQL = "select " + m_sTblMain + "." + sKey + " from " +	sJoin + " where " + sWhere + " " + sOrder;

				Trace("HitList SQL: " + sSQL);

				if(m_bPaged)
					sSQL = "set rowcount " + ((m_iPage + 1) * m_cRecsPerPage) + "; " + sSQL + " ; set rowcount 0";


				int iRecStart = (m_bPaged) ? m_iPage * m_cRecsPerPage : 0;
				
 				DataRowCollection rows = Connection.execSQL_DS(sSQL,"HitQuery").Tables[0].Rows;

 
				StringBuilder sb = new StringBuilder((rows.Count-iRecStart)*9);
				for(int i = iRecStart; i < rows.Count; i++)
				{
					sb.Append(rows[i][0].ToString());
					if(i < (rows.Count - 1))
						sb.Append(",");
				}

				
				sJoin = GetJoinClause(false, true, true);
				string sSelect = GetSelectClause(sKey);
				
				//current records: select select list from main_table (select joins) where key in (keylist) order by sort order
				sSQL = "select " + sSelect + " from " + sJoin + " where " + m_sTblMain + "." + sKey + " in (" + sb.ToString() + ") " + GetBaseWhereClause() + sOrder;

				Trace("Select SQL: " + sSQL);
				//to do joins: go through each list of fields in query (base where, filter where, select, sort) and get tables and mode for each
				//there will be much overlap so need to use associative array
				//go through list and build inner joins or outer joins.  on conditions built as you go: each pair (man table and other) has well known on conditions, or they are in MD.
				m_ds = Connection.execSQL_DS(sSQL,"LVQuery");


			}
			catch(Exception e)
			{
				Trace("Error in ListView.DoQuery: " + e.Message + ": " + sSQL);
				//swallow
			
			}

		}

		public void AddBaseKeyInCondition(string sInList)
		{
			AddBaseFilter(m_xmln.Attributes["key"].Value, "in", sInList);
		}

		internal XmlNode GetFieldNode(string sField)
		{
			return m_xmln.SelectSingleNode(".//field[@name='" + sField + "']");
		}

		protected string GetValueOp(string sValue)
		{
			if(sValue == "" || sValue == null) return "";
			Regex r = new Regex("[^><=]+$");
			return r.Replace(sValue,"");
		}

		protected string GetValueValue(string sValue)
		{
			if(sValue == "" || sValue == null) return "";
			Regex r = new Regex("^[<>=]+");
			return r.Replace(sValue,"");
		}

		protected string GetSQLOp(string sOp, string sValue) { return GetSQLOp(sOp,sValue,false); }
		protected string GetSQLOp(string sOp, string sValue, bool bLiteral)
		{
			string s = "";
			if(!bLiteral)
			{
				s = GetValueOp(sValue);
				if(s != "" && s != null)
					return s;
			}
			switch(sOp)
			{
				case "eq" : s = "="; break;
				case "ne" : s = "<>"; break;
				case "gt" : s = ">"; break;
				case "lt" : s = "<"; break;
				case "gte" : s= ">="; break;
				case "lte" : s = "<="; break;
				case "in" : s = "in"; break;
				case "nin" : s = "not in"; break;
				case "like" : s= "like"; break;
				case "seq" : 
					if(sValue.ToLower() == "null")
						s = "is";
					else if(sValue.IndexOf(",") >= 0)
						s = "in";
					else 
						s = "=";
					break;
			}
			return s;
		}
		
		protected string GetValueSQL(string sValue, string sDataType) { return GetValueSQL(sValue,sDataType,false); }
		protected string GetValueSQL(string sValue, string sDataType, bool bLiteral)
		{
			if(!bLiteral)
				sValue = GetValueValue(sValue);
			switch(sDataType)
			{
				case "string" :	return sValue.ToLower() == "null" ? "null" : SQL.EncodeFull(sValue + "%");
				case "int": return sValue;
				case "datetime" : goto case "date";
				case "sdate": goto case "date";
				case "date" : return "cast('" + sValue + "' as datetime)";
				default: return sValue;
			}
		}

		protected string DealWithNullFilter(XmlNode xmlnField, string sField, string sOp, string sValue)
		{
			if(sValue.Length > 4)
				sValue = sValue.Substring(5);
			else
				sValue = "";

			string sSQLSuffix = "";
			switch(sOp)
			{
				case "ne": goto case "sne";
				case "sne" :
					sSQLSuffix = "is not null";
					if(sValue.Length > 0)
						sSQLSuffix += " and " + sField + " not in (" + GetValueSQL(sValue, xmlnField.Attributes["dt"].Value) + ") ";
					break;
				case "eq" : goto default;
				case "seq" : goto default;
				case "like" : goto default;
				default :
					sSQLSuffix = "is null";
					if(sValue.Length > 0)
						sSQLSuffix += " or " + sField + " in (" + GetValueSQL(sValue, xmlnField.Attributes["dt"].Value) + ") ";
					break;
			}
			return sField + " " + sSQLSuffix;
		}

		protected string FQFN(string sField)
		{
			XmlNode xmlnField = GetFieldNode(sField);
			if(xmlnField == null)
				throw new LVMissingFieldException(sField, m_sEntity);
			string sTable = m_sTblMain;
			XmlNode xmlnJoinTable = xmlnField.SelectSingleNode("join");
			if(xmlnJoinTable != null)
				sTable = xmlnJoinTable.Attributes["table"].Value;
			return sTable + "." + sField;
		}

		protected string GetBaseWhereClause()
		{
			string s = "";
			string sKey = "";
			string  sOp, sValue;
			XmlNode  xmlnField;
			XmlNodeList xmlnsBF;

			try
			{
				xmlnsBF = m_xmln.SelectNodes("basefilters/basefilter");
				if(xmlnsBF == null) 
					return s;

				foreach(XmlNode xmlnBF in xmlnsBF)
				{
					sKey = xmlnBF.Attributes["field"].Value;
					xmlnField = GetFieldNode(sKey);
					if(xmlnField == null)
						throw new LVMissingFieldException(sKey, m_sEntity);
					sOp = xmlnBF.Attributes["op"].Value;
					sValue = xmlnBF.Attributes["value"].Value;
					if(sValue.ToLower().IndexOf("null") >= 0)
						s += " AND (" + DealWithNullFilter(xmlnField, FQFN(sKey), sOp, sValue) + ") ";
					else
						s += " AND (" + FQFN(sKey) + " " + GetSQLOp(sOp,sValue,true) + " (" + GetValueSQL(sValue, xmlnField.Attributes["dt"].Value, true) + ")) ";
				}

				foreach(string key in m_aaBaseFilters.Keys)
				{
					sKey = key;
					FieldFilter ff = (FieldFilter)m_aaBaseFilters[sKey];
					xmlnField = GetFieldNode(sKey);
					if(xmlnField == null)
						throw new LVMissingFieldException(sKey, m_sEntity);
					if(ff.Value.ToLower().IndexOf("null") >= 0)
						s += " AND (" + DealWithNullFilter(xmlnField, FQFN(sKey), ff.Op, ff.Value) + ") ";
					else
						s += " AND ( " + FQFN(sKey) + " " + GetSQLOp(ff.Op, ff.Value, true) + " (" + GetValueSQL(ff.Value, xmlnField.Attributes["dt"].Value, true) + ")) ";
				}

				return s;

			}
			catch(Exception e)
			{
				Trace("ListView.GetBaseWhereClause: Error with field: {0}: {1}",sKey,e.Message );
				throw e;
			}
		}
		
		protected string GetWhereClause() //always filters; include base always if we ever support it, start with AND
		{
			string s = GetBaseWhereClause();
			string sKey = "";
			FieldFilter ff;
			XmlNode xmlnField;
			try
			{
				foreach(string key in m_aaFilters.Keys)
				{
					sKey = key;
					ff = (FieldFilter)m_aaFilters[sKey];
					xmlnField = GetFieldNode(sKey);
					if(xmlnField == null)
						throw new LVMissingFieldException(sKey, m_sEntity);
					if(ff.Value.ToLower().IndexOf("null") >= 0)
						s += "AND (" + DealWithNullFilter(xmlnField, FQFN(sKey), ff.Op, ff.Value) + ") ";
					else if(xmlnField.Attributes["dt"].Value == "sdate")
						s += "AND ( case(convert(varchar(50), " + FQFN(sKey) + ", 110) as datetime) " + GetSQLOp(ff.Op, ff.Value) + " (" + GetValueSQL(ff.Value,xmlnField.Attributes["dt"].Value) + ")) ";
					else
						s += "AND (" + FQFN(sKey) + " " + GetSQLOp(ff.Op, ff.Value) + " (" + GetValueSQL(ff.Value, xmlnField.Attributes["dt"].Value) + ")) ";
				}
				return s;
			}
			catch(Exception e)
			{
				Trace("ListView.GetWhereClause: Error with field: " + sKey + ": " + e.Message);
				throw e;
			}
		}

		protected string GetOrderByClause() //include ORDER BY
		{
			int i;
			if(m_arSortFields.Count == 0) return "";
			string s = "ORDER BY ";

			try
			{
				for(i=0;i<m_arSortFields.Count;i++)
				{
					FieldSort fs = (FieldSort)m_arSortFields[i];
					s += FQFN(fs.Field) + " " + fs.Order + ((i < (m_arSortFields.Count - 1)) ? ", " : " ");
				}
				return s;
			}
			catch(Exception e)
			{
				Trace("ListView.GetOrderByClause Error: " + e.Message);
				throw e;
			}

		}

		protected string GetSelectClause(string sKeyField) //no aliases
		{
			if(m_arColumns.Count == 0) return "*";
			string s = "";
			bool bHaveKey = false;

			try
			{
				foreach(ColumnBase df in m_arColumns)
				{
					if(!df.IsField())
						continue;

					if(df.Field == sKeyField)
						bHaveKey = true;
					
					s += (s == "" ? "" : ",") + FQFN(df.Field);
				}

				foreach(string sKey in m_aaDataFields.Keys)
				{
					FieldColumn df = (FieldColumn)m_aaDataFields[sKey];

					if(!df.IsField())
						continue;

					if(df.Field == sKeyField) bHaveKey = true;
					s += (s == "" ? "" : ",") + FQFN(df.Field);
				}

				if(!bHaveKey)
					s = FQFN(sKeyField) + "," + s;

				return s;

			}
			catch(Exception e)
			{
				Trace("ListView.GetSelectClause Error: " + e.Message);
				throw e;
			}
		}

		protected string GetJoinClause(bool bFilters, bool bSorts, bool bDisplay)
		{
			string sKey;
			XmlNode xmlnField;
			XmlNode xmlnJoin;
			string sType;

			string s = m_sTblMain;

			try
			{
				//collect all tables
				Hashtable aaTables = new Hashtable();
				if(bFilters)
					foreach(string key in m_aaFilters.Keys)
					{
						sKey = key;
						xmlnField = GetFieldNode(sKey);
						if(xmlnField == null)
							throw new LVMissingFieldException(sKey, m_sEntity);
						xmlnJoin = xmlnField.SelectSingleNode("join");
						if(xmlnJoin != null)
							aaTables[xmlnJoin.Attributes["table"].Value] = 1;
					}

				if(bSorts)
					foreach(FieldSort fs in m_arSortFields)
					{
						xmlnField = GetFieldNode(fs.Field);
						if(xmlnField == null)
							throw new LVMissingFieldException(fs.Field, m_sEntity);
						xmlnJoin = xmlnField.SelectSingleNode("join");
						if(xmlnJoin != null)
							aaTables[xmlnJoin.Attributes["table"].Value] = 1;
					}

				if(bDisplay)
					foreach(ColumnBase df in m_arColumns)
					{
						if(!df.IsField()) 
							continue;

						xmlnField = GetFieldNode(df.Field);
						if(xmlnField == null)
							throw new LVMissingFieldException(df.Field, m_sEntity);
						xmlnJoin = xmlnField.SelectSingleNode("join");
						if(xmlnJoin != null)
							aaTables[xmlnJoin.Attributes["table"].Value] = 1;
					}

				//now aaTables has all tables in it, with table names point to join
				XmlNode xmlnJoins = m_xmldMetaData.SelectSingleNode("//joins/lefttable[@name='" + m_sTblMain + "']");
				XmlNodeList xmlnsJoinFields;
				foreach(string key in aaTables.Keys)
				{
					sKey = key;
					xmlnJoin = xmlnJoins.SelectSingleNode("righttable[@name='" + sKey + "']");
					if(xmlnJoin == null)
						throw new LVMissingJoinException(sKey, m_sEntity);
					sType = Xml.GetAttribute(xmlnJoin,"type","INNER");
					s += " " + sType + " JOIN " + sKey + " ON ";
					xmlnsJoinFields = xmlnJoin.SelectNodes("joinfield");

					bool bFirst = true;
					foreach(XmlNode xmlnJoinField in xmlnsJoinFields)
					{
						if(bFirst)
							bFirst = false;
						else
							s += "AND ";
						s += m_sTblMain + "." + xmlnJoinField.Attributes["leftname"].Value + " = " + sKey + "." + xmlnJoinField.Attributes["rightname"] + " ";
					}
				}

				return s;
			}
			catch(Exception e)
			{
				Trace("ListView.GetJoinClause Error: {0}",e.Message);
				throw e;
			}
		}

		//left nav area includes current page start, current page end, current count, total count, link to clear filter: currently viewing n..m of s (to view all t, click here)
		//right nav area includes page list and page mode toggle: prev ..n-2 n-1 n n+1 n+2 ... next show_all
		protected string Style(string sStyle)
		{
			return " class=\"" + m_sStylePrefix + sStyle + "\" ";
		}

		public void EmitNavStart()
		{
			Write("<TABLE" + Style("ViewTable") + "cellpadding=0 cellspacing=0 border=0>");
			Write("<TR" + Style("ViewRow") + "><TD" + Style("NavCell") + " colspan=\"");
			Write(ColumnCount.ToString());
			Write("\"><TABLE" + Style("NavTable") + "cellpadding=0 cellspacing=0 border=0 width=100%><TR " + Style("ViewRow") + ">");
		}

		public void EmitLeftNav()
		{
			Write("<TD " + Style("NavCell") + ">");

			int iCur = m_iPage * m_cRecsPerPage;
			int iMax = iCur + m_cRecsPerPage;
			if(iMax > m_cRecs)
				iMax = m_cRecs;
			Write("<b>Filter:</b> [<a href=\"javascript:frmFilter_" + m_sId + ".submit()\">Filter</a>]&nbsp;&nbsp;&nbsp;");
			if(m_bPaged)
				Write("<b>Items:</b> " + (iCur + 1) + " to " + iMax + " of " + m_cRecs);
			
			//write clear all link if there are more recs
			if(m_cRecs < m_cRecsTotal)
				Write("&nbsp;&nbsp;[<a " + Style("NavLink") + AnchorTarget + " HREF=\"" + TargetPage + "?pgCmd=ClearFilters&pgLV=" + m_sId + "\">View all " + m_cRecsTotal + "</a>]");

			Write("</TD>");		
		}

		protected string MakePageLink(int iPage)
		{
			if(iPage == m_iPage)
				return "<b>" + (iPage + 1) + "</b>";
			else
				return "<a " + AnchorTarget + " HREF=\"" + TargetPage + "?pgCmd=SetPage&pgLV=" + m_sId + "&pgParam=" + iPage + "\">" + (iPage+1) + "</a>";
		}

		public void EmitRightNav()
		{
			Write("<td " + Style("NavCell") + " align=\"right\">");
			string sMsg = "";

			if(m_bPaged)
			{
				//prev link
				string s = "&lt;Previous";
				if(m_iPage > 0)
					s = "<a " + AnchorTarget + Style("NavLink") + " HREF=\"" + TargetPage + "?pgCmd=SetPage&pgLV=" + m_sId + "&pgParam=" + (m_iPage - 1) + "\">" + s + "</a>"; //prev with link

				//do pages
				string sPages = ""; // "" + (m_iPage + 1) + " ";
				int iPageMin = m_iPage - 2;
				if(iPageMin < 0) 
					iPageMin = 0;
				int iPageMac = m_iPage + 5 - (m_iPage - iPageMin);
				if(iPageMac > PageCount()) 
					iPageMac = PageCount();

				//compute min and max page to show
				//if min > 0 include page 1
				if(iPageMin > 0)
					sPages += MakePageLink(0) + (iPageMin > 1 ? "..." : "&nbsp;");
				for(int i=iPageMin;i<iPageMac;i++)
					sPages += MakePageLink(i) + "&nbsp;";

				sMsg = s + " " + sPages;

				//next link
				s = "Next&gt;";
				if(m_iPage < PageCount() - 1)
					s = "<a " + AnchorTarget + Style("NavLink") + " HREF=\"" + TargetPage + "?pgCmd=SetPage&pgLV=" + m_sId + "&pgParam=" + (m_iPage + 1) + "\">" + s + "</a>"; //next with link
				sMsg += s + " ";
				if(m_sTarget != null && m_sTarget != "")
					sMsg += "&nbsp;&nbsp;";
				else if(m_iPage < PageCount() - 1 || m_iPage != 0)
					sMsg += "&nbsp;&nbsp;[<a href=\"javascript:ConfirmLink('Are you sure you want to show all? Display lots of records on a single page can be very slow.','" + TargetPage + "?pgCmd=SetPageMode&pgLV=" + m_sId + "&pgParam=" + (int)PageMode.All + "')\">Show All</a>]";
				else
					sMsg += "&nbsp;&nbsp;[Show All]";
			}
			else //in show all mode
				sMsg += " [<a " + AnchorTarget + " HREF=\"" + TargetPage + "?pgCmd=SetPageMode&pgLV=" + m_sId + "&pgParam=" + (int)PageMode.Paged + "\">Show Pages</a>]";


			Write("&nbsp;&nbsp;&nbsp;&nbsp;<b>Pages:</b> ");

			Write(sMsg);
			
			//if(m_bExcelLink)	
			//if(m_iPage < PageCount() - 1 || m_iPage != 0)
				Write("&nbsp;&nbsp;[<a target=\"_blank\" href=\"/CentralWeb/Common/ListViewExcel.aspx?sId=" + m_sRawId + "&sEntity=" + m_sEntity + "\">Excel</a>]");
			
			Write("</td>");
		}

		public void EmitNavEnd()
		{
			Write("</tr></table>"); //end of nav table
			Write("</td></tr>"); //end of nav table containing cell and row
		}

		public void EmitViewStart()
		{
			Write("<tr><td><div style=\"width: 1px; height: 10px\"></div></td></tr>");
		}

		//header are : col headers with sort links (asc, desc, off): need two icons
		//row two is filters

		public void EmitHeaders() { EmitHeaders(true, true); }
		
		public void EmitHeaders(bool bShowFilter, bool bHeaderLinks)
		{
			EmitColumnHeaders(bHeaderLinks);

			if(bShowFilter)
			{
				EmitFilters();
			}
		}

		public void EmitColumnHeaders(bool bHeaderLinks)
		{
			Write("<tr " + Style("HdrRow") + ">");

			foreach(ColumnBase df in m_arColumns)
				EmitHdr(df, bHeaderLinks);
			
			Write("</tr>");
		}

		public void EmitFilters()
		{
			Write("<form name=\"frmFilter_" + m_sId + "\" method=\"post\" action=\"" + TargetPage + "?pgCmd=SetFilter&pgLV=" + m_sId + "&pgParamCount=" + ColumnCount + "\" " + AnchorTarget + " ><tr>");
			
			for(int i=0;i<m_arColumns.Count;i++)
				EmitFilter((ColumnBase)m_arColumns[i],i+1);

			Write("<td " + Style("FilterCell") + " style=\"border: 0px\">");
	
			Write("<input type=submit value=\"Filter\">");

			Write("</td></tr></form>");
		}

		protected void EmitHdr(ColumnBase df, bool bHeaderLinks)
		{
			Write("<td valign=\"bottom\" " + Style("HdrCell") + ">");
			string sMsg = df.Label;
			if(df is CheckboxColumn || df is CheckboxCallbackColumn)
				sMsg = "<input type=\"checkbox\" onclick=\"LVToggleCheckAll()\">&nbsp;";
			else if(bHeaderLinks && df.CanSort)
			{
				string sOrder = df.CurrentSort;
				string sOrderLink;
				string sOrderIcon = "";
				switch(sOrder)
				{
					case "asc":
						sOrderLink = "desc";
						sOrderIcon = " +" + GetSortIndex(df.Field);
						break;
					case "desc":
						sOrderLink = "none";
						sOrderIcon = " -" + GetSortIndex(df.Field);
						break;
					default :
						sOrderLink = "asc";
						sOrderIcon = "";
						break;
				}	
				sMsg = "<a " + Style("SortLink") + " HREF=\"\" onclick=\"LVSetSort('" + m_sId + "','" + TargetPage + "','" + df.Field + "','" + sOrderLink + "','" + (m_sTarget != null ? m_sTarget : "") + "')\">" + sMsg + sOrderIcon + "</a>";
			}
			Write(sMsg);
			Write("</td>");
		}


		protected void EmitFilter(ColumnBase df, int idx)
		{
			Write("<TD " + Style("FilterCell") + ">");
			df.EmitFilter(idx);
			Write("</TD>");
		}

		public void EmitForm(LVEmitFormCallback efcb)
		{
			if(efcb == null) return;
			try
			{
				m_iRec = 0;
				foreach(DataRow row in m_ds.Tables[0].Rows)
				{
					m_iRec++;
					efcb(this, row, m_iRec);					
				}
			}
			catch(Exception e)
			{
				Trace("ListView.EmitForm error: " + e.Message);
			}
		}

		public void EmitRows()
		{
			try
			{
				m_iRec = 0;
				foreach(DataRow row in m_ds.Tables[0].Rows)
				{
					m_iRec++;
					Write("<TR id=\"lv_Row_" + m_iRec + "\"" + Style("ViewRow") + ">");
					EmitRow(row);
					Write("</TR>");
				}
			}
			catch(Exception e)
			{
				Trace("ListView.EmitRows Error Row={0}: {1}",m_iRec,e.Message);
			}
		}		

		protected void EmitRow(DataRow row)
		{
			try
			{
				foreach(ColumnBase df in m_arColumns)
				{
					Write("<TD ");
					Write(Style("ViewCell"));
					df.EmitParentElementAttributes();
					Write(">");
					df.EmitCell(row, m_iRec);
					Write("</TD>");
				}
			}
			catch(Exception e)
			{
				Trace("ListView.EmitRow Error: {0}",e.Message);
			}
		}

		public void EmitViewEnd()
		{
			Write("</TABLE>");
			Write("<input type=\"hidden\" name=\"lv_RecCount\" id=\"lv_RecCount\" value=\"" + m_iRec + "\">");
		}


		public void Write(string s)
		{
			Context.Response.Write(s);
		}

		
		public void EmitExcel()
		{
			m_bPaged = false;
			SetPageRecs(m_bPaged);
			m_iPage = 0;

			DoQuery();

			Utilities.Excel.ExcelWriter xl = new Utilities.Excel.ExcelWriter(Context);
			xl.StartWorkBook(this.m_sTitle,"MMD Central","Fullplay Media");
			xl.StartWorkSheet(this.m_sTitle);

			for(int i=0;i<m_arColumns.Count;i++)
				if(m_arColumns[i] is FieldColumn)
					xl.EmitColumn();

			xl.StartRow();
			foreach(FieldColumn df in m_arColumns)
			{
				xl.EmitString(df.Label,"s21");
			}
			xl.EndRow();

			m_iRec = 0;
			foreach(DataRow row in m_ds.Tables[0].Rows)
			{
				m_iRec++;
				xl.StartRow();
				foreach(FieldColumn df in m_arColumns)
				{
					string sDt = GetFieldNode(df.Field).Attributes["dt"].Value;
					string sValue;
					switch (sDt)
					{
						case "int": 
							sValue = df.GetValue(row, m_iRec);
							try
							{
								int i = int.Parse(sValue);
								xl.EmitInteger(i,"");
							}
							catch { xl.EmitString(sValue,""); }
							break;
						case "float":
							sValue = df.GetValue(row, m_iRec);
							try
							{
								double d = double.Parse(sValue);
								xl.EmitDouble(d,"");
							}
							catch { xl.EmitString(sValue,""); }
							break;
						case "date": goto case "sdate";
						case "sdate":
						{
							object o = row[df.Field];
							if(o == System.DBNull.Value)
								xl.EmitShortDateTime(null);
							else
								xl.EmitShortDateTime(((DateTime)o).ToString("s"));
							break;
						}
						case "datetime":
						{
							object o = row[df.Field];
							if(o == System.DBNull.Value)
								xl.EmitLongDateTime(null);
							else
								xl.EmitLongDateTime(((DateTime)o).ToString("s"));
							break;
						}
						case "string": goto default;
						default:
							xl.EmitString(df.GetValue(row,m_iRec),"");
							break;
					}

				}
				xl.EndRow();
			}

			xl.EndWorkSheet();
			xl.EndWorkBook();
		}

	}

	public class FieldFilter
	{
		string m_sField, m_sOp, m_sValue;
		public FieldFilter(string sField, string sOp, string sValue)
		{
			m_sField = sField;
			m_sOp = sOp;
			m_sValue = sValue;
		}

		public string Field 
		{
			get { return m_sField; }
			set { m_sField = value; }
		}

		public string Op
		{
			get { return m_sOp; } 
			set { m_sOp = value; }
		}

		public string Value
		{
			get { return m_sValue; }
			set { m_sValue = value;	}
		}
	}

	public class FieldSort
	{
		string m_sField, m_sOrder;
		public FieldSort(string sField,string sOrder)
		{
			m_sField = sField;
			m_sOrder = sOrder;
		}

		public string Field
		{
			get { return m_sField; }
			set { m_sField = value; }
		}

		public string Order
		{
			get { return m_sOrder; }
			set { m_sOrder = value; }
		}
	}

	public class FieldColumn : ColumnBase
	{
		public FieldColumn(ListView lv, string sField, string sLabel) : base(lv,sField,sLabel) {}

		public override bool IsField() { return true; }
		public override string RenderCell(DataRow row, int iRec)
		{
			try
			{
				XmlNode xmlnDisplay = m_xmlnField.SelectSingleNode("display");
				string sDisplayType = "literal";
				if(xmlnDisplay != null)
					sDisplayType = xmlnDisplay.Attributes["type"].Value.ToLower();
				switch(sDisplayType)
				{
					case "literal": 
						return FormatValue(row[m_sField]);
					case "lookupex":
					{
						if(m_ltDisplay == null)
							m_ltDisplay = new LookupTable(m_xmlnField.SelectNodes("display/lv"));

						string sDataType = m_xmlnField.Attributes["dt"].Value;
						if(sDataType == "int")
							return m_ltDisplay.GetValue(Convert.ToInt32(row[m_sField]).ToString());
						else
							return m_ltDisplay.GetValue(row[m_sField].ToString());
					}
					case "lookupdb":
						if(m_ltDisplay == null)
							m_ltDisplay = new LookupTable(m_lv.Connection, GetNodeParam(xmlnDisplay,"sql",""));
						return m_ltDisplay.GetValue(row[m_sField].ToString());
					case "lookupenum":
						if(m_ltDisplay == null)
							m_ltDisplay = new LookupTable(Type.GetType(GetNodeParam(xmlnDisplay,"type","")));
						return m_ltDisplay.GetValue(row[m_sField].ToString());
					default:
						return FormatValue(row[m_sField]);
				}
			}
			catch(Exception e)
			{
				m_lv.Trace("FieldColumn.RenderCell error: {0}",e.Message);
				throw e;
			}

		}

		public override string GetValue(DataRow row,int iRec)
		{
			string s = "";
			try
			{
				string sDisplayType = "literal";

				XmlNode xmlnDisplay = m_xmlnField.SelectSingleNode("display");
				if(xmlnDisplay != null)
					sDisplayType = xmlnDisplay.Attributes["type"].Value.ToLower();
				switch(sDisplayType)
				{
					case "literal":
						s = FormatValue(row[m_sField]);
						break;
					case "lookupex":
						if(m_ltDisplay == null)
							m_ltDisplay = new LookupTable(m_xmlnField.SelectNodes("display/lv"));
						s = m_ltDisplay.GetValue(row[m_sField].ToString());
						break;
					case "lookupdb":
						if(m_ltDisplay == null)
							m_ltDisplay = new LookupTable(m_lv.Connection, GetNodeParam(xmlnDisplay,"sql",""));
						s = m_ltDisplay.GetValue(row[m_sField].ToString());
						break;
					case "lookupenum":
						if(m_ltDisplay == null)
							m_ltDisplay = new LookupTable(Type.GetType(GetNodeParam(xmlnDisplay,"type","")));
						s = m_ltDisplay.GetValue(row[m_sField].ToString());
						break;
				}
				
				
			}
			catch(Exception e)
			{
				m_lv.Trace("FieldColumn.GetValue Error: {0}",e.Message);				
				System.Diagnostics.Debugger.Break();
			}
			return s; //resp.Write(s + "&nbsp;");
		}

	}

	public class FieldLinkColumn : FieldColumn
	{
		string m_sLink;

		public FieldLinkColumn(ListView lv, string sField, string sLabel, string sLink) : base(lv, sField, sLabel) 
		{
			m_sLink = sLink; 
		}

		public override string RenderCell(DataRow row, int iRec)
		{
			if(m_sLink != null && m_sLink != "")
				return MakeHref( FillInPlaceholders(m_sLink, row, iRec), base.RenderCell(row, iRec) );
			else
				return base.RenderCell(row, iRec);
		}
	}

	public class FieldCallbackColumn : FieldColumn
	{
		ListViewCallback m_lvcb;

		public FieldCallbackColumn(ListView lv, string sField, string sLabel, ListViewCallback lvcb) : base(lv, sField, sLabel)
		{
			m_lvcb = lvcb;
		}

		public override string RenderCell(DataRow row, int iRec)
		{
			if(m_lvcb != null)
				return MakeHref( m_lvcb(row, this, iRec), base.RenderCell(row, iRec) );
			else
				return base.RenderCell( row, iRec );
		}
	}

	public class ShortcutColumn : ColumnBase
	{
		string m_sLink;
		public ShortcutColumn(ListView lv, string sLabel) : this(lv, sLabel,null) {}
		public ShortcutColumn(ListView lv, string sLabel, string sLink) : base(lv, sLabel, sLabel)
		{
			m_sLink = sLink;
		}

		public override string RenderCell(DataRow row, int iRec)
		{
			if(m_sLink != "" && m_sLink != null)
				return MakeHref( FillInPlaceholders(m_sLink, row, iRec), m_sLabel);
			else
				return m_sLabel;
		}
	}

	public class ShortcutCallbackColumn : ColumnBase
	{
		ListViewCallback m_lvcb;
		string m_sColumnItemsLable;

		public ShortcutCallbackColumn(ListView lv, string sLabel, ListViewCallback lvcb) : base(lv, sLabel, sLabel) 
		{
			m_lvcb = lvcb;
			m_sColumnItemsLable = sLabel;
		}

		public ShortcutCallbackColumn(ListView lv, string sColumnLable, string sColumnItemsLable, ListViewCallback lvcb) : base(lv, sColumnLable, sColumnLable) 
		{
			m_lvcb = lvcb;
			m_sColumnItemsLable = sColumnItemsLable;
		}

		public override string RenderCell(DataRow row, int iRec)
		{
			if(m_lvcb != null)
			{
				string sLink = m_lvcb(row,this,iRec);
				if(sLink == "" || sLink == null)
					return "";
				else
					return MakeHref( sLink, m_sColumnItemsLable);
			}
			else
				return "";
		}
	}

	public class CheckboxColumn : ColumnBase
	{
		string m_sCheckValue;
		public CheckboxColumn(ListView lv, string sLabel, string sCheckValue) : base(lv, sLabel, sLabel)
		{
			m_sCheckValue = sCheckValue;
		}

		public override string RenderCell(DataRow row, int iRec)
		{
			return "<input type=\"checkbox\" name=\"lv_Check_" + iRec 
				+ "\" id=\"lv_Check_" + iRec + "\" value=\"" 
				+ FillInPlaceholders(m_sCheckValue, row, iRec) + "\">";
		}
	}

	public class CheckboxCallbackColumn : CheckboxColumn
	{
		ListViewCallback m_lvcb;
	
		public CheckboxCallbackColumn(ListView lv, string sLabel, ListViewCallback lvcb) : base(lv, sLabel, "")
		{
			m_lvcb = lvcb;
		}

		public override string RenderCell(DataRow row, int iRec)
		{
			if(m_lvcb != null)
				return m_lvcb(row, this, iRec);
			else
				return base.RenderCell(row, iRec);
		}
	}

	public class ControlColumn : ColumnBase
	{
		string m_sControl;
		public ControlColumn(ListView lv, string sLabel, string sControl) : base(lv, sLabel, sLabel)
		{
			m_sControl = sControl;
		}

		public override string RenderCell(DataRow row, int iRec)
		{
			return FillInPlaceholders(m_sControl, row, iRec);
		}
	}

	public class ControlCallbackColumn : ColumnBase
	{
		ListViewCallback m_lvcb;
		
		public ControlCallbackColumn(ListView lv, string sLabel, ListViewCallback lvcb) : base(lv, sLabel, sLabel)
		{
			m_lvcb = lvcb;
		}

		public override string RenderCell(DataRow row, int iRec)
		{
			return (m_lvcb != null) ? m_lvcb(row, this, iRec) : "";
		}
	}

	public class SearchableControlColumn : FieldColumn
	{
		string m_sControl;
		public SearchableControlColumn(ListView lv, string sField, string sLabel, string sControl) : base(lv, sField, sLabel)
		{
			m_sControl = sControl;
		}

		public override string RenderCell(DataRow row, int iRec)
		{
			return FillInPlaceholders(m_sControl, row, iRec);
		}
	}

	public class SearchableControlCallbackColumn : ColumnBase
	{
		ListViewCallback m_lvcb;

		public SearchableControlCallbackColumn(ListView lv, string sField, string sLabel, ListViewCallback lvcb) : base(lv, sField, sLabel)
		{
			m_lvcb = lvcb;
		}

		public override string RenderCell(DataRow row, int iRec)
		{
			return (m_lvcb != null) ? m_lvcb(row, this, iRec) : "";
		}
	}

	public abstract class ColumnBase
	{
		protected ListView m_lv;
		protected string m_sField;
		protected string m_sLabel;
		protected XmlNode m_xmlnField;
		protected LookupTable m_ltDisplay = null;
		protected int m_iMaxLen = -1;
		protected string m_sTarget = "";

		protected string m_sAlign = null;
		protected string m_sVAlign = null;
		protected string m_sStyle = null;


		public ColumnBase( ListView lv, string sField, string sLabel) 
		{
			m_lv = lv;
			m_sField = sField;
			m_sLabel = sLabel;

			m_xmlnField = m_lv.GetFieldNode(m_sField);
			if(m_xmlnField == null && this is FieldColumn)
			{
				m_lv.Trace("In metadata, could not find field: {0}", m_sField);
				//System.Diagnostics.Debugger.Break();
				throw new LVMissingFieldException(m_sField, m_lv.Entity);
			}
		}

		public string Field { get { return m_sField; } }
		public string Label { get { return m_sLabel; } }
		public string Align { get { return m_sAlign; } set { m_sAlign = value; } }
		public string VerticalAlign { get { return m_sVAlign; } set { m_sVAlign = value; } }
		public string Style { get { return m_sStyle; } set { m_sStyle = value; } }

		public string Target 
		{ 
			get { return m_sTarget; }
			set { m_sTarget = value; }
		}

		public void SetTargetTop() { m_sTarget = "_top"; }
		public void SetTargetBlank() { m_sTarget = "_blank"; }

		public int MaxLength { set { m_iMaxLen = value; } }
			
		public virtual bool IsField() { return false; }

		public string FormatValue(object v)
		{
			if(m_xmlnField == null || v == null || v == System.DBNull.Value) return "";
				
			string sDataType = "";
			string sFormat = "";
			try 
			{
				sDataType = m_xmlnField.Attributes["dt"].Value;
				sFormat = Xml.GetAttribute(m_xmlnField,"format","");
			}
			catch {}
				
			switch(sDataType)
			{
				case "string" :
					if(m_iMaxLen > -1)
						return v.ToString().Substring(0,m_iMaxLen);
					else
						return v.ToString();
				case "int":
					return Convert.ToInt32(v).ToString("D");
				case "float":
					return ((double)v).ToString("F");
				case "date":
					return (sFormat == "") ? ((DateTime)v).ToString("d") : ((DateTime)v).ToString(sFormat);
				case "sdate":
					return (sFormat == "") ? ((DateTime)v).ToString("d") : ((DateTime)v).ToString(sFormat);
				case "datetime":
					return (sFormat == "") ? ((DateTime)v).ToString() : ((DateTime)v).ToString(sFormat);
				default :
					return v.ToString();
			}
		}

		public bool CanSort
		{
			get
			{
				if(m_xmlnField == null) return false;
				return m_xmlnField.Attributes["sort"].Value == "1";
			}
		}

		public string CurrentSort
		{
			get
			{
				string sSort = "";
				FieldSort sf = (FieldSort)m_lv.SortFields[m_sField];
				if(sf != null)
					sSort = sf.Order;
				return sSort;
			}
		}

		public void EmitFilter(int idx)
		{
			XmlNode xmlnFilter = null;
			if(m_xmlnField != null)
				xmlnFilter = m_xmlnField.SelectSingleNode("filter");
			if(xmlnFilter == null || xmlnFilter.Attributes["type"].Value == "none")
			{
				Write("&nbsp;");
				return;
			}
			//now it depends on type of filter
			Write("<INPUT TYPE=\"HIDDEN\" NAME=\"pgFilterKey_" + idx + "\" VALUE=\"" + m_sField + "\">"); //hidden field name
			Write("<INPUT TYPE=\"HIDDEN\" NAME=\"pgFilterOp_" + idx + "\" VALUE=\"\">"); //ops are looked up later on server side

			FieldFilter ff = (FieldFilter)m_lv.Filters[m_sField]; //get current filter, if any
			string sValue = (ff != null) ? ff.Value : "";

			LookupTable lt;
			switch(xmlnFilter.Attributes["type"].Value)
			{
				case "lookupex" :
					XmlNodeList xmlnsValues = xmlnFilter.SelectNodes("lv");
					lt = new LookupTable(xmlnsValues);
					m_lv.Write(lt.GetHTMLComboBox("pgFilterValue_" + idx, sValue, true, true));
					break;
				case "lookupdb" :
					lt = new LookupTable(m_lv.Connection, GetNodeParam(xmlnFilter, "sql", ""));
					m_lv.Write(lt.GetHTMLComboBox("pgFilterValue_" + idx, sValue, true, true));
					break;
				case "lookupenum":
					lt = new LookupTable(Type.GetType(GetNodeParam(xmlnFilter,"type","")));
					m_lv.Write(lt.GetHTMLComboBox("pgFilterValue_" + idx, sValue, true, true));
					break;
				case "text" :
					m_lv.Write("<INPUT TYPE=\"TEXT\" NAME=\"pgFilterValue_" + idx + "\" SIZE=\"" 
						+ GetNodeParam(xmlnFilter, "size", "5") + "\" MAXLENGTH=\"" 
						+ GetNodeParam(xmlnFilter, "maxlength", "100") + "\" VALUE=\"" + sValue + "\" >"); //field
					break;								
			}
		}

		public string MakeHref(string sHref, string sLabel)
		{
			if(sHref == null || sHref == "") return sLabel;
			if(sLabel == null || sLabel == "") sLabel = "&lt;null&gt;";
			if(m_sTarget == null || m_sTarget == "")
				return "<A HREF=\"" + sHref + "\">" + sLabel + "</A>";
			else
				return "<A TARGET=\"" + m_sTarget + "\" HREF=\"" + sHref + "\">" + sLabel + "</A>";
		}

		public string FillInPlaceholders(string sLink, DataRow row, int iRec)
		{
			int iPos = -1;
			int iPosEnd = 0;
			string sField = "";
			while( (iPos = sLink.IndexOf("%")) >= 0)
			{
				iPosEnd = sLink.IndexOf("%", iPos + 1);
				if(iPosEnd < 0) //no closing one, so skip it
					break;
				sField = sLink.Substring(iPos+1, iPosEnd - iPos - 1);
				if(sField == "iRec")
					sLink = sLink.Replace("%" + sField + "%", iRec.ToString());
				else
					sLink = sLink.Replace("%" + sField + "%", row[sField].ToString().Replace("'","\\'") );
			}
			return sLink;
		}

		public abstract string RenderCell(DataRow row, int iRec);

		public void EmitCell(DataRow row, int iRec)
		{
			Write(RenderCell(row, iRec));
			Write("&nbsp;");
		}

		public void EmitParentElementAttributes()
		{
			if(Align != null)
			{
				Write(" align=\""); Write(Align); Write("\"");
			}
			if(VerticalAlign != null)
			{
				Write(" valign=\""); Write(VerticalAlign); Write("\"");
			}
			if(Style != null)
			{
				Write(" style=\""); Write(Style); Write("\"");
			}

		}

		public virtual string GetValue(DataRow row, int iRec)
		{
			return "";
		}

		public string GetNodeParam(XmlNode xmln, string sParam, string sDefault)
		{
			string s = sDefault;
			XmlNode xmlnParam = xmln.SelectSingleNode("param[@name='" + sParam + "']");
			if(xmlnParam != null)
				s = xmlnParam.Attributes["value"].Value;
			return s;
		}

		protected void Write(string s)
		{
			m_lv.Write(s);
		}
	}

}
