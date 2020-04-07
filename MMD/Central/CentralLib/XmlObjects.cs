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
/*
  	public enum Location
	{
		Staging,
		Deploy,
	}

	public enum MMDPostedFileType
	{
		Banner = 0,
		IdleLoopElement = 1,
		SpecialOffer = 2
	}

	public enum SortOrder
	{
		ASC = 0,
		DESC = 1
	}

	public enum FrameType
	{
		ScreenML = 0,
		Video = 1,
		Image = 2,
		RecList = 3,
		SpecialOffer = 4
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd",IsNullable=true)]
	public class NullableDateTime
	{
		public NullableDateTime() {}
		public NullableDateTime(DateTime dt) { value = dt; }

		[XmlAttribute(AttributeName="value")]
		public DateTime value;

		static public NullableDateTime Now	{ get {	return new NullableDateTime(DateTime.Now);	} }

		static public object GetValue(NullableDateTime ndt)
		{
			if(ndt == null)
				return null;
			else
				return ndt.value;
		}

		static public object GetValue(NullableDateTime ndt,object ndtDefault)
		{
			if(ndt == null)
				return ndtDefault;
			else
				return ndt.value;
		}

		static public NullableDateTime CreateInstance(object o)
		{
			if(o == null) return null;
			if(o == DBNull.Value) return null;

			if(o is SqlDateTime)
			{
				SqlDateTime dt = (SqlDateTime)o;
				if(dt.IsNull) return null;
				
				return new NullableDateTime(dt.Value);
			}

			if(o is DateTime)
				return new NullableDateTime((DateTime)o);

			return new NullableDateTime(Convert.ToDateTime(o));
		}
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd",IsNullable=true)]
	public class NullableInt
	{
		public NullableInt() {}
		public NullableInt(int i) { value = i; }

		[XmlAttribute(AttributeName="value")]
		public int value;

		static public NullableInt CreateInstance(object o)
		{
			if(o == null) return null;
			if(o == DBNull.Value) return null;

			if(o is System.Int32)
				return new NullableInt((int)o);

			return new NullableInt(Convert.ToInt32(o));
		}

		static public object GetValue(NullableInt o)
		{
			if(o == null) return null;
			else return (object)o.value;
		}
	}

	public class BannerType
	{
		public BannerType() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idBannerType;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		[XmlElement(ElementName="Width")]
		public int iWidth;

		[XmlElement(ElementName="Height")]
		public int iHeight;
	}	

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class Banner
	{
		public Banner() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idBanner;

		[XmlAttribute(AttributeName="dbidBannerType")]
		public int idBannerType;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		[XmlElement(ElementName="DeviceLink",DataType="anyURI",IsNullable=true)]
		public string sDeviceLink;

		[XmlElement(ElementName="DateCreated",IsNullable=true)]
		public NullableDateTime dtCreated;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="CreatedBy",DataType="normalizedString",IsNullable=true)]
		public string sCreatedBy;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

		[XmlElement(ElementName="Status",IsNullable=false)]
		public StagingStatus byStatus;

		[XmlElement(IsNullable=false)]
		public MMDFile File;

		[XmlElement(IsNullable=true)]
		public BannerType BannerType;

		[XmlArray("ScheduleItems",IsNullable=true)]
		public ScheduleItem[] ScheduleItems;
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class IdleLoop
	{
		public IdleLoop() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idIdleLoop;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		[XmlElement(ElementName="Info",DataType="string",IsNullable=true)]
		public string sInfo;
	
		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;
	
		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

		[XmlElement(ElementName="DateCreated",IsNullable=true)]
		public NullableDateTime dtCreated;
	
		[XmlElement(ElementName="CreatedBy",DataType="normalizedString",IsNullable=true)]
		public string sCreatedBy;


		[XmlElement(ElementName="Status",IsNullable=false)]	
		public StagingStatus byStatus;
		
		[XmlArray("Frames",IsNullable=true)]
		[XmlArrayItem("Screen",typeof(ScreenMLFrame)),
		 XmlArrayItem("Video",typeof(VideoFrame)),
		 XmlArrayItem("Image",typeof(ImageFrame)),
		 XmlArrayItem("RecList",typeof(RecListFrame))]
		public IdleLoopElement[] Frames;
		
//		public ScheduleItem[] ScheduleItems;

		public IdleLoop(Location location, DataRow row, DataSetHolder ds)
		{	
			idIdleLoop = Convert.ToInt32(row["idIdleLoop"]);
			sName = Convert.ToString(row["sName"]);
			sInfo = Convert.ToString(row["sInfo"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
			if(location == Location.Staging)
				byStatus = (StagingStatus)Convert.ToInt32(row["byStatus"]);

			DataRow[] rows = row.GetChildRows("IL_ILE");
			Frames = new IdleLoopElement[rows.Length];
			for(int i=0;i<rows.Length;i++)
				Frames[i] = IdleLoopElement.CreateInstance(rows[i],ds);
		}
	
		public void Fill(Location location,DataRow row, DataSetHolder ds, bool bNew, CSWContext ctx)
		{
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

		public ScreenMLFrame() { frameType = FrameType.ScreenML; }

		public ScreenMLFrame(DataRow row, DataSetHolder ds) : base(row,ds)
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

	}

	public class VideoFrame : IdleLoopElement
	{
		[XmlElement("StartTime",IsNullable=true)]
		public NullableInt iStartTime;

		[XmlElement("EndTime",IsNullable=true)]
		public NullableInt iEndTime;

		public VideoFrame() { frameType = FrameType.Video; }
		public VideoFrame(DataRow row, DataSetHolder ds) : base(row,ds) 
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
	}

	public class ImageFrame : IdleLoopElement
	{
		[XmlElement("ShowTime")]
		public int cShowTime;

		[XmlElement("Link",IsNullable=true)]
		public string sLink;

		public ImageFrame() { frameType = FrameType.Image; }
		public ImageFrame(DataRow row, DataSetHolder ds) : base(row,ds)
		{
			frameType = FrameType.Image;
			cShowTime = Convert.ToInt32(row["cShowTime"]);
			sLink = Convert.ToString("sLink");
		}

		public override void Fill(DataRow row, DataRow rowParent, DataSetHolder ds, bool bNew, CSWContext ctx)
		{
			base.Fill(row,rowParent,ds,bNew,ctx);
			row["cShowTime"] = cShowTime;
			row["sLink"] = sLink;
		}
	}

	public class RecListFrame : IdleLoopElement
	{
		[XmlElement("dbidRecListType")]
		public int idRecListType;

		[XmlElement("dbidMediaType")]
		public int idMediaType;

		[XmlElement("ItemsPerPage")]
		public int cItemsPerPage;

		[XmlElement("ShowTime")]
		public int cShowTime;

		[XmlElement("Template",IsNullable=true)]
		public string sTemplate;

		public RecListFrame() { frameType = FrameType.RecList; }
		public RecListFrame(DataRow row, DataSetHolder ds) : base(row,ds)
		{
			idRecListType = Convert.ToInt32(row["idRecListType"]);
			idMediaType = Convert.ToInt32(row["idMediaType"]);
			cItemsPerPage = Convert.ToInt32(row["cItemsPerPage"]);
			cShowTime = Convert.ToInt32(row["cShowTime"]);
			sTemplate = Convert.ToString(row["sTemplate"]);
		}

		public override void Fill(DataRow row, DataRow rowParent, DataSetHolder ds, bool bNew, CSWContext ctx)
		{
			base.Fill(row,rowParent,ds,bNew,ctx);
			row["idRecListType"] = idRecListType;
			row["idMediaType"] = idMediaType;
			row["cItemsPerPage"] = cItemsPerPage;
			row["sTemplate"] = sTemplate;
		}

	}

	public class IdleLoopElement
	{
		public IdleLoopElement() {}
		
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

		protected IdleLoopElement(DataRow row, DataSetHolder ds)
		{
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
				DataRow[] rowFiles = rowFA[i].GetChildRows("FA_F");
				for(int j=0;j<rowFiles.Length;j++)
					al.Add(new MMDFile(rowFiles[j],ds));
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

		static public IdleLoopElement CreateInstance(DataRow row, DataSetHolder ds)
		{
			FrameType frameType = (FrameType)Convert.ToInt32(row["iFrameType"]);
			switch(frameType)
			{
				case FrameType.ScreenML : return new ScreenMLFrame(row,ds);
				case FrameType.Video : return new VideoFrame(row,ds);
				case FrameType.Image : return new ImageFrame(row,ds);
				case FrameType.RecList : return new RecListFrame(row,ds);
			}
			return null;
		}

	}

	public abstract class PersisterBase : CSWObjectBase
	{
		protected bool _bUpdating;
		protected Location _location;
		protected DataSetHolder _dsh;

		public DataSetHolder DataSet { get { return _dsh; } }

		public PersisterBase(CSWContext ctx, bool bUpdating) : base(ctx)
		{
			_bUpdating = bUpdating;
			_dsh = GetDataSet();
		}

		public PersisterBase(CSWContext ctx, bool bUpdating, Location location) : base(ctx)
		{
			_bUpdating = bUpdating;
			_location = location;
			_dsh = GetDataSet();
		}

		protected abstract DataSetHolder GetDataSet();
		protected abstract void UpdateDataSet();

	}

	public class MMDFilePersister : PersisterBase
	{
		public MMDFilePersister(CSWContext ctx, bool bUpdating) : base(ctx,bUpdating) {}

		protected override void UpdateDataSet()
		{
			_dsh.Update("RLType","idRecListType");
			_dsh.Update("RLTypeAttr","idRecListTypeAttribute");
			_dsh.Update("RLItemTypeAttr","idRecListTypeItemAttribute");
		}


		protected override DataSetHolder GetDataSet()
		{
			Data.Connection conn = Context.Config.MainDatabase;
			DataSetHolder dsh = new DataSetHolder();

			dsh.FillTable(conn,"File","select * from tblMMD_File");
			dsh.FillTable(conn,"FileAssociation","select * from tblMMD_FileAssociation");

			return dsh;
		}

		public MMDFile GetFileById(int idFile)
		{
			DataRow[] rows = _dsh.Tables["File"].Select("idFile = " + idFile);
			MMDFile file = new MMDFile(rows[0],_dsh);
			return file;
		}
		
	}


	public class MMDFile
	{
		public MMDFile() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idFile;

		[XmlElement(ElementName="MIMEType",DataType="normalizedString",IsNullable=false)]
		public string sMIMEType;

		[XmlElement(ElementName="URL",DataType="anyURI",IsNullable=false)]
		public string sURL;

		[XmlElement(ElementName="FileName",DataType="normalizedString",IsNullable=false)]
		public string sFileName;

		[XmlElement("Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		public MMDPostedFileType FileType;

		//readonly
		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

		[XmlElement(ElementName="AssetInfo",DataType="string",IsNullable=true)]
		public string sAssetInfo;

		public MMDFile(DataRow row, DataSetHolder dsh)
		{
			idFile = Convert.ToInt32(row["idFile"]);
			sName = Convert.ToString(row["sName"]);
			sMIMEType = Convert.ToString(row["sMIMEType"]);
			sFileName = Convert.ToString(row["sFileName"]);
			FileType = (MMDPostedFileType)Convert.ToInt32(row["iType"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
			sAssetInfo = Convert.ToString(row["sAssetInfo"]);
		}

		public void Fill(DataRow row, bool bNew)
		{
			//if(!bNew) row["idFile"] = idFile;
			row["sMIMEType"] = sMIMEType;
			row["sFileName"] = sFileName;
			row["iType"] = (int)FileType;
			row["dtChanged"] = dtChanged.value;
			row["sChangedBy"] = sChangedBy;
			row["sAssetInfo"] = sAssetInfo;
			row["sName"] = sName;
		}

	}
*/
	/*public class RecListBaseType
	{
		public RecListBaseType() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idRecListBaseType;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		public RecListBaseType(DataRow row)
		{
			idRecListBaseType = Convert.ToInt32(row["idRecListBaseType"]);
			sName = Convert.ToString(row["sName"]);
		}
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class RecListType
	{
		public RecListType() {}
		
		[XmlAttribute("dbid")]
		public int idRecListType;

		[XmlAttribute(AttributeName="dbidRecListBaseType")]
		public int idRecListBaseType;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		[XmlElement("baseType", IsNullable=false)]
		public RecListBaseType BaseType;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

		[XmlElement(ElementName="Status",IsNullable=false)]
		public StagingStatus byStatus;

		[XmlArray("RecListTypeAttributes", IsNullable=true)]
		public RecListTypeAttribute[] RecListAttributes;

		[XmlArray("RecListTypeItemAttributes", IsNullable=true)]
		public RecListTypeItemAttribute[] RecListItemAttributes;

		public RecListType(DataRow row, DataSetHolder dsh)
		{
			Initialize(row, dsh);
		}
		
		public void Initialize(DataRow row, DataSetHolder dsh)
		{
			idRecListType = Convert.ToInt32(row["idRecListType"]);
			idRecListBaseType = Convert.ToInt32(row["idRecListBaseType"]);
			sName = Convert.ToString(row["sName"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
			byStatus = (StagingStatus)Convert.ToInt32(row["byStatus"]);
			DataRow[] rows = row.GetChildRows("RLT_RLTA");
			RecListAttributes = new RecListTypeAttribute[rows.Length];
			for(int i = 0;i < rows.Length; i++)
				RecListAttributes[i] = new RecListTypeAttribute(rows[i], dsh);
			rows = row.GetChildRows("RLT_RLTIA");
			RecListItemAttributes = new RecListTypeItemAttribute[rows.Length];
			for(int i = 0;i < rows.Length; i++)
				RecListItemAttributes[i] = new RecListTypeItemAttribute(rows[i], dsh);
			rows = dsh.Tables["BaseType"].Select("idRecListBaseType = " + idRecListBaseType);
			if(rows.Length > 0)
				BaseType = new RecListBaseType(rows[0]);
		}

		public void Fill(DataRow row, DataSetHolder ds, bool bNew ,CSWContext ctx)
		{
			row["idRecListType"] = idRecListType;
			row["idRecListBaseType"] = idRecListBaseType;
			row["sName"] = sName;
			row["dtChanged"] = DateTime.Now;
			row["sChangedBy"] = ctx.UserName;
			foreach(RecListTypeAttribute rlta in RecListAttributes)
			{
				DataRow AttrRow = new DataRow();
			}
		}
	}

	public class MediaType
	{
		public MediaType() {}

		[XmlAttribute("dbid")]
		public int idMediaType;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		public MediaType(DataRow row, DataSetHolder dsh)
		{
			idMediaType = Convert.ToInt32(row["idMediaType"]);
			sName = Convert.ToString(row["sName"]);
		}
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class RecList
	{
		public RecList() {}

		[XmlAttribute("dbid")]
		public int idRecList;

		[XmlAttribute("dbidRecListType")]
		public int idRecListType;

		[XmlAttribute("dbidStoreConcept")]
		public int idStoreConcept;

		[XmlAttribute("dbidMediaType")]
		public int idMediaType;

		[XmlElement("Type",IsNullable=false)]
		public RecListType Type;

		//[XmlElement("Concept",IsNullable=true)]
		//public StoreConcept Concept;

		[XmlElement("MediaType",IsNullable=true)]
		public MediaType MType;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

		[XmlElement(ElementName="Status",IsNullable=false)]
		public StagingStatus byStatus;

		[XmlArray("RecListAttributes",IsNullable=true)]
		public RecListAttribute[] RecListAttributes;

		[XmlArray("RecListItems",IsNullable=true)]
		public RecListItem[] RecListItems;

		public RecList(DataRow row, DataSetHolder dsh)
		{
			idRecList = Convert.ToInt32(row["idRecList"]);
			idRecListType = Convert.ToInt32(row["idRecListType"]);
			idStoreConcept = Convert.ToInt32(row["idStoreConcept"]);
			idMediaType = Convert.ToInt32(row["idMediaType"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
			byStatus = (StagingStatus)Convert.ToInt32(row["byStatus"]);

			DataRow[] rows = row.GetChildRows("RL_RLI");
			RecListItems = new RecListItem[rows.Length];
			for(int i=0;i<rows.Length;i++)
				RecListItems[i] = new RecListItem(rows[i], dsh);
			rows = row.GetChildRows("RL_RLA");
			RecListAttributes = new RecListAttribute[rows.Length];
			for(int i = 0;i < rows.Length;i++)
				RecListAttributes[i] = new RecListAttribute(rows[i], dsh);
			rows = dsh.Tables["RLType"].Select("idRecListType = " + idRecListType);
			if(rows.Length > 0)
				Type = new RecListType(rows[0], dsh);
			else
				Type = null;
			rows = dsh.Tables["MediaType"].Select("idMediaType = " + idMediaType);
			if(rows.Length > 0)
				MType = new MediaType(rows[0], dsh);
			else
				MType = null;
		}

		public void Fill(DataRow row, DataSetHolder ds, bool bNew,CSWContext ctx)
		{
			row["sChangedBy"] = ctx.UserName;

			if(bNew)
			{
				row["dtCreated"] = DateTime.Now;
				row["dtChanged"] = null;
			}
			else
			{
				row["dtCreated"] = dtCreated.value;
				row["dtChanged"] = DateTime.Now;
			}

			row["dtDelta"] = DateTime.Now;
			row["sDeltaAction"] = (bNew) ? 'A' : 'C';
			row["idRecListType"] = RecListType.idRecListType;
			
			if(Genre == null)
				row["idGenre"] = null;
			else
				row["idGenre"] = Genre.idGenre;

			row["idMediaType"] = MediaType.idMediaType;

			if(Concept == null)
				row["idStoreConcept"] = null;
			else
				row["idStoreConcept"] = Concept.idStoreConcept;

			if(!bNew)
			{
				foreach(DataRow cRow in row.GetChildRows("RL_RLI"))
					cRow.Delete();
			}

			foreach(RecListItem rli in RecListItems)
			{
				DataRow cRow = ds.Tables["RecListItem"].NewRow();	
				rli.Fill(cRow,ds,row,true,ctx);
				ds.Tables["RecListItem"].Rows.Add(cRow);
			}
		}
	}

	public class RecListPersister : CSWObjectBase
	{
		protected bool _bUpdating;
		protected Location _location;
		protected DataSetHolder _dsh;

		public DataSetHolder DataSet { get { return _dsh; } }

		public RecListPersister(CSWContext ctx, bool bUpdating, Location location) : base(ctx)
		{
			_bUpdating = bUpdating;
			_location = location;
			_dsh = GetDataSet(bUpdating,location);
		}

		protected void UpdateDataSet()
		{
			_dsh.Update("RecList","idRecList");
			_dsh.Update("RLAttr","idRecListAttribute");
			_dsh.Update("RLItem","idRecListItem");
			_dsh.Update("RLItemAttr","idRecListItemAttribute");
		}


		protected DataSetHolder GetDataSet(bool bUpdating, Location location)
		{
			Data.Connection conn = Context.Config.MainDatabase;
			DataSetHolder dsh = new DataSetHolder();

			dsh.FillTable(conn,"BaseType","select * from tblMMD_RecommendListBaseType");
			dsh.FillTable(conn,"AttrType", "select * from tblMMD_RecommendListAttributeType");
			dsh.FillTable(conn,"RLType","select * from tblMMD_RecommendListTypeStaging");
			dsh.FillTable(conn,"RLTypeAttr","select * from tblMMD_RecommendListTypeAttributeStaging");
			dsh.FillTable(conn,"RLItemTypeAttr","select * from tblMMD_RecommendListTypeItemAttributeStaging");
			dsh.FillTable(conn,"MediaType","select * from tblMMD_MetaDataMediaType");

			dsh.AddTable(conn,"RecList","select * from tblMMD_RecommendListStaging",bUpdating);
			dsh.AddTable(conn,"RLAttr","select * from tblMMD_RecommendListAttributeStaging",bUpdating);
			dsh.AddTable(conn,"RLItem","select * from tblMMD_RecommendListItemStaging",bUpdating);
			dsh.AddTable(conn,"RLItemAttr","select * from tblMMD_RecommendListItemAttributeStaging",bUpdating);

			dsh.Relations.Add("RLT_RLTIA",dsh.Tables["RLType"].Columns["idRecListType"],dsh.Tables["RLItemTypeAttr"].Columns["idRecListType"]);
			dsh.Relations.Add("RLT_RLTA",dsh.Tables["RLType"].Columns["idRecListType"],dsh.Tables["RLTypeAttr"].Columns["idRecListType"]);
			dsh.Relations.Add("RL_RLA",dsh.Tables["RecList"].Columns["idRecList"],dsh.Tables["RLAttr"].Columns["idRecList"]);
			dsh.Relations.Add("RL_RLI",dsh.Tables["RecList"].Columns["idRecList"],dsh.Tables["RLItem"].Columns["idRecList"]);
			dsh.Relations.Add("RLI_RLIA",dsh.Tables["RLItem"].Columns["idRecListItem"],dsh.Tables["RLItemAttr"].Columns["idRecListItem"]);
	

			if(bUpdating)
			{
				ForeignKeyConstraint fkc = new ForeignKeyConstraint("C_RL_RLA",dsh.Tables["RecList"].Columns["idRecList"],dsh.Tables["RLAttr"].Columns["idRecList"]);
				fkc.DeleteRule = Rule.Cascade;
				fkc.UpdateRule = Rule.Cascade;
				dsh.Tables["RecList"].Constraints.Add(fkc);

				fkc = new ForeignKeyConstraint("C_RL_RLI",dsh.Tables["RecList"].Columns["idRecList"],dsh.Tables["RLItem"].Columns["idRecList"]);
				fkc.DeleteRule = Rule.Cascade;
				fkc.UpdateRule = Rule.Cascade;
				dsh.Tables["RecList"].Constraints.Add(fkc);

				fkc = new ForeignKeyConstraint("C_RLI_RLIA",dsh.Tables["RLItem"].Columns["idRecListItem"],dsh.Tables["RLItemAttr"].Columns["idRecListItem"]);
				fkc.DeleteRule = Rule.Cascade;
				fkc.UpdateRule = Rule.Cascade;
				dsh.Tables["RLItem"].Constraints.Add(fkc);
			
				dsh.EnforceConstraints = true;
			}

			return dsh;
		}

		public RecList GetRecListById(int idRecList)
		{
			DataRow[] rows = _dsh.Tables["RecList"].Select("idRecList = " + idRecList);
			RecList rl = new RecList(rows[0],_dsh);
			return rl;
		}

		public RecList[] GetRecLists()
		{
			DataRow[] rows = _dsh.Tables["RecList"].Select();
			RecList[] rls = new RecList[rows.Length];
			for(int i=0;i < rows.Length;i++)
				rls[i] = new RecList(rows[i], _dsh);
			return rls;
		}

		public RecList[] GetRecListsByType(int idRecListType)
		{
			DataRow[] rows = _dsh.Tables["RecList"].Select("idRecListTypes = " + idRecListType);
			RecList[] rls = new RecList[rows.Length];
			for(int i = 0;i < rows.Length;i++)
				rls[i] = new RecList(rows[i], _dsh);
			return rls;
		}

		public void Update(RecList rl)
		{
			UpdateHelper(rl);
			UpdateDataSet();
		}

		public void Update(RecList[] rls)
		{
			for(int i=0;i<rls.Length;i++)
				UpdateHelper(rls[i]);
			UpdateDataSet();
		}

		protected void UpdateHelper(RecList rl)
		{
			DataRow[] rows = _dsh.Tables["RecList"].Select("idRecList = " + rl.idRecList);
			DataRow row;
			bool bNew = rows.Length == 0;
			row = (bNew) ? _dsh.Tables["RecList"].NewRow() : rows[0];
			rl.Fill(row,_dsh,bNew,this.Context);
		}

		public void RemoveLists(int[] idRecLists)
		{
			foreach(int i in idRecLists)
			{
				DataRow r = _dsh.Tables["RecList"].Rows.Find(idRecLists[i]);
				r.Delete();
				if(r != null) _dsh.Tables["RecList"].Rows.Remove(r);
			}
			UpdateDataSet();
		}

	}

	public class RecListTypePersister : CSWObjectBase
	{
		protected bool _bUpdating;
		protected Location _location;
		protected DataSetHolder _dsh;

		public DataSetHolder DataSet { get { return _dsh; } }

		public RecListTypePersister(CSWContext ctx, bool bUpdating, Location location) : base(ctx)
		{
			_bUpdating = bUpdating;
			_location = location;
			_dsh = GetDataSet(bUpdating,location);
		}

		protected void UpdateDataSet()
		{
			_dsh.Update("RLType","idRecListType");
			_dsh.Update("RLTypeAttr","idRecListTypeAttribute");
			_dsh.Update("RLItemTypeAttr","idRecListTypeItemAttribute");
		}


		protected DataSetHolder GetDataSet(bool bUpdating, Location location)
		{
			Data.Connection conn = Context.Config.MainDatabase;
			DataSetHolder dsh = new DataSetHolder();

			dsh.FillTable(conn,"BaseType","select * from tblMMD_RecommendListBaseType");
			dsh.FillTable(conn,"AttrType", "select * from tblMMD_RecommendListAttributeType");
		
			dsh.AddTable(conn,"RLType","select * from tblMMD_RecommendListTypeStaging",bUpdating);
			dsh.AddTable(conn,"RLTypeAttr","select * from tblMMD_RecommendListTypeAttributeStaging",bUpdating);
			dsh.AddTable(conn,"RLItemTypeAttr","select * from tblMMD_RecommendListTypeItemAttributeStaging",bUpdating);

			dsh.Relations.Add("RLT_RLTIA",dsh.Tables["RLType"].Columns["idRecListType"],dsh.Tables["RLItemTypeAttr"].Columns["idRecListType"]);
			dsh.Relations.Add("RLT_RLTA",dsh.Tables["RLType"].Columns["idRecListType"],dsh.Tables["RLTypeAttr"].Columns["idRecListType"]);
	

			if(bUpdating)
			{
				ForeignKeyConstraint fkc = new ForeignKeyConstraint("C_RLT_RLTIA",dsh.Tables["RLType"].Columns["idRecListType"],dsh.Tables["RLTypeItemAttr"].Columns["idRecListType"]);
				fkc.DeleteRule = Rule.Cascade;
				fkc.UpdateRule = Rule.Cascade;
				dsh.Tables["RLType"].Constraints.Add(fkc);

				fkc = new ForeignKeyConstraint("C_RLT_RLTA",dsh.Tables["RLType"].Columns["idRecListType"],dsh.Tables["RLTypeAttr"].Columns["idRecListType"]);
				fkc.DeleteRule = Rule.Cascade;
				fkc.UpdateRule = Rule.Cascade;
				dsh.Tables["RLType"].Constraints.Add(fkc);
			
				dsh.EnforceConstraints = true;
			}

			return dsh;
		}

		public RecListType GetRecListTypeById(int idRecListType)
		{
			DataRow[] rows = _dsh.Tables["RLType"].Select("idRecListType = " + idRecListType);
			RecListType rlt = new RecListType(rows[0],_dsh);
			return rlt;
		}

		public RecListType[] GetRecListTypes()
		{
			DataRow[] rows = _dsh.Tables["RLType"].Select();
			RecListType[] rlts = new RecListType[rows.Length];
			for(int i=0;i<rows.Length;i++)
				rlts[i] = new RecListType(rows[i],_dsh);
			return rlts;
		}

		public void Update(RecListType rlt)
		{
			UpdateHelper(rlt);
			UpdateDataSet();
		}

		public void Update(RecListType[] rlts)
		{
			for(int i=0;i<rlts.Length;i++)
				UpdateHelper(rlts[i]);
			UpdateDataSet();
		}

		protected void UpdateHelper(RecListType rlt)
		{
			DataRow[] rows = _dsh.Tables["RLType"].Select("idRecListType = " + rlt.idRecListType);
			DataRow row;
			bool bNew = rows.Length == 0;
			row = (bNew) ? _dsh.Tables["RLType"].NewRow() : rows[0];
			rlt.Fill(row,_dsh,bNew,this.Context);
		}

		public void RemoveTypes(int[] idRecListTypes)
		{
			string s = "0 ";
			foreach(int i in idRecListTypes)
				s += ", " + i + " ";

			DataRow[] rows = _dsh.Tables["RLType"].Select("idRecListType IN (" + s + ")");
			foreach(Datarow row in rows)
				_dsh.Tables["RLType"].Rows.Remove(row);

			foreach(int i in idRecListTypes)
			{
				DataRow r = _dsh.Tables["RLType"].Rows.Find(idRecListTypes[i]);
				r.Delete();
				if(r != null) _dsh.Tables["RLType"].Rows.Remove(r);
			}
			UpdateDataSet();
		}
	}

	public class RecListAttributeType
	{
		public RecListAttributeType() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idRecListAttributeType;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		public RecListAttributeType(DataRow row, DataSetHolder dsh)
		{
			idRecListAttributeType = Convert.ToInt32(row["idRecListAttributeType"]);
			sName = Convert.ToString(row["sName"]);
		}
	}

	public class RecListTypeItemAttribute
	{
		public RecListTypeItemAttribute() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idRecListTypeItemAttribute;

		[XmlAttribute(AttributeName="dbidRecListType")]
		public int idRecListType;

		[XmlAttribute(AttributeName="dbidRecListAttributeType")]
		public int idRecListAttributeType;

		[XmlAttribute(AttributeName="required")]
		public bool bRequired;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		[XmlElement("AttributeType",IsNullable=false)]
		public RecListAttributeType AttributeType;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

		[XmlElement(ElementName="Status",IsNullable=false)]
		public StagingStatus byStatus;

		public RecListTypeItemAttribute(DataRow row, DataSetHolder dsh)
		{
			Initialize(row, dsh);
		}
		
		public void Initialize(DataRow row, DataSetHolder dsh)
		{
			idRecListTypeItemAttribute = Convert.ToInt32(row["idRecListTypeItemAttribute"]);
			idRecListType = Convert.ToInt32(row["idRecListType"]);
			idRecListAttributeType = Convert.ToInt32(row["idRecListAttributeType"]);
			bRequired = Convert.ToBoolean(row["bRequired"]);
			sName = Convert.ToString(row["sName"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
			DataRow[] rows = dsh.Tables["AttrType"].Select("idRecListAttributeType = " + idRecListAttributeType);
			if(rows.Length > 0)
				AttributeType = new RecListAttributeType(rows[0], dsh);
		}

		public void Fill(DataRow row, DataSetHolder ds, bool bNew ,CSWContext ctx)
		{
			row["idRecListTypeItemAttribute"] = idRecListTypeItemAttribute;
			row["idRecListType"] = idRecListType;
			row["idRecListAttributeType"] = idRecListAttributeType;
			row["bRequired"] = bRequired;
			row["sName"] = sName;
			row["dtChanged"] = DateTime.Now;
			row["sChangedBy"] = ctx.UserName;
			row["dtDelta"] = DateTime.Now;
			row["sDeltaAction"] = (bNew) ? 'A' : 'C';
		}
	}

	public class RecListTypeAttribute
	{
		public RecListTypeAttribute() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idRecListTypeAttribute;

		[XmlAttribute(AttributeName="dbidRecListType")]
		public int idRecListType;

		[XmlAttribute(AttributeName="dbidRecListAttributeType")]
		public int idRecListAttributeType;

		[XmlAttribute(AttributeName="required")]
		public bool bRequired;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		[XmlElement("AttributeType",IsNullable=false)]
		public RecListAttributeType AttributeType;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

		[XmlElement(ElementName="Status",IsNullable=false)]
		public StagingStatus byStatus;

		public RecListTypeAttribute(DataRow row, DataSetHolder dsh)
		{
			Initialize(row, dsh);
		}
		
		public void Initialize(DataRow row, DataSetHolder dsh)
		{
			idRecListTypeAttribute = Convert.ToInt32(row["idRecListTypeAttribute"]);
			idRecListType = Convert.ToInt32(row["idRecListType"]);
			idRecListAttributeType = Convert.ToInt32(row["idRecListAttributeType"]);
			bRequired = Convert.ToBoolean(row["bRequired"]);
			sName = Convert.ToString(row["sName"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
			DataRow[] rows = dsh.Tables["AttrType"].Select("idRecListAttributeType = " + idRecListAttributeType);
			if(rows.Length > 0)
				AttributeType = new RecListAttributeType(rows[0], dsh);
		}

		public void Fill(DataRow row, DataSetHolder ds, bool bNew ,CSWContext ctx)
		{
			row["idRecListTypeAttribute"] = idRecListTypeAttribute;
			row["idRecListType"] = idRecListType;
			row["idRecListAttributeType"] = idRecListAttributeType;
			row["bRequired"] = bRequired;
			row["sName"] = sName;
			row["dtChanged"] = DateTime.Now;
			row["sChangedBy"] = ctx.UserName;
			row["dtDelta"] = DateTime.Now;
			row["sDeltaAction"] = (bNew) ? 'A' : 'C';
		}
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class RecListAttribute
	{
		public RecListAttribute() {}
		
		[XmlAttribute("dbid")]
		public int idRecListAttribute;

		[XmlAttribute(AttributeName="dbidRecList")]
		public int idRecList;

		[XmlAttribute("dbidRecListAttributeType")]
		public int idRecListAttributeType;

		[XmlElement("sValue",IsNullable=true)]
		public string sValue;

		[XmlElement("txValue",IsNullable=true)]
		public string txValue;

		[XmlElement("AttributeType",IsNullable=false)]
		public RecListAttributeType AttributeType;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

		[XmlElement(ElementName="Status",IsNullable=false)]
		public StagingStatus byStatus;

		public RecListAttribute(DataRow row, DataSetHolder dsh)
		{
			Initialize(row, dsh);
		}
		
		public void Initialize(DataRow row, DataSetHolder dsh)
		{
			idRecListAttribute = Convert.ToInt32(row["idRecListAttribute"]);
			idRecList = Convert.ToInt32(row["idRecList"]);
			idRecListAttributeType = Convert.ToInt32(row["idRecListAttributeType"]);
			sValue = Convert.ToString(row["sValue"]);
			txValue = Convert.ToString(row["txValue"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
			DataRow[] rows = dsh.Tables["AttrType"].Select("idRecListAttributeType = " + idRecListAttributeType);
			if(rows.Length > 0)
				AttributeType = new RecListAttributeType(rows[0], dsh);
		}

		public void Fill(DataRow row, DataSetHolder ds, bool bNew ,CSWContext ctx)
		{
			row["idRecListAttribute"] = idRecListAttribute;
			row["sName"] = sName;
			row["dtChanged"] = DateTime.Now;
			row["sChangedBy"] = ctx.UserName;
			row["dtDelta"] = DateTime.Now;
			row["sDeltaAction"] = (bNew) ? 'A' : 'C';
		}
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class RecListItemAttribute
	{
		public RecListItemAttribute() {}
		
		[XmlAttribute("dbid")]
		public int idRecListItemAttribute;

		[XmlAttribute(AttributeName="dbidRecListItem")]
		public int idRecListItem;

		[XmlAttribute("dbidRecListAttributeType")]
		public int idRecListAttributeType;

		[XmlElement("sValue",IsNullable=true)]
		public string sValue;

		[XmlElement("txValue",IsNullable=true)]
		public string txValue;

		[XmlElement("AttributeType",IsNullable=false)]
		public RecListAttributeType AttributeType;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

		[XmlElement(ElementName="Status",IsNullable=false)]
		public StagingStatus byStatus;

		public RecListItemAttribute(DataRow row, DataSetHolder dsh)
		{
			Initialize(row, dsh);
		}
		
		public void Initialize(DataRow row, DataSetHolder dsh)
		{
			idRecListItemAttribute = Convert.ToInt32(row["idRecListItemAttribute"]);
			idRecListItem = Convert.ToInt32(row["idRecListItem"]);
			idRecListAttributeType = Convert.ToInt32(row["idRecListAttributeType"]);
			sValue = Convert.ToString(row["sValue"]);
			txValue = Convert.ToString(row["txValue"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
			DataRow[] rows = dsh.Tables["AttrType"].Select("idRecListAttributeType = " + idRecListAttributeType);
			if(rows.Length > 0)
				AttributeType = new RecListAttributeType(rows[0],dsh);
		}

		public void Fill(DataRow row, DataSetHolder ds, bool bNew ,CSWContext ctx)
		{
			row["idRecListAttribute"] = idRecListAttribute;
			row["sName"] = sName;
			row["dtChanged"] = DateTime.Now;
			row["sChangedBy"] = ctx.UserName;
			row["dtDelta"] = DateTime.Now;
			row["sDeltaAction"] = (bNew) ? 'A' : 'C';
		}
	}
	
	public class RecListItem
	{
		public RecListItem() {}

		[XmlAttribute("dbid")]
		public int idRecListItem;

		[XmlAttribute("dbidRecList")]
		public int idRecList;

		[XmlElement("item",IsNullable=false)]
		public int idItem;

		[XmlElement("sequence",IsNullable=false)]
		public int iSeq;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

		[XmlElement(ElementName="Status",IsNullable=false)]
		public StagingStatus byStatus;

		[XmlArray("RecListItemAttributes",IsNullable=true)]
		public RecListItemAttribute[] RecListItemAttributes;

		public RecListItem(DataRow row, DataSetHolder dsh)
		{
			idRecListItem = Convert.ToInt32(row["idRecListItem"]);
			idRecList = Convert.ToInt32(row["idRecList"]);
			idItem = Convert.ToInt32(row["idItem"]);
			iSeq = Convert.ToInt32(row["iSeq"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
			DataRow[] rows = row.GetChildRows("RLI_RLIA");
			RecListItemAttributes = new RecListItemAttribute[rows.Length];
			for(int i = 0;i < rows.Length;i++)
				RecListItemAttributes[i] = new RecListItemAttribute(row, dsh);
		}

		public void Fill(DataRow row, DataSetHolder ds, DataRow pRow, bool bNew,CSWContext ctx)
		{
			row.SetParentRow(pRow);

			if(sUPC != null)
			{
				DataSet dsItem = ctx.Config.MainDatabase.execSQL_DS("select idItem where sUPC = '" + sUPC + "'");
				if(dsItem.Tables[0].Rows.Count < 1)
					throw new ApplicationException("Unknown UPC: " + sUPC);
				row["idItem"] = dsItem.Tables[0].Rows[0]["idItem"];
			}
			else if(sXref != null)
			{
				DataSet dsItem = ctx.Config.MainDatabase.execSQL_DS("select idItem where sXref = '" + sXref + "'");
				if(dsItem.Tables[0].Rows.Count < 1)
					throw new ApplicationException("Unknown Xref: " + sXref);

				row["idItem"] = dsItem.Tables[0].Rows[0]["sXref"];
			}
			else
				throw new ApplicationException("Must specify a UPC or XREF for each RecListItem");

			row["iSeq"] = iSeq;
			row["dtChanged"] = DateTime.Now;
			row["sChangedBy"] = ctx.UserName;
			row["dtDelta"] = DateTime.Now;
			row["sDeltaAction"] = (bNew) ? 'A' : 'C';

		}
	}*/

/*
	public class ProductType
	{
		public ProductType() {}

		[XmlAttribute("dbid")]
		public int idProductType;

		[XmlElement("Name",DataType="normalizedString")]
		public string sName;
	}

	public class Genre
	{
		public Genre() {}
	
		[XmlAttribute("dbid")]
		public int idGenre;

		private int idMediaType;

		public MediaType MediaType;

		[XmlElement("Name",IsNullable=true)]
		public string sName;

		[XmlElement("DisplayName",IsNullable=true)]
		public string sDescription;

		//		[XmlAttribute("UseInDevice")]
		//		public bool bUseInDevice;

		public Genre(DataRow row, DataSet ds)
		{
			idGenre = Convert.ToInt32(row["idGenre"]);
			idMediaType = Convert.ToInt32(row["idMediaType"]);
			sName = Convert.ToString(row["sName"]);
			sDescription = Convert.ToString(row["sDescription"]);

			DataRow[] rows = ds.Tables["MediaType"].Select("idMediaType = " + idMediaType);
			//if(rows.Length > 0) MediaType = new MediaType(rows[0]);
		}
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class Product
	{
		public Product() {}

		[XmlAttribute("dbid")]
		public int idProduct;

		[XmlElement("UPC")]
		public string sUPC;

		[XmlElement("Name",IsNullable=true)]
		public string sName;

		[XmlElement("Title",IsNullable=true)]
		public string sTitle;

		[XmlElement("DateCopyrighted",IsNullable=true)]
		public NullableDateTime dtCopyright;

		[XmlElement("DateReleased",IsNullable=true)]
		public NullableDateTime dtRelease;

		[XmlElement("DateCreated",IsNullable=true)]
		public NullableDateTime dtCreated;
		

		public Product(DataRow row)
		{
			idProduct = Convert.ToInt32(row["idProduct"]);
			sUPC = Convert.ToString(row["sUPC"]);
			sName = Convert.ToString(row["sName"]);
			sTitle = Convert.ToString(row["sTitle"]);
			dtCopyright = NullableDateTime.CreateInstance(row["dtCopyright"]);
			dtRelease = NullableDateTime.CreateInstance(row["dtRelease"]);
			dtCreated = NullableDateTime.CreateInstance(row["dtCreated"]);
		}
	}
*/
	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class ReportObject
	{	
		public ReportObject() {}

		[XmlAttribute("roid",DataType="NMTOKEN")]
		public string sReportObjectId;

		[XmlElement("ReportObject")]
		public ReportObject reportObject;

		[XmlElement("Attr")]
		public ReportAttribute[] ReportAttributes;

		[XmlElement("Agg")]
		public ReportAggregate[] ReportAggregates;

		[XmlElement("Criterion")]
		public ReportCriterion[] ReportCriteria;

		[XmlElement("Sort")]
		public ReportSort[] ReportSorts;
	}

	public class ReportAttribute
	{
		public ReportAttribute() {}

		[XmlAttribute("atid",DataType="NMTOKEN")]
		public string sReportAttrId;

		[XmlAttribute("name",DataType="normalizedString")]
		public string sName;
	}

	public class ReportAggregate
	{
		public ReportAggregate() {}

		[XmlAttribute("agid",DataType="NMTOKEN")]
		public string sReportAggId;

		[XmlAttribute("name",DataType="normalizedString")]
		public string sName;

		[XmlAttribute("roid",DataType="NMTOKEN")]
		public string sReportObjectId;
	}

	public class ReportCriterion
	{
		public ReportCriterion() {}

		[XmlAttribute("atid",DataType="NMTOKEN")]
		public string sReportAttrId;

		[XmlAttribute("agid",DataType="NMTOKEN")]
		public string sReportAggId;

		[XmlAttribute("op",DataType="NMTOKEN")]
		public string sOp;

		[XmlAttribute("operand",DataType="normalizedString")]
		public string sOperand;
	}

	public class ReportSort
	{
		public ReportSort() {}

		[XmlAttribute("atid",DataType="NMTOKEN")]
		public string sReportAttrId;

		[XmlAttribute("agid",DataType="NMTOKEN")]
		public string sReportAggId;

		[XmlAttribute("sort")]
		public SortOrder sortOrder;
	}
/*
	public enum TaskRunSchedule
	{
		Periodic = 0,
		Daily = 1,
		RunOnce = 2
	}

	public enum TaskRunStatus
	{
		Disabled = 0,
		Ready = 1,
		Running = 2
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
		public class Task
	{
		public Task() {}

		[XmlAttribute("dbid")]
		public int idTask;

		[XmlElement("Description")]
		public string sDescription;

		[XmlElement("Type")]
		public TaskRunSchedule eType;

		[XmlElement("Schedule",IsNullable=true)]
		public ScheduleItem Schedule;
	
		[XmlElement("PeriodMinutes",IsNullable=true)]
		public NullableInt iPeriodMinutes;

		[XmlElement("TimeOfDay",IsNullable=true)]
		public NullableDateTime dtTimeOfDay;

		[XmlElement("RunOnDayOfWeek",IsNullable=true)]
		public XmlObjects.RunOnDayOfWeek RunOnDayOfWeek;

		[XmlElement("Priority",IsNullable=false)]
		public int iPriority;

		[XmlElement("Status",IsNullable=false)]
		public TaskRunStatus eStatus;

		[XmlElement("DateCreated",IsNullable=true)]
		public NullableDateTime dtCreated;

		[XmlElement("DateModified",IsNullable=true)]
		public NullableDateTime dtModified;

		[XmlElement("DateLastRan",IsNullable=true)]
		public NullableDateTime dtLastRan;

		[XmlElement("Action")]
		public TaskAction[] Action;
	}

	public class TaskAction
	{
		public TaskAction() {}

		[XmlAttribute("dbid")]
		public int idAction;

		[XmlAttribute("task")]
		public int idTask;

		[XmlAttribute("Sequence")]
		public int iSequence;

		[XmlElement("Command",IsNullable=false)]
		public string sCommand;

		[XmlElement("Description",IsNullable=true)]
		public string sDescription;

		[XmlElement("DateCreated",IsNullable=true)]
		public NullableDateTime dtCreated;

		[XmlElement("DateModified",IsNullable=true)]
		public NullableDateTime dtModified;

		[XmlElement("DateLastRan",IsNullable=true)]
		public NullableDateTime dtLastRan;

	}

	public class TaskHistory
	{
		[XmlAttribute("dbidTask")]
		public int idTask;

		[XmlElement("DateStarted")]
		public DateTime dtStarted;

		[XmlElement("DateEnded")]
		public DateTime dtEnded;

		[XmlElement("CompletionStatus")]
		public NullableInt iCompletionStatus;

		[XmlElement("CompletionMessage")]
		public string sCompletionMessage;

		[XmlElement("ActionHistory")]
		public TaskActionHistory[] taskActionHistory;
	}

	public class TaskActionHistory
	{
		[XmlAttribute("dbidAction")]
		public int idAction;

		[XmlAttribute("dbidTask")]
		public int idTask;

		[XmlElement("DateStarted")]
		public DateTime dtStarted;

		[XmlElement("DateEnded")]
		public DateTime dtEnded;

		[XmlElement("CompletionStatus")]
		public NullableInt iCompletionStatus;

		[XmlElement("CompletionMessage")]
		public string sCompletionMessage;	
	}

	public class Package
	{
		public Package() {}

		[XmlAttribute("dbid")]
		public int idPackage;

		[XmlAttribute("incremental")]
		public bool bIncremental;

		[XmlAttribute("archived")]
		public bool bArchived;

		[XmlElement("DateBuilt")]
		public DateTime dtPackageBuilt;

		[XmlElement("BuildNumber")]
		public int iBuildNumber;

		[XmlElement("Version")]
		public string sVersion;

		[XmlElement("FileName")]
		public string sFileName;

		[XmlElement("BuiltBy")]
		public string sBuiltBy;
	}

	public class Store
	{
		public Store() {}

		[XmlAttribute("dbid")]
		public int idStore;

		[XmlAttribute("concept")]
		public int idStoreConcept;

		[XmlElement("DateLastLogUpdate")]
		public DateTime dtLastLogUpdate;

		private int idCurrentPackage;

		[XmlElement("CurrentPackage")]
		public Package CurrentPackage;

		[XmlElement("StoreConcept")]
		public StoreConcept storeConcept;

		[XmlElement("GroupType")]
		public StoreGroupType[] GroupTypes;

		
	}

	public class StoreConcept
	{
		public StoreConcept() {}
		
		[XmlAttribute("dbid")]
		public int idStoreConcept;

		[XmlElement("Name",DataType="normalizedString")]
		public string sName;

		[XmlElement("Description",IsNullable=true)]
		public string sInfo;

		[XmlElement("UpdateProductType")]
		public ProductType[] UpdateProductTypes;

		public StoreConcept(DataRow row)
		{
			idStoreConcept = Convert.ToInt32(row["idStoreConcept"]);
			sName = Convert.ToString(row["sName"]);
			sInfo = Convert.ToString(row["sInfo"]);
		}
	}

	public class StoreGroupType
	{
		public StoreGroupType() {}

		[XmlAttribute("dbid")]
		public int idGroupType;

		[XmlElement("Name",DataType="normalizedString")]
		public string sTypeName;

		[XmlElement("StoreGroup",IsNullable=true)]
		public StoreGroup[] StoreGroups;
	}

	public class StoreGroup
	{
		public StoreGroup() {}
		
		[XmlAttribute("dbid")]
		public int idGroup;

		[XmlAttribute("dbidGroupType")]
		public int idGroupType;
		
		[XmlElement("Name",DataType="normalizedString")]
		public string sName;

		[XmlElement("Description",IsNullable=true)]
		public string sInfo;
		
	}
*/
}
