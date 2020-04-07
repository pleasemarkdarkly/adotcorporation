using System;
using System.Data;
using System.Data.SqlClient;
using System.Xml;
using System.Xml.Serialization;
using MMD.Central.CentralLib;
using MMD.Central.CentralLib.Data;
using MMD.Central.CentralLib.Utilities;
using MMD.Central.CentralLib.XmlObjects;

namespace  MMD.Central.CentralLib.ServiceImpl
{
/*
	public class Advertising : CSWObjectBase 
	{
		public Advertising(CSWContext ctx) : base(ctx)
		{
		}

		private Location _location;

		public Location Location { get { return _location; } set { _location = value; } }
														
		DataSetHolder _dsFiles = null;

		private DataSetHolder FileDataSet()
		{
			if(_dsFiles == null)
			{
				_dsFiles = new DataSetHolder();
				Data.Connection conn = Context.Config.MainDatabase;
				_dsFiles.FillTable(conn,"File","select * from tblMMD_File");
				_dsFiles.FillTable(conn,"FileAssociation","select * from tblMMD_FileAssociation");
			}
			return _dsFiles;
		}

		public MMDFile RegisterFile(string sFileName, MMDPostedFileType FileType, string sMIMEType)
		{
			switch(FileType)
			{
				case MMDPostedFileType.IdleLoopElement:
					PostedFile.ValidateFramePosting(sMIMEType);
					break;
				case MMDPostedFileType.RecListAttribute:
					PostedFile.ValidateImagePosting(sMIMEType);
					break;
			}

			DataSetHolder ds = FileDataSet();
			
			ds.BuildCommands("File");

			DataRow[] rows = ds.Tables["File"].Select("sFileName = " + SQL.EncodeFull(sFileName));
			if(rows.Length > 0)
				throw new ApplicationException("File already exists.");
			
			MMDFile file = new MMDFile();
			file.dtChanged = NullableDateTime.Now;
			file.FileType = FileType;
			file.sFileName = sFileName;
			file.sMIMEType = sMIMEType;
			file.sAssetInfo = Context.UserName;

			DataRow row = ds.Tables["File"].NewRow();
			file.Fill(row,true);
			ds.Tables["File"].Rows.Add(row);

			ds.Update("File","idFile");
	
			file = new MMDFile(row,ds,this.Context );
			return file;
		}

		public MMDFile[] GetAllFiles()
		{
			DataSetHolder ds = FileDataSet();

			DataRow[] rows = ds.Tables["File"].Select();
			MMDFile[] arFiles = new MMDFile[rows.Length];
			for(int i=0;i<rows.Length;i++)
				arFiles[i] = new MMDFile(rows[i],ds,this.Context);

			return arFiles;
		}

		public MMDFile GetFile(int idFile)
		{
			DataSetHolder ds = FileDataSet();
			DataRow row = ds.Tables["File"].Rows.Find(idFile);
			return new MMDFile(row,ds,this.Context );
		}

		public void RemoveFiles([XmlElement(ElementName="dbidFile")]int[] idFiles)
		{
			DataSetHolder ds = FileDataSet();
			ds.BuildCommands("File");

			for(int i=0;i<idFiles.Length;i++)
			{
				DataRow row = ds.Tables["File"].Rows.Find(idFiles[i]);
				if(row != null)
					throw new ApplicationException("Missing idFile (" + idFiles[i] + ")");

				DataRow[] rows = ds.Tables["FileAssociation"].Select("idFile = " + idFiles[i]);
				if(rows.Length > 0)
					throw new ApplicationException("File still has associations");
				
				row.Delete();
			}
			
			ds.Update("File","idFile");
		}

		private DataSetHolder IdleLoopDataSet(Location location)
		{
			DataSetHolder ds = new DataSetHolder();
			Data.Connection conn = Context.Config.MainDatabase;
			ds.FillTable(conn,"File","select * from tblMMD_File where iType = " + (int)MMDPostedFileType.IdleLoopElement);
			ds.FillTable(conn,"FileAssociation","select * from tblMMD_FileAssociation where iType = " + (int)MMDPostedFileType.IdleLoopElement);

			ds.Relations.Add("FA_F",ds.Tables["FileAssociation"].Columns["idFile"],ds.Tables["File"].Columns["idFile"]);

			string sTable;
			sTable = (location == Location.Staging) ? "tblMMD_IdleLoopStaging" : "tblMMD_IdleLoop";
			ds.FillTable(conn,"IdleLoop","select * from " + sTable);

			sTable = (location == Location.Staging) ? "tblMMD_IdleLoopElementStaging" : "tblMMD_IdleLoopElement";
			ds.FillTable(conn,"IdleLoopElement","select * from " + sTable);

			ds.Relations.Add("IL_ILE",ds.Tables["IdleLoop"].Columns["idIdleLoop"],ds.Tables["IdleLoopElement"].Columns["idIdleLoop"]);
			ds.Relations.Add("ILE_FA",ds.Tables["IdleLoopElement"].Columns["idIdleLoopElement"],ds.Tables["FileAssociation"].Columns["idLinker"]);
			
			return ds;
		}

		private void UpdateIdleLoop(DataSetHolder ds)
		{
			ds.BuildCommands("IdleLoop");
			ds.BuildCommands("IdleLoopElement");
			ds.BuildCommands("FileAssociation");
			//ds.BuildCommands("File");

			ds.AddForeignKeyConstraint("C_IL_ILE","IdleLoop","idIdleLoop","IdleLoopElement","idIdleLoop");
			ds.AddForeignKeyConstraint("C_ILE_FA","IdleLoopElement","idIdleLoopElement","FileAssociation","idLinker");
			ds.AddForeignKeyConstraint("C_FA_F","FileAssociation","idFile","File","idFile");

			ds.Update("IdleLoop","idIdleLoop");
			ds.Update("IdleLoopElement","idIdleLoopElement");
			ds.Update("FileAssociation");
			//ds.Update("File","idFile");
		}

		public IdleLoop[] GetAllIdleLoops()
		{
			DataSetHolder ds = IdleLoopDataSet(_location);
			DataRow[] rows = ds.Tables["IdleLoop"].Select();
			IdleLoop[] arIdleLoops = new IdleLoop[rows.Length];
			for(int i=0;i<rows.Length;i++)
				arIdleLoops[i] = new IdleLoop(_location,rows[i],ds,this.Context);
			return arIdleLoops;
		}
		
		[return : XmlElement("IdleLoop")]
		public IdleLoop[] GetIdleLoopById([XmlElement(ElementName="dbidIdleLoop")]int[] idIdleLoop)
		{
			DataSetHolder ds = IdleLoopDataSet(_location);
			IdleLoop[] arIdleLoops = new IdleLoop[idIdleLoop.Length];
			for(int i=0;i<idIdleLoop.Length;i++)
			{
				DataRow row = ds.Tables["IdleLoop"].Rows.Find(idIdleLoop[i]);
				if(row == null)
					throw new ApplicationException("Missing IdleLoop (id = " + idIdleLoop[i] + ")");
				arIdleLoops[i] = new IdleLoop(_location,row,ds,this.Context);
			}
			return arIdleLoops;
		}

		public void AddIdleLoop([XmlElement("IdleLoop")]IdleLoop[] idleLoops)
		{
			DataSetHolder ds = IdleLoopDataSet(_location);
			for(int i=0;i<idleLoops.Length;i++)
			{
				DataRow row = ds.Tables["IdleLoop"].NewRow();
				idleLoops[i].Fill(_location,row,ds,true,Context);
				ds.Tables["IdleLoop"].Rows.Add(row);
			}

			UpdateIdleLoop(ds);
		}

		public void UpdateIdleLoop([XmlElement("IdleLoop")]IdleLoop[] idleLoops)
		{
			DataSetHolder ds = IdleLoopDataSet(_location);
			for(int i=0;i<idleLoops.Length;i++)
			{
				bool bNew = idleLoops[i].idIdleLoop == 0;
				DataRow row = (bNew) ? ds.Tables["IdleLoop"].NewRow() : ds.Tables["IdleLoop"].Rows.Find(idleLoops[i].idIdleLoop);
				idleLoops[i].Fill(_location,row,ds,bNew,Context);
				if(bNew) ds.Tables["IdleLoop"].Rows.Add(row);
			}

			UpdateIdleLoop(ds);

		}


		public void UpdateIdleLoopElementFiles([XmlElement("dbidIdleLoopElement")] int idIdleLoopElement,
			[XmlElement("File",IsNullable=true)]MMDFile[] Files)
		{
			DataSetHolder ds = IdleLoopDataSet(_location);
			DataRow row = ds.Tables["IdleLoopElement"].Rows.Find(idIdleLoopElement);

			IdleLoopElement ile = IdleLoopElement.CreateInstance(row,ds,this.Context);
			ile.Files = Files;
			ile.Fill(row,null,ds,false,Context);

			UpdateIdleLoop(ds);
		}

*/		
		

/*
		// Banner Types
		public BannerType[] GetAllBannerTypes()
		{
			return null;
		}

		public BannerType GetBannerTypeById([XmlElement(ElementName="dbidBannerType")]int idBannerType)
		{
			return null;
		}

		public void AddBannerType([XmlElement("BannerType")]BannerType bannerType)
		{
		}

		public void UpdateBannerType([XmlElement("BannerType")]BannerType bannerType)
		{
		}


		//Banners
		public Banner[] GetBannersByBannerType([XmlElement("dbidBannerType")]int idBannerType)
		{
			return null;
		}

		public Banner GetBannerById([XmlElement("dbidBanner")]int idBanner)
		{
			return null;
		}

		public void AddBanner([XmlElement("Banner")]Banner banner)
		{
		}

		public void UpdateBanner([XmlElement("Banner")]Banner banner)
		{
		}

		public void RemoveBanner([XmlElement("dbidBanner")]int idBanner)
		{
		}
	}
*/		

}
