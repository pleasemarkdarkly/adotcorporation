using Microsoft.Win32;
using System;
using System.Collections;
using System.Data;
using System.Data.SqlClient;
using System.Web;
using System.Xml.Serialization;
using MMD.Central.CentralLib;
using MMD.Central.CentralLib.Data;
using MMD.Central.CentralLib.Utilities;

namespace MMD.Central.CentralLib.XmlObjects
{

	public class MMDFilePersister : PersisterBase
	{
		//public MMDFilePersister(CSWContext ctx, bool bUpdating) : base(ctx,bUpdating) {}
		public MMDFilePersister(CSWContext ctx, bool bUpdating, Location location) : base(ctx, bUpdating, location) {}

		protected override void UpdateDataSet()
		{
			_dsh.EnforceConstraints = false;
			_dsh.Update("File","idFile");
			_dsh.EnforceConstraints = true;
//			_dsh.Update("RLTypeAttr","idRecListTypeAttribute");
//			_dsh.Update("RLItemTypeAttr","idRecListTypeItemAttribute");
//			_dsh.Update("FileAssociation");
		}


		protected override DataSetHolder GetDataSet()
		{
			Data.Connection conn = Context.Config.MainDatabase;
			DataSetHolder dsh = new DataSetHolder();

			
			dsh.AddTable(conn,"File","select * from tblMMD_File",this._bUpdating);
			string sTable = (_location == Location.Staging) ? "tblMMD_FileAssociationStaging" : "tblMMD_FileAssociation";
			dsh.FillTable(conn,"FileAssociation","select * from " + sTable);

			dsh.Relations.Add("F_FA",dsh.Tables["File"].Columns["idFile"],dsh.Tables["FileAssociation"].Columns["idFile"]);

			return dsh;
		}

		public MMDFile GetFileById(int idFile)
		{
			DataRow[] rows = _dsh.Tables["File"].Select("idFile = " + idFile);
			
			if(rows.Length == 0)
				throw new CSWException("Invalid File ID");
			
			MMDFile file = new MMDFile(rows[0],_dsh,this.Context);
			return file;
		}
		
		public MMDFile[] GetAllFiles()
		{
			DataRowCollection rows = _dsh.Tables["File"].Rows;
			MMDFile[] files = new MMDFile[rows.Count];
			for(int i=0;i<rows.Count;i++)
				files[i] = new MMDFile(rows[i],_dsh,this.Context);
			return files;
		}

		public void RemoveFiles(int[] idFiles)
		{
			//Validate file exists, and has no associations
			foreach(int idFile in idFiles)
			{
				DataRow row = _dsh.Tables["File"].Rows.Find(idFile);
				if(row == null)
					throw new CSWException("Invalid File ID (" + idFile + ")");
				MMDFile file = new MMDFile(row,_dsh,Context);
				if(file.Associations.Length > 0)
					throw new CSWException("Cannot delete files with associations (" + idFile + ")");
			}

			foreach(int idFile in idFiles)
			{
				DataRow row = _dsh.Tables["File"].Rows.Find(idFile);
				MMDFile file = new MMDFile(row,_dsh,Context);
				file.DeleteFile(Context);
				row.Delete();
			}
			UpdateDataSet();
		}


		public MMDFile RegisterFile(int idStoreConcept, MMDPostedFileType fileType, string sFileName, string sName)
		{
			//check if file exists
			if(!System.IO.File.Exists(Context.Config.GetLocalPath(idStoreConcept,fileType,sFileName)))
				throw new CSWException("File does not exists (" + Context.Config.GetWebPath(idStoreConcept,fileType,sFileName));

			//make sure its not already registered
			DataRow[] rows = _dsh.Tables["File"].Select("idStoreConcept = " + idStoreConcept + " and iType = " + (int)fileType + " and sFileName = '" + sFileName + "'");
			if(rows.Length > 0)
				throw new CSWException("File already registered (" + rows[0]["idFile"] + ")");

			//check if valid mimetype
			string sMIMEType = PostedFile.GetMIMEType(sFileName);
			switch(fileType)
			{
				case MMDPostedFileType.FeatureArticle :
					PostedFile.ValidateImagePosting(sMIMEType);
					break;
				case MMDPostedFileType.IdleLoopElement :
					PostedFile.ValidateFramePosting(sMIMEType);
					break;
				case MMDPostedFileType.RecListAttribute :
					PostedFile.ValidateImagePosting(sMIMEType);
					break;
				default:
					throw new CSWException("Invalid File Type");
			}

			MMDFile f = new MMDFile();
			f.dtChanged = NullableDateTime.Now;
			f.FileType = fileType;
			f.idFile = 0;
			f.idStoreConcept = idStoreConcept;
			f.sChangedBy = "WebService";
			f.sName = sName;
			f.MediaType = PostedFile.GetMediaType(sMIMEType);			
			f.sMIMEType = sMIMEType;
			f.sAssetInfo = "";

			UpdateFile(f);
			rows = _dsh.Tables["File"].Select("idStoreConcept = " + idStoreConcept + " and iType = " + (int)fileType + " and sFileName = '" + sFileName + "'");
			f.idFile = Convert.ToInt32(rows[0]["idFile"]);
			return f;
		}

		public void UpdateFile(MMDFile f)
		{
			DataRow[] rows = _dsh.Tables["File"].Select("idFile = " + f.idFile);
			DataRow row;
			bool bNew = rows.Length == 0;

			row = (bNew) ? _dsh.Tables["RecList"].NewRow() : rows[0];
			f.Fill(row,bNew,_dsh,this.Context);
			
		}
	}


	public class MMDFile
	{
		public MMDFile() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idFile;

		[XmlAttribute(AttributeName="dbidStoreConcept")]
		public int idStoreConcept;

		[XmlElement(ElementName="MIMEType",DataType="normalizedString",IsNullable=false)]
		public string sMIMEType;

		[XmlElement(ElementName="URL",DataType="anyURI",IsNullable=false)]
		public string sURL;

		[XmlElement(ElementName="FileName",DataType="normalizedString",IsNullable=false)]
		public string sFileName;

		[XmlElement("Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		public MMDPostedFileType FileType;

		public FileMediaType MediaType;

		//readonly
		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

		[XmlElement(ElementName="AssetInfo",DataType="string",IsNullable=true)]
		public string sAssetInfo;

		public MMDFileAssociation[] Associations;

		public MMDFile(DataRow row, DataSetHolder dsh,CSWContext ctx)
		{
			idFile = Convert.ToInt32(row["idFile"]);
			idStoreConcept = Convert.ToInt32(row["idStoreConcept"]);
			sName = Convert.ToString(row["sName"]);
			sMIMEType = Convert.ToString(row["sMIMEType"]);
			sFileName = Convert.ToString(row["sFileName"]);
			FileType = (MMDPostedFileType)Convert.ToInt32(row["iType"]);
			MediaType = (FileMediaType)Convert.ToInt32(row["iFileMediaType"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
			sAssetInfo = Convert.ToString(row["sAssetInfo"]);
			sURL = ctx.Config.GetWebPath(idStoreConcept,FileType,sFileName);

			Associations = null;
			DataRow[] rows = row.GetChildRows("F_FA");
			if(rows.Length > 0)
			{
				Associations = new MMDFileAssociation[rows.Length];
				for(int i=0;i<rows.Length;i++)
					Associations[i] = new MMDFileAssociation(rows[i],dsh);
			}
		}

		public void Fill(DataRow row, bool bNew, DataSetHolder dsh, CSWContext ctx )
		{
			//if(!bNew) row["idFile"] = idFile;
			row["idStoreConcept"] = idStoreConcept;
			row["sMIMEType"] = sMIMEType;
			row["sFileName"] = sFileName;
			row["iType"] = (int)FileType;
			row["iFileMediaType"] = (int)MediaType;
			row["dtChanged"] = dtChanged.value;
			row["sChangedBy"] = sChangedBy;
			row["sAssetInfo"] = sAssetInfo;
			row["sName"] = sName;

			if(bNew) dsh.Tables["File"].Rows.Add(row);
		}

		public void DeleteFile(CSWContext ctx)
		{
			System.IO.File.Delete(ctx.Config.GetLocalPath(idStoreConcept,FileType,sFileName));
		}

		public string URL(CSWContext ctx) { return ctx.Config.GetWebPath(idStoreConcept,FileType,sFileName); }
	}

	public class MMDFileAssociation
	{
		public MMDFileAssociation() {}
		
		[XmlAttribute(AttributeName="dbidLinker")]
		public int idLinker;

		[XmlAttribute(AttributeName="dbidFile")]
		public int idFile;

		public MMDPostedFileType FileType;

		public NullableInt SubType;

		public MMDFileAssociation(DataRow row, DataSetHolder dsh)
		{
			idLinker = Convert.ToInt32(row["idLinker"]);
			idFile = Convert.ToInt32(row["idFile"]);
			FileType = (MMDPostedFileType)Convert.ToInt32(row["iType"]);
			SubType = NullableInt.CreateInstance(row["iSubType"]);
		}

		public void Fill(DataRow row, bool bNew)
		{
			row["idLinker"] = idLinker;
			row["idFile"] = idFile;
			row["iType"] = (int)FileType;
			row["iSubType"] = NullableInt.GetValue(SubType);
		}
	}
	
	public class PostedFile
	{
		
		private static bool ScanArray(string[] arString, string sMatch)
		{
			foreach(string s in arString)
				if(sMatch == s)
					return true;

			return false;
		}

		public static void CheckFileExists(CSWContext ctx, int idStoreConcept, MMDPostedFileType fileType, string sFileName)
		{
			if(System.IO.File.Exists(ctx.Config.GetLocalPath(idStoreConcept,fileType,sFileName)))
			{
				throw new CSWCommandException((int)ErrMsg.FileAlreadyExists,sFileName);
			}
			
		}

		public static FileMediaType GetMediaType(string sMIMEType)
		{

			try
			{
				ValidateImagePosting(sMIMEType);
				return FileMediaType.Image;
			}
			catch {}

			try
			{
				ValidateVideoPosting(sMIMEType);
				return FileMediaType.Video;
			}
			catch {}

			return FileMediaType.ScreenML;
		}

		public static void ValidateBannerPosting(string sMIMEType)
		{
			string[] c_szBannerMIMETypes = { "image/jpeg", "image/pjpeg", "image/gif", "image/bmp", "image/png", "image/x-png" };

			if(!ScanArray(c_szBannerMIMETypes, sMIMEType))
				throw new CSWCommandException((int)ErrMsg.BannerInvalidMIMEType,sMIMEType);
		}

		public static void ValidateImagePosting(string sMIMEType)
		{
			string[] c_szBannerMIMETypes = { "image/jpeg", "image/pjpeg", "image/gif", "image/bmp", "image/png", "image/x-png" };

			if(!ScanArray(c_szBannerMIMETypes, sMIMEType))
				throw new CSWCommandException((int)ErrMsg.ImageInvalidMIMEType,sMIMEType);
		}

		public static void ValidateScreenMLPosting(string sMIMEType)
		{
			string[] c_szFrameMIMETypes = { "text/xml" };

			if(!ScanArray(c_szFrameMIMETypes, sMIMEType))
				throw new CSWCommandException((int)ErrMsg.ScreenMLInvalidMIMEType,sMIMEType);
		}

		public static void ValidateVideoPosting(string sMIMEType)
		{
			string[] c_szFrameMIMETypes = { "video/x-ms-wmv", "video/x-ms-asf" };

			if(!ScanArray(c_szFrameMIMETypes, sMIMEType))
				throw new CSWCommandException((int)ErrMsg.VideoInvalidMIMEType,sMIMEType);
		}

		public static void ValidateBannerPosting(HttpPostedFile File)
		{
			ValidateBannerPosting(File.ContentType);
		}

		public static void ValidateFramePosting(string sMIMEType)
		{
			string[] c_szFrameMIMETypes = { "video/x-ms-wmv", "text/xml", "image/pjpeg", "image/jpeg", "image/gif", "image/bmp", "image/png", "image/x-png" };

			if(!ScanArray(c_szFrameMIMETypes, sMIMEType))
				throw new CSWCommandException((int)ErrMsg.FrameInvalidMIMEType,sMIMEType);

		}

		public static void ValidateFramePosting(HttpPostedFile File)
		{
			ValidateFramePosting(File.ContentType);
		}


		public static string GetAssetInfo(HttpPostedFile File)
		{
			return "Type=" + File.ContentType + ";Size=" + File.ContentLength + ";Path=" + File.FileName;
		}

		public static string GenerateUniqueFileName(HttpPostedFile File)
		{
			return Utilities.File.GetFileName(File.FileName)  + Utilities.File.GetFileExtension(File.FileName);
		}

		public static string GetMIMEType(string sFileName)
		{
			string sFileExtension = Utilities.File.GetFileExtension(sFileName);
			RegistryKey key = Registry.ClassesRoot.OpenSubKey(sFileExtension);
			if(key == null)
				return null;

			return Convert.ToString(key.GetValue("Content Type",null));
		}
	}
	

}
