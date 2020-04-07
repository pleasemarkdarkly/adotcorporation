using System;
using System.Collections;
using System.Data;
using System.Data.SqlClient;
using System.Data.SqlTypes;
using System.Xml;
using System.Xml.Serialization;
using MMD.Central.CentralLib.Data;
using MMD.Central.CentralLib;

namespace MMD.Central.CentralLib.XmlObjects
{
	public class IdleLoopPersister : PersisterBase
	{
		public IdleLoopPersister(CSWContext ctx, bool bUpdating) : base(ctx,bUpdating,Location.Staging) {}
		public IdleLoopPersister(CSWContext ctx, bool bUpdating, Location location) : base(ctx,bUpdating,location) {}

		protected override DataSetHolder GetDataSet()
		{
			DataSetHolder ds = new DataSetHolder();
			Data.Connection conn = Context.Config.MainDatabase;
			ds.FillTable(conn,"File","select * from tblMMD_File where iType = " + (int)MMDPostedFileType.IdleLoopElement);
			ds.SetPrimaryKey("File","idFile");			

			Location location = _location;
			string sTable;
			sTable = (location == Location.Staging) ? "tblMMD_FileAssociationStaging" : "tblMMD_FileAssociation";
			ds.FillTable(conn,"FileAssociation","select * from " + sTable + " where iType = " + (int)MMDPostedFileType.IdleLoopElement);

			ds.Relations.Add("F_FA",ds.Tables["File"].Columns["idFile"],ds.Tables["FileAssociation"].Columns["idFile"]);

			

			sTable = (location == Location.Staging) ? "tblMMD_IdleLoopStaging" : "tblMMD_IdleLoop";
			ds.FillTable(conn,"IdleLoop","select * from " + sTable);

			sTable = (location == Location.Staging) ? "tblMMD_IdleLoopElementStaging" : "tblMMD_IdleLoopElement";
			ds.FillTable(conn,"IdleLoopElement","select * from " + sTable);

			DataRelation dr = ds.Relations.Add("IL_ILE",ds.Tables["IdleLoop"].Columns["idIdleLoop"],ds.Tables["IdleLoopElement"].Columns["idIdleLoop"]);

			ds.Relations.Add("ILE_FA",ds.Tables["IdleLoopElement"].Columns["idIdleLoopElement"],ds.Tables["FileAssociation"].Columns["idLinker"]);
		
			return ds;		
		}

		protected override void UpdateDataSet()
		{
			DataSetHolder ds = _dsh;
			ds.BuildCommands("IdleLoop");
			ds.BuildCommands("IdleLoopElement");
			ds.BuildCommands("FileAssociation");
			//ds.BuildCommands("File");
			
			//ds.AddForeignKeyConstraint("C_IL_ILE","IdleLoop","idIdleLoop","IdleLoopElement","idIdleLoop");
			//ds.AddForeignKeyConstraint("C_ILE_FA","IdleLoopElement","idIdleLoopElement","FileAssociation","idLinker");
			//ds.AddForeignKeyConstraint("C_F_FA","File","idFile","FileAssociation","idFile");
			ds.EnforceConstraints = false;
			ds.Update("IdleLoop","idIdleLoop");
			ds.Update("IdleLoopElement","idIdleLoopElement");
			ds.Update("FileAssociation");
			ds.EnforceConstraints = true;
			//ds.Update("File","idFile");
		}

		public IdleLoop GetIdleLoopById(int idIdleLoop)
		{
			DataRow[] rows = _dsh.Tables["IdleLoop"].Select("idIdleLoop = " + idIdleLoop);
			if(rows.Length > 0)
				return new IdleLoop(_location,rows[0],_dsh,this.Context);
			else
				return null;
		}

		public IdleLoopElement GetIdleLoopElementById(int idIdleLoopElement)
		{
			DataRow[] rows = _dsh.Tables["IdleLoopElement"].Select("idIdleLoopElement = " + idIdleLoopElement);
			if(rows.Length > 0)
				return IdleLoopElement.CreateInstance(rows[0],_dsh,this.Context);
			else
				return null;
		}

	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class IdleLoop
	{
		public IdleLoop() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idIdleLoop;

		[XmlAttribute(AttributeName="dbidStoreConcept")]
		public int idStoreConcept;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		[XmlElement(ElementName="Info",DataType="string",IsNullable=true)]
		public string sInfo;
	
		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;
	
		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

		[XmlElement(ElementName="Status",IsNullable=false)]	
		public StagingStatus byStatus;
		
		[XmlArray("Frames",IsNullable=true)]
		[XmlArrayItem("Screen",typeof(ScreenMLFrame)),
		XmlArrayItem("Video",typeof(VideoFrame)),
		XmlArrayItem("Image",typeof(ImageFrame)),
		XmlArrayItem("RecList",typeof(RecListFrame))]
		public IdleLoopElement[] Frames;
		
		public IdleLoop(Location location, DataRow row, DataSetHolder ds,CSWContext ctx)
		{	
			idIdleLoop = Convert.ToInt32(row["idIdleLoop"]);
			idStoreConcept = Convert.ToInt32(row["idStoreConcept"]);
			sName = Convert.ToString(row["sName"]);
			sInfo = Convert.ToString(row["sInfo"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
			if(location == Location.Staging)
				byStatus = (StagingStatus)Convert.ToInt32(row["byStatus"]);

			DataRow[] rows = row.GetChildRows("IL_ILE");
			Frames = new IdleLoopElement[rows.Length];
			for(int i=0;i<rows.Length;i++)
				Frames[i] = IdleLoopElement.CreateInstance(rows[i],ds,ctx);
		}
	
		public void Fill(Location location,DataRow row, DataSetHolder ds, bool bNew, CSWContext ctx)
		{
			row["idStoreConcept"] = idStoreConcept;
			row["sName"] = sName;
			row["sInfo"] = sInfo;
			row["dtChanged"] = DateTime.Now;
			row["sChangedBy"] = ctx.UserName;
			if(location == Location.Staging)
				row["byStatus"] = (byte)byStatus;

			//delete idle loop elements and file associations for idle loop
			if(!bNew)
			{
				DataRow[] rowsILE = row.GetChildRows("IL_ILE");
				for(int i=0;i<rowsILE.Length;i++)
				{
					DataRow[] rowsFA = rowsILE[i].GetChildRows("ILE_FA");
					for(int j=0;j<rowsFA.Length;j++)
						rowsFA[j].Delete();

					rowsILE[i].Delete();
				}
			}

			for(int i=0;i<Frames.Length;i++)
			{
				DataRow cRow = ds.Tables["IdleLoopElement"].NewRow();
				Frames[i].Fill(cRow,row,ds,bNew,ctx);
				ds.Tables["IdleLoopElement"].Rows.Add(cRow);
			}
		}
	}

	public class ScreenMLFrame : IdleLoopElement
	{

		[XmlElement("Template",IsNullable=false)]
		public string sTemplate;

		[XmlElement("QueryString",DataType="normalizedString",IsNullable=true)]
		public string sQueryString;

		[XmlElement("ShowTime")]
		public int cShowTime;

		public ScreenMLFrame() 
		{ 
			frameType = FrameType.ScreenML; 
			sTemplate = null;
			sQueryString = null;
			cShowTime = 0;
		}

		public ScreenMLFrame(DataRow row, DataSetHolder ds, CSWContext ctx) : base(row,ds, ctx)
		{
			frameType = FrameType.ScreenML;
			sTemplate = Convert.ToString(row["sTemplate"]);
			sQueryString = Convert.ToString(row["sLink"]);
			cShowTime = Convert.ToInt32(row["cShowTime"]);
		}

		public override void Fill(DataRow row, DataRow rowParent, DataSetHolder ds, bool bNew, CSWContext ctx)
		{
			base.Fill(row,rowParent,ds,bNew,ctx);
			row["sTemplate"] = sTemplate;
			row["sLink"] = sQueryString;
			row["cShowTime"] = cShowTime;
		}

		public override void BuildIdleLoopFrame(XmlDocument xmld, XmlElement xmlRoot)
		{			
			XmlElement xmle = xmld.CreateElement("element");
			xmle.SetAttribute("seq",iSeq.ToString());
			xmle.SetAttribute("name",sName);
			xmle.SetAttribute("frame_type",frameType.ToString());
			xmle.SetAttribute("mime_type","text/xml");
			xmle.SetAttribute("url","getIdleLoopView.m?idle=1&url=" + Files[0].URL(Context));
			xmle.SetAttribute("playing_time", cShowTime.ToString());
			xmlRoot.AppendChild(xmle);
		}


	}

	public class VideoFrame : IdleLoopElement
	{
		[XmlElement("StartTime",IsNullable=true)]
		public NullableInt iStartTime;

		[XmlElement("EndTime",IsNullable=true)]
		public NullableInt iEndTime;

		public VideoFrame() 
		{ 
			frameType = FrameType.Video; 
			iStartTime = null;
			iEndTime = null;
		}

		public VideoFrame(DataRow row, DataSetHolder ds, CSWContext ctx) : base(row,ds, ctx) 
		{
			frameType = FrameType.Video;
			iStartTime = NullableInt.CreateInstance(row["iStartTime"]);
			iEndTime = NullableInt.CreateInstance(row["iEndTime"]);
		}

		public override void Fill(DataRow row, DataRow rowParent, DataSetHolder ds, bool bNew, CSWContext ctx)
		{
			base.Fill(row,rowParent,ds,bNew,ctx);
			row["iStartTime"] = NullableInt.GetValue(iStartTime);
			row["iEndTime"] = NullableInt.GetValue(iEndTime);
		}

		public override void BuildIdleLoopFrame(XmlDocument xmld, XmlElement xmlRoot)
		{			
			XmlElement xmle = xmld.CreateElement("element");
			xmle.SetAttribute("seq",iSeq.ToString());
			xmle.SetAttribute("name",sName);
			xmle.SetAttribute("frame_type",frameType.ToString());
			xmle.SetAttribute("mime_type",Files[0].sMIMEType);
			xmle.SetAttribute("url",Files[0].URL(Context));
			if(iStartTime != null)
				xmle.SetAttribute("start_time",iStartTime.ToString());
			if(iEndTime != null)
				xmle.SetAttribute("end_time", iEndTime.ToString());
			xmlRoot.AppendChild(xmle);
		}

	}

	public class ImageFrame : IdleLoopElement
	{
		[XmlElement("ShowTime")]
		public int cShowTime;

		[XmlElement("Link",IsNullable=true)]
		public string sLink;

		public ImageFrame() 
		{ 
			frameType = FrameType.Image; 
			cShowTime = 0;
			sLink = null;
		}

		public ImageFrame(DataRow row, DataSetHolder ds, CSWContext ctx) : base(row,ds, ctx)
		{
			frameType = FrameType.Image;
			cShowTime = Convert.ToInt32(row["cShowTime"]);
			sLink = Convert.ToString(row["sLink"]);
		}

		public override void Fill(DataRow row, DataRow rowParent, DataSetHolder ds, bool bNew, CSWContext ctx)
		{
			base.Fill(row,rowParent,ds,bNew,ctx);
			row["cShowTime"] = cShowTime;
			row["sLink"] = sLink;
		}

		public override void BuildIdleLoopFrame(XmlDocument xmld, XmlElement xmlRoot)
		{			
				XmlElement xmle = xmld.CreateElement("element");
				xmle.SetAttribute("seq",iSeq.ToString());
				xmle.SetAttribute("name",sName);
				xmle.SetAttribute("frame_type",frameType.ToString());
				xmle.SetAttribute("mime_type","text/xml");
				xmle.SetAttribute("url","getIdleLoopImage.m?idle=1&url=" + Files[0].URL(Context));
				xmle.SetAttribute("playing_time", cShowTime.ToString());
				xmlRoot.AppendChild(xmle);
		}

	}

	public class ProductFrame : IdleLoopElement
	{
		[XmlElement("ShowTime")]
		public int cShowTime;

		[XmlElement("idItem")]
		public int idItem;

		public ProductFrame() 
		{ 
			frameType = FrameType.Product; 
			cShowTime = 0;
			idItem = 0;
		}
		
		public ProductFrame(DataRow row, DataSetHolder ds, CSWContext ctx) : base(row,ds, ctx)
		{
			frameType = FrameType.Product;
			cShowTime = Convert.ToInt32(row["cShowTime"]);
			idItem = Convert.ToInt32(row["idItem"]);
		}

		public override void BuildIdleLoopFrame(XmlDocument xmld, XmlElement xmlRoot)
		{			
				XmlElement xmle = xmld.CreateElement("element");
				xmle.SetAttribute("seq",iSeq.ToString());
				xmle.SetAttribute("frame_type",frameType.ToString());
				xmle.SetAttribute("name",sName);
				xmle.SetAttribute("url","getItemDetail.m?idle=1&idItem=" + this.idItem);
				xmle.SetAttribute("playing_time", cShowTime.ToString());
				xmlRoot.AppendChild(xmle);
		}


	}

	public class PromotionListFrame : BaseRecListFrame
	{
		public PromotionListFrame() { frameType = FrameType.Promotion; }
		public PromotionListFrame(DataRow row, DataSetHolder ds, CSWContext ctx) : base(row,ds, ctx) { frameType = FrameType.Promotion;  }

		public override void BuildIdleLoopFrame(XmlDocument xmld, XmlElement xmlRoot)
		{
			// get rec list id from rec list type, store concept
			// spr_MMDGetRecommendListID -- need new one for promotion lists
			
			// if no matching rec list id, leave

			// get rec list count

			// make N entries where N = count \ cItemsPerPage
			
			/*
				XmlElement xmle = xmld.CreateElement("element");
				xmle.SetAttribute("seq",dr.GetInt32("iSeq").ToString());
				xmle.SetAttribute("name",dr.GetString("sName"));
				xmle.SetAttribute("frame_type",frameType.ToString());
				xmle.SetAttribute("mime_type",dr.GetString("sMIMEType"));
				xmle.SetAttribute("url",dr.GetString("sURL"));
				xmle.SetAttribute("playing_time", dr.GetInt32("cSecsPlayingTime").ToString());
				xmlRoot.AppendChild(xmle);
			*/
		}

	}

	public class RecListFrame : BaseRecListFrame
	{
		[XmlElement("dbidProductType")]
		public int idProductType;

		public RecListFrame()
		{
			frameType = FrameType.RecList;
			idProductType = 0;
		}

		public RecListFrame(DataRow row, DataSetHolder ds, CSWContext ctx) : base(row,ds, ctx)
		{
			frameType = FrameType.RecList;
			idProductType = Convert.ToInt32(row["idProductType"]);
		}

		public override void Fill(DataRow row, DataRow rowParent, DataSetHolder ds, bool bNew, CSWContext ctx)
		{
			base.Fill(row,rowParent,ds,bNew,ctx);
			row["idProductType"] = idProductType;
		}

		public override void BuildIdleLoopFrame(XmlDocument xmld, XmlElement xmlRoot)
		{
			// get rec list id from product type, rec list type, store concept
			// spr_MMDGetRecommendListID
			
			// if no matching rec list id, leave

			// get rec list count

			// make N entries where N = count \ cItemsPerPage
			
			/*
				XmlElement xmle = xmld.CreateElement("element");
				xmle.SetAttribute("seq",dr.GetInt32("iSeq").ToString());
				xmle.SetAttribute("name",dr.GetString("sName"));
				xmle.SetAttribute("frame_type",frameType.ToString());
				xmle.SetAttribute("mime_type",dr.GetString("sMIMEType"));
				xmle.SetAttribute("url",dr.GetString("sURL"));
				xmle.SetAttribute("playing_time", dr.GetInt32("cSecsPlayingTime").ToString());
				xmlRoot.AppendChild(xmle);
			*/
		}
	
	}

	public class BaseRecListFrame : IdleLoopElement
	{
		[XmlElement("dbidRecListType")]
		public int idRecListType;

		[XmlElement("ItemsPerPage")]
		public int cItemsPerPage;

		[XmlElement("ShowTime")]
		public int cShowTime;

		[XmlElement("Template",IsNullable=true)]
		public string sTemplate;

		public BaseRecListFrame() : base()
		{ 
			//frameType = FrameType.RecList; 
			idRecListType = 0;
			cItemsPerPage = 0;
			cShowTime = 0;
			sTemplate = "";
		}

		public BaseRecListFrame(DataRow row, DataSetHolder ds, CSWContext ctx) : base(row,ds, ctx)
		{
			//frameType = FrameType.RecList;
			idRecListType = Convert.ToInt32(row["idRecListType"]);
			cItemsPerPage = Convert.ToInt32(row["cItemsPerPage"]);
			cShowTime = Convert.ToInt32(row["cShowTime"]);
			sTemplate = Convert.ToString(row["sTemplate"]);
		}

		public override void Fill(DataRow row, DataRow rowParent, DataSetHolder ds, bool bNew, CSWContext ctx)
		{
			base.Fill(row,rowParent,ds,bNew,ctx);
			row["idRecListType"] = idRecListType;
			row["cItemsPerPage"] = cItemsPerPage;
			row["sTemplate"] = sTemplate;
		}
	}

	public class IdleLoopElement
	{
		public IdleLoopElement() 
		{
			sName = "";
			dtChanged = NullableDateTime.Now;
			sChangedBy = "";
		}
		
		[XmlAttribute(AttributeName="dbid")]
		public int idIdleLoopElement;

		[XmlAttribute(AttributeName="dbidIdleLoop")]
		public int idIdleLoop;

		[XmlElement(ElementName="Sequence")]
		public int iSeq;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;
	
		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;	

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

		[XmlElement(ElementName="FrameType")]
		public FrameType frameType;		

		[XmlArray("Files",IsNullable=true)]
		public MMDFile[] Files;

		private CSWContext _ctx;
		protected CSWContext Context { get { return _ctx; } }

		protected IdleLoopElement(DataRow row, DataSetHolder ds, CSWContext ctx)
		{
			_ctx = ctx;
			idIdleLoopElement = Convert.ToInt32(row["idIdleLoopElement"]);
			idIdleLoop = Convert.ToInt32(row["idIdleLoop"]);
			iSeq = Convert.ToInt32(row["iSeq"]);
			sName = Convert.ToString(row["sName"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
		
			//files
			ArrayList al = new ArrayList();
			
			DataRow[] rowFA = row.GetChildRows("ILE_FA");
			for(int i=0;i<rowFA.Length;i++)
			{
				DataRow[] rowFiles = rowFA[i].GetParentRows("F_FA");
				for(int j=0;j<rowFiles.Length;j++)
					al.Add(new MMDFile(rowFiles[j],ds,ctx));
			}

			Files = new MMDFile[al.Count];
			for(int i=0;i<al.Count;i++)
				Files[i] = (MMDFile)al[i];
		}

		public virtual void Fill(DataRow row, DataRow rowParent, DataSetHolder ds, bool bNew, CSWContext ctx)
		{
			if(rowParent != null) row.SetParentRow(rowParent);

			row["idIdleLoop"] = idIdleLoop;
			row["iSeq"] = iSeq;
			row["sName"] = sName;
			row["dtChanged"] = DateTime.Now;
			row["sChangedBy"] = ctx.UserName;
			row["iFrameType"] = (int)frameType;
			//files
			for(int i=0;i<Files.Length;i++)
			{
				DataRow rowFA = ds.Tables["FileAssociation"].NewRow();
				rowFA["iType"] = MMDPostedFileType.IdleLoopElement;
				rowFA["idFile"] = Files[i].idFile;
				rowFA.SetParentRow(row);
				ds.Tables["FileAssociation"].Rows.Add(rowFA);
			}
		}

		public virtual void BuildIdleLoopFrame(XmlDocument xmld, XmlElement xmlRoot)
		{
		}

		static public IdleLoopElement CreateInstance(DataRow row, DataSetHolder ds, CSWContext ctx)
		{
			FrameType frameType = (FrameType)Convert.ToInt32(row["iFrameType"]);
			switch(frameType)
			{
				case FrameType.ScreenML : return new ScreenMLFrame(row,ds, ctx);
				case FrameType.Video : return new VideoFrame(row,ds, ctx);
				case FrameType.Image : return new ImageFrame(row,ds, ctx);
				case FrameType.RecList : return new RecListFrame(row,ds, ctx);
				case FrameType.Promotion : return new PromotionListFrame(row,ds, ctx);
				case FrameType.Product : return new ProductFrame(row, ds, ctx);
			}
			return null;
		}

		static public IdleLoopElement CreateNewInstance(FrameType frameType)
		{
			switch(frameType)
			{
				case FrameType.ScreenML : return new ScreenMLFrame();
				case FrameType.Video : return new VideoFrame();
				case FrameType.Image : return new ImageFrame();
				case FrameType.RecList : return new RecListFrame();
				case FrameType.Promotion : return new PromotionListFrame();
				case FrameType.Product : return new ProductFrame();
			}
			return null;
		}

	}


}
