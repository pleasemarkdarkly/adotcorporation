using System;
using System.Collections;
using System.Data;
using System.Data.SqlClient;
using System.Data.SqlTypes;
using System.Xml.Serialization;
using MMD.Central.CentralLib.Data;
using MMD.Central.CentralLib;

namespace MMD.Central.CentralLib.XmlObjects
{
	public class RequestArticlePersister : PersisterBase
	{

		public RequestArticlePersister(CSWContext ctx, bool bUpdating, Location location) : base(ctx,bUpdating,location) {}

		public RequestArticlePersister(CSWContext ctx, bool bUpdating) : this(ctx,bUpdating,Location.Staging) {}

		protected override DataSetHolder GetDataSet()
		{
			DataSetHolder ds = new DataSetHolder();
			Data.Connection conn = Context.Config.MainDatabase;
			ds.AddTable(conn,"Request","select * from tblMMD_RequestArticle",_bUpdating);
			return ds;		
		}

		protected override void UpdateDataSet()
		{
			_dsh.EnforceConstraints = false;
			_dsh.Update("Request","idRequestFeature");
			_dsh.EnforceConstraints = true;
		}

		public RequestArticle GetRequestArticleById(int idRequestArticle)
		{
			DataRow[] rows = _dsh.Tables["Request"].Select("idRequestArticle = " + idRequestArticle);
			if(rows.Length > 0)
				return new RequestArticle(rows[0],_dsh);
			else
				return null;

		}

		public void UpdateRequestArticles(RequestArticle [] ras)
		{
			for(int i = 0;i < ras.Length;i++)
				UpdateHelper(ras[i]);
			UpdateDataSet();
		}

		public void UpdateRequestArticle(RequestArticle ra)
		{
			UpdateHelper(ra);
			UpdateDataSet();
		}

		protected void UpdateHelper(RequestArticle ra)
		{
			DataRow [] rows = _dsh.Tables["Request"].Select("idRequestArticle = " + ra.idRequestArticle);
			DataRow row;
			bool bNew = rows.Length == 0;
			row = (bNew) ? _dsh.Tables["Request"].NewRow() : rows[0];
			ra.Fill(row,_dsh,bNew,this.Context,_location);
		}

		public void RemoveRequestArticles(int[] idRequestArticles)
		{
			foreach(int i in idRequestArticles)
			{
				DataRow row = _dsh.Tables["Request"].Rows.Find(idRequestArticles[i]);
				row.Delete();
			}
			UpdateDataSet();
		}
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class RequestArticle
	{
		public RequestArticle() {}
		
		[XmlAttribute("dbid")]
		public int idRequestArticle;

		[XmlAttribute(AttributeName="dbidItem")]
		public int idItem;

		[XmlElement(ElementName="Author",DataType="normalizedString",IsNullable=false)]
		public string sAuthor;

		[XmlElement(ElementName="Feature",DataType="normalizedString",IsNullable=false)]
		public string txFeature;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

		[XmlElement(ElementName="Status",IsNullable=false)]
		public StagingStatus byStatus;

		public RequestArticle(DataRow row, DataSetHolder dsh) : this(row, dsh, Location.Staging) {}
		
		public RequestArticle(DataRow row, DataSetHolder dsh, Location location)
		{
			idRequestArticle = Convert.ToInt32(row["idRequestArticle"]);
			idItem = Convert.ToInt32(row["idItem"]);
			sAuthor = Convert.ToString(row["sAuthor"]);
			txFeature = Convert.ToString(row["txFeature"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
			if(location == Location.Staging)
			{
				byStatus = (StagingStatus)Convert.ToInt32(row["byStatus"]);
			}
		}

		public void Fill(DataRow row, DataSetHolder dsh, bool bNew ,CSWContext ctx, Location location)
		{
			row["sAuthor"] = sAuthor;
			row["txFeature"] = txFeature;
			row["dtChanged"] = DateTime.Now;
			row["sChangedBy"] = ctx.UserName;
			if(Location.Staging == location)
			{
				row["byStatus"] = byStatus;
			}
		}
	}

	public class RequestFeaturePersister : PersisterBase
	{

		public RequestFeaturePersister(CSWContext ctx, bool bUpdating, Location location) : base(ctx,bUpdating,location) {}

		public RequestFeaturePersister(CSWContext ctx, bool bUpdating) : this(ctx,bUpdating,Location.Staging) {}

		protected override DataSetHolder GetDataSet()
		{
			DataSetHolder ds = new DataSetHolder();
			Data.Connection conn = Context.Config.MainDatabase;
			ds.AddTable(conn,"Request","select * from tblMMD_RequestFeature",_bUpdating);
			ds.AddTable(conn,"File","select * from tblMMD_File",_bUpdating);
			if(Location.Staging == _location)
			{
				ds.AddTable(conn,"FileAssoc","select * from tblMMD_FileAssociationStaging",_bUpdating);
			}
			else
			{
				ds.AddTable(conn,"FileAssoc","select * from tblMMD_FileAssociation",_bUpdating);
			}
			ds.Relations.Add("RF_FA",ds.Tables["Request"].Columns["idRequestFeature"],ds.Tables["FileAssoc"].Columns["idLinker"]);
			ds.Relations.Add("FA_F",ds.Tables["FileAssoc"].Columns["idFile"],ds.Tables["File"].Columns["idFile"]);
			return ds;		
		}

		protected override void UpdateDataSet()
		{
			_dsh.EnforceConstraints = false;
			_dsh.Update("Request","idRequestFeature");
			_dsh.Update("File","idFile");
			_dsh.Update("FileAssoc","idLinker");
			_dsh.EnforceConstraints = true;
		}

		protected void RemoveFromDataSet()
		{
			_dsh.EnforceConstraints = false;
			_dsh.Update("FileAssoc","idLinker");
			_dsh.Update("File","idFile");
			_dsh.Update("Request","idRequestFeature");
			_dsh.EnforceConstraints = true;
		}

		public RequestFeature GetRequestFeatureById(int idRequestFeature)
		{
			DataRow[] rows = _dsh.Tables["Request"].Select("idRequestFeature = " + idRequestFeature);
			if(rows.Length > 0)
				return new RequestFeature(rows[0],_dsh,this.Context);
			else
				return null;

		}

		public void UpdateRequestFeatures(RequestFeature [] rfs)
		{
			for(int i = 0;i < rfs.Length;i++)
				UpdateHelper(rfs[i]);
			UpdateDataSet();
		}

		public void UpdateRequestFeature(RequestFeature rf)
		{
			UpdateHelper(rf);
			UpdateDataSet();
		}

		protected void UpdateHelper(RequestFeature rf)
		{
			DataRow [] rows = _dsh.Tables["Request"].Select("idRequestFeature = " + rf.idRequestFeature);
			DataRow row;
			bool bNew = rows.Length == 0;
			row = (bNew) ? _dsh.Tables["Request"].NewRow() : rows[0];
			rf.Fill(row,_dsh,bNew,this.Context,_location);
		}

		public void RemoveRequestFeatures(int[] idRequestFeatures)
		{
			foreach(int i in idRequestFeatures)
			{
				DataRow row = _dsh.Tables["Request"].Rows.Find(idRequestFeatures[i]);
				RemoveHelper(row);
			}
			RemoveFromDataSet();
		}

		protected void RemoveHelper(DataRow row)
		{
			DataRow [] rows = row.GetChildRows("RF_FA");
			foreach(DataRow childrow in rows)
			{
				childrow.Delete();
			}
			row.Delete();
		}
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class RequestFeature
	{
		public RequestFeature() {}
		
		[XmlAttribute("dbid")]
		public int idRequestFeature;

		[XmlAttribute(AttributeName="dbidStoreConcept")]
		public int idStoreConcept;

		[XmlElement(ElementName="Author",DataType="normalizedString",IsNullable=false)]
		public string sAuthor;

		[XmlElement(ElementName="Feature",DataType="normalizedString",IsNullable=false)]
		public string txFeature;

		public MMDFile ImageFile;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

		[XmlElement(ElementName="Status",IsNullable=false)]
		public StagingStatus byStatus;

		public RequestFeature(DataRow row, DataSetHolder dsh, CSWContext ctx) : this(row, dsh, Location.Staging, ctx) {}
		
		public RequestFeature(DataRow row, DataSetHolder dsh, Location location, CSWContext ctx)
		{
			idRequestFeature = Convert.ToInt32(row["idRequestFeature"]);
			idStoreConcept = Convert.ToInt32(row["idStoreConcept"]);
			sAuthor = Convert.ToString(row["sAuthor"]);
			txFeature = Convert.ToString(row["txFeature"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
			if(location == Location.Staging)
			{
				byStatus = (StagingStatus)Convert.ToInt32(row["byStatus"]);
			}
			ImageFile = null;
			DataRow [] rows = dsh.Tables["FileAssoc"].Select("idLinker = " + idRequestFeature);
			if(rows.Length > 0)
			{
				DataRow [] FileRows = dsh.Tables["File"].Select("idFile = " + rows[0]["idFiel"]);
				if(FileRows.Length > 0)
				{
					ImageFile = new MMDFile(FileRows[0], dsh, ctx);
				}
			}

		}

		public void Fill(DataRow row, DataSetHolder dsh, bool bNew ,CSWContext ctx, Location location)
		{
			row["sAuthor"] = sAuthor;
			row["txFeature"] = txFeature;
			row["dtChanged"] = DateTime.Now;
			row["sChangedBy"] = ctx.UserName;
			if(Location.Staging == location)
			{
				row["byStatus"] = byStatus;
			}
			if(null != ImageFile)
			{
				ImageFile.Fill(row,bNew,dsh,ctx);
			}
		}
	}
}