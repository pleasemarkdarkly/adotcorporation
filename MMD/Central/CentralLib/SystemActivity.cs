using System;
using System.Data;
using System.Data.SqlClient;
using System.Xml;
using System.Xml.Serialization;
using MMD.Central.CentralLib;
using MMD.Central.CentralLib.Utilities;
using MMD.Central.CentralLib.XmlObjects;

namespace MMD.Central.CentralLib.ServiceImpl
{
	//service exceptions


}


namespace MMD.Central.CentralLib.ServiceImpl
{
	public class ReportSession
	{
		public ReportSession() {}

		public int iRecsPerPage;
		public int iPages;
		
		public ReportNode reportRoot;
		public ReportObject reportObject;
	}

	public class ReportNode
	{
		public ReportNode() {}

		public ReportNode reportChild;
		
		[XmlIgnore]
		public ReportObject reportObject;

		public int iHitCount;
		public string sHitList;

		public Aggregate[] Aggregates;


	}

	public class Aggregate
	{
		public Aggregate() {}

		public string sAggregateId;
		public string sReportObjectId;
		public int iAggregate;
	}


	/// <summary>
	/// Summary description for SystemActivity.
	/// </summary>
	public class SystemActivityImpl : CSWObjectBase 
	{		
		public SystemActivityImpl(CSWContext ctx) : base(ctx) 
		{ 
			_xmldMeta = new XmlDocument();
			_xmldMeta.Load( ctx.Server.MapPath( "/CentralWeb/Common/Xml/md_report.xml" ) );
			_xnsmgrMeta = new XmlNamespaceManager( _xmldMeta.NameTable );
			_xnsmgrMeta.AddNamespace("","urn:schemas-fullplaymedia-com:mmd-report-md");
		}

		private XmlDocument _xmldMeta;
		private XmlNamespaceManager _xnsmgrMeta;

		private ReportSession _Session;
		private ReportObject _input;
		
		public void Initialize(ReportObject reportObject)
		{
			_input = reportObject;
			_Session = new ReportSession();
			
			_Session.reportRoot = new ReportNode();
			_Session.reportObject = reportObject;
			_Session.reportRoot.reportObject = reportObject;

			PrepReportTree(_Session.reportRoot, null);
		}


		public void Initialize(string sReportFile)
		{
			XmlTextReader xr = new XmlTextReader(sReportFile);
			XmlSerializer xs = new XmlSerializer(typeof(ReportObject),"urn:schemas-fullplaymedia-com:mmd");

			ReportObject reportObject = (ReportObject)xs.Deserialize(xr);
			Initialize(reportObject);
		}

				
		private void PrepReportTree(ReportNode reportNode, ReportNode parentNode)
		{
			// magical SQL generation

			// hitlist - select <id> from <table/view> where <criteria> <order by clause>
		
			string sSQL = "select " + TableKey(reportNode.reportObject) + 
						" from " + Table(reportNode.reportObject) + " " + 
						WhereCriteria(reportNode.reportObject,parentNode) + " " + 
						OrderBy(reportNode.reportObject);

			DataSet ds = Context.Config.MainDatabase.execSQL_DS(sSQL);
			DataRowCollection rows = ds.Tables[0].Rows;
			reportNode.iHitCount = rows.Count;
			string sHitList = "";
			bool bComma = false;
			foreach(DataRow row in rows)
			{
				if(bComma)
					sHitList += ",";
				else 
					bComma = true;

				sHitList += row[0].ToString();
			}
			// select list - select <id, attr/aggs> from <table/view> where <id in hitlist> <order by clause >

			if(reportNode.reportObject.reportObject != null)
			{
				reportNode.reportChild = new ReportNode();
				reportNode.reportChild.reportObject = reportNode.reportObject.reportObject;
				PrepReportTree(reportNode.reportChild, reportNode);
			}

		}

		private string TableKey(ReportObject ro)
		{
			return _xmldMeta.SelectSingleNode("//ReportObject[@roid='" + ro.sReportObjectId + "']/@key", _xnsmgrMeta).InnerText;
			
		}

		private string Table(ReportObject ro)
		{
			return _xmldMeta.SelectSingleNode("//ReportObject[@roid='" + ro.sReportObjectId + "']/DataView", _xnsmgrMeta).InnerText;
		}

		private string WhereCriteria(ReportObject ro, ReportNode parent)
		{
			if(ro.ReportCriteria == null || ro.ReportCriteria.Length == 0)
				return "";

			bool bAnd = false;
			string s = "where ";

			if(parent != null)
			{
				// check parent relationship to report object
				//conditions - Rel/@all=1 - full outer
				//			   Rel/@key no @relkey
				//			   Rel/@key and @relkey
				//			   Rel/Key
				//			   Special
			}

			XmlNode xmlnRO = _xmldMeta.SelectSingleNode("//ReportObject[@roid='" + ro.sReportObjectId + "']", _xnsmgrMeta);
			for(int i=0;i<ro.ReportCriteria.Length;i++)
			{
				ReportCriterion rc = ro.ReportCriteria[i];
				//TODO - support aggregates
				XmlNode xmlnAttr = xmlnRO.SelectSingleNode("//Attr[@atid='" + rc.sReportAttrId + "']", _xnsmgrMeta);

				if(bAnd)
					s += " and ";
				else
					bAnd = true;

				s += WhereCriterion(rc, xmlnAttr);
			}
			return s;
		}

		private string WhereCriterion(ReportCriterion rc, XmlNode xmlnAttr)
		{
			//TODO - support aggregates
			string sOp;
			switch(rc.sOp)
			{
				case "eq" : sOp = "="; break;
				case "ne" : sOp = "<>"; break;
				case "gt" : sOp = ">"; break;
				case "gte" : sOp = ">="; break;
				case "lt" : sOp = "<"; break;
				case "lte" : sOp = "<="; break;
				default:
					sOp = rc.sOp;
					break;
			}

			string sDataType = xmlnAttr.Attributes["dt"].Value;
			if(sDataType == "string" || sDataType == "datetime")
				return (rc.sReportAttrId + " " + sOp + " " + SQL.EncodeFull(rc.sOperand));
			else
				return rc.sReportAttrId + " " + sOp + " " + rc.sOperand;
		}

		private string OrderBy(ReportObject ro)
		{
			if(ro.ReportSorts == null || ro.ReportSorts.Length == 0)
				return "";

			bool bComma = false;
			string s = "order by ";
			if(ro.ReportSorts != null)
			{
				for(int i=0;i<ro.ReportSorts.Length;i++)
				{
					ReportSort rs = ro.ReportSorts[i];

					//TODO - Matt - support aggregates
					if(bComma)
						s += ", ";
					else
						bComma = true;
					
					s += rs.sReportAttrId + " " + rs.sortOrder;
				}
			}
			return s;
		}
		

		public void LoadFromSession()
		{
		}

		public void SaveToSession()
		{
		}

	
	
		

	}

	



}
