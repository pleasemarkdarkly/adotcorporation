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
	/// <summary>
	/// Summary description for Promotions.
	/// </summary>
	/// 
	/*
	public class PromotionPersister : PersisterBase
	{
		public PromotionPersister(CSWContext ctx, bool bUpdating) : base(ctx,bUpdating,Location.Staging) {}

		protected override DataSetHolder GetDataSet()
		{
			DataSetHolder ds = new DataSetHolder();
			Data.Connection conn = Context.Config.MainDatabase;
			ds.FillTable(conn,"File","select * from tblMMD_File where iType = " + (int)MMDPostedFileType.SpecialOffer);
			ds.SetPrimaryKey("File","idFile");			


			Location location = _location;
			string sTable;
			sTable = (location == Location.Staging) ? "tblMMD_FileAssociationStaging" : "tblMMD_FileAssociation";
			ds.FillTable(conn,"FileAssociation","select * from " + sTable + " where iType = " + (int)MMDPostedFileType.SpecialOffer);

			ds.Relations.Add("F_FA",ds.Tables["File"].Columns["idFile"],ds.Tables["FileAssociation"].Columns["idFile"]);

			sTable = (location == Location.Staging) ? "tblMMD_PromotionStaging" : "tblMMD_Promotion";
			ds.FillTable(conn,"Promotion","select * from " + sTable);

			ds.Relations.Add("P_FA",ds.Tables["Promotion"].Columns["idPromotion"],ds.Tables["FileAssociation"].Columns["idLinker"]);

			return ds;		
		}

		protected override void UpdateDataSet()
		{
			DataSetHolder ds = _dsh;
			ds.BuildCommands("IdleLoop");
			ds.BuildCommands("IdleLoopElement");
			ds.BuildCommands("FileAssociation");
			//ds.BuildCommands("File");
			
			ds.AddForeignKeyConstraint("C_IL_ILE","IdleLoop","idIdleLoop","IdleLoopElement","idIdleLoop");
			ds.AddForeignKeyConstraint("C_ILE_FA","IdleLoopElement","idIdleLoopElement","FileAssociation","idLinker");
			ds.AddForeignKeyConstraint("C_F_FA","File","idFile","FileAssociation","idFile");

			ds.Update("IdleLoop","idIdleLoop");
			ds.Update("IdleLoopElement","idIdleLoopElement");
			ds.Update("FileAssociation");
			//ds.Update("File","idFile");
		}


		public Promotion GetPromotionById(int idPromotion)
		{
			DataRow[] rows = _dsh.Tables["Promotion"].Select("idPromotion = " + idPromotion);
			if(rows.Length > 0)
				return new Promotion(rows[0],_dsh);
			else
				return null;

		}

	}

	public class Promotion
	{
		public Promotion() 
		{
			sName = "";
			dtChanged = NullableDateTime.Now;
			sChangedBy = "";
		}
		
		[XmlAttribute(AttributeName="dbid")]
		public int idPromotion;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;
	
		[XmlElement(ElementName="Description")]
		public string sDescription;

		[XmlElement("DetailText")]
		public string sDetailText;

		[XmlElement("ListImage")]
		public MMDFile listFile;

		[XmlElement("DetailImage")]
		public MMDFile detailFile;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;	

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;


		public Promotion(DataRow row, DataSetHolder ds)
		{
			this.idPromotion = Convert.ToInt32(row["idPromotion"]);
			sName = Convert.ToString(row["sName"]);
			sDescription = Convert.ToString(row["sDescription"]);
			sDetailText = Convert.ToString(row["txDetail"]);

			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);

			int idListFile = Convert.ToInt32(row["idListFile"]);
			int idDetailFile = Convert.ToInt32(row["idDetailFile"]);

			
			DataRow[] rows = ds.Tables["File"].Select("idFile = " + idListFile);
			if(rows.Length > 0)
				listFile = new MMDFile(rows[0],ds);

			rows = ds.Tables["File"].Select("idFile = " + idDetailFile);
			if(rows.Length > 0)
				detailFile = new MMDFile(rows[0],ds);

		}

		public virtual void Fill(DataRow row, DataRow rowParent, DataSetHolder ds, bool bNew, CSWContext ctx)
		{
		}


	}
*/

	/*public class FeatureRequestPersister : PersisterBase
	{
		public FeatureRequestPersister(CSWContext ctx, bool bUpdating) : base(ctx,bUpdating,Location.Staging) {}

		protected override DataSetHolder GetDataSet()
		{
			DataSetHolder ds = new DataSetHolder();
			Data.Connection conn = Context.Config.MainDatabase;
			ds.FillTable(conn,"Feature","select * from tblMMD_FeatureRequest");
			return ds;		
		}

		protected override void UpdateDataSet()
		{
		}


		public FeatureRequest GetFeatureRequestById(int idFeatureRequest)
		{
			DataRow[] rows = _dsh.Tables["Feature"].Select("idFeatureRequest = " + idFeatureRequest);
			if(rows.Length > 0)
				return new FeatureRequest(rows[0],_dsh);
			else
				return null;

		}

	}


	public class FeatureRequest
	{

		public int idFeatureRequest;

		public int idItem;

		public string sAuthor;

		public string txFeature;

		public NullableDateTime dtChanged;
		public string sChangedBy;

		public NullableDateTime dtDelta;
		public string sDeltaAction;

		public FeatureRequest() 
		{
			dtChanged = NullableDateTime.Now;
			dtDelta = NullableDateTime.Now;
			sDeltaAction = "A";
		}

		public FeatureRequest(DataRow row, DataSetHolder dsh)
		{
			idFeatureRequest = Convert.ToInt32(row["idFeatureRequest"]);
			idItem = Convert.ToInt32(row["idItem"]);
			sAuthor = Convert.ToString(row["sAuthor"]);
			txFeature = Convert.ToString(row["txFeature"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
			dtDelta = NullableDateTime.CreateInstance(row["dtDelta"]);
			sDeltaAction = Convert.ToString(row["sDeltaAction"]);
		}
	}*/

}
