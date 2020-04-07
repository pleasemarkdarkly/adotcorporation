using System;
using System.Xml;
using System.IO;
using System.Collections;
using System.Data.SqlClient;
using MMD.Common.CommonLib.Data;


namespace MMD.Common.CommonLib
{
	public class Package
	{
		static private readonly string _PackageExtension = "zip";
		static private readonly int _MaxScriptLength = 1024 * 1024 * 5;
		static private readonly string _ManifestFilename = "Manifest.xml";
		static private readonly string _PackageScriptDir = "Scripts";
		static private readonly string _PackageTableDir = "Tables";
		static private readonly string _PackageDirTreeDir = "DirTrees";
		static private readonly string _ScriptExtension = "sql";
		static private readonly string _TableExtension = "tbl";
		static private readonly int _MaxRecursionDepth = 10;

		private string _PackageName;
		private Connection _Conn;
		private Config _Config;
		private DirSet _ActiveDirSet;
		private Level _ThisLevel;
		private LevelDirection _LevelDir;
		private string _TransportSubdir;
		private PackageHeader _PackageHeader;
		private DateTime _PackageStarted;

		public enum PackageDataType
		{
			Table = 1,
			DirTree = 2
		}
		public enum DeltaStyle
		{
			Delta = 1,
			Replace = 2,
		}
		public enum Level
		{
			Store = 1,
			Central = 2,
			Master = 3
		}
		public enum LevelDirection
		{
			Down = 1,
			Up = 2
		}
		public enum Stage
		{
			Inbound = 1,
			Outbound = 2
		}
		private struct DirSet
		{
			public string sSerialize;
			public string sPackage;
			public string sTransport;
		}
		private struct PackageContent
		{
			public PackageDataType DataType;
			public string sTable;
			public string sSelectClause;
			public int idDirTreeClass;
			public string sDirTreeClass;
			public string sDirTreeURL;
			public DeltaStyle DeltaStyle;
			public bool bDelSent;
			public DateTime dtLastRun;
			public int iCompletionStatus;
		}
		private struct PackageHeader
		{
			public Level SourceLevel;
			public Level DestLevel;
			public string Description;
			public string TransportSubdir;
			public DateTime LastRun;
		}

		private enum DirSetClass
		{
			InDown = 1,
			InUp = 2,
			OutDown = 3,
			OutUp = 4
		}
		
		public Package()
		{
			_Conn = General.LocalConnection();
			_Config = Config.GetInstance();
			_ThisLevel = _Config.TransferLevel;
		}

		public void Transfer(int idPackage)
		{
			Level lvlSource, lvlDest;
			LookupEndpoints(idPackage,out lvlSource, out lvlDest);
			Stage stage = ClassifyStage(lvlSource, lvlDest);
			_LevelDir = ClassifyLevelDirection(lvlSource, lvlDest);
			_PackageHeader = LoadPackageHeader(idPackage);
			
			// get the transport subdir
			string sql = String.Format("select sTransportSubdir from tblMMD_Package where idPackage = {0}",idPackage);
			SqlDataReader dr = _Conn.execSQL_DR(sql);
			if (!dr.Read())
			{
				dr.Close();
				throw new Exception("can't find package " + idPackage);
			}
			NamedFieldReader nfr = new NamedFieldReader(dr);
			_TransportSubdir = nfr.GetString("sTransportSubdir");
			nfr.Close();

			LoadDirSet(stage, _LevelDir);
			
			if (stage == Stage.Inbound)
				TransferInbound(idPackage);
			else
				TransferOutbound(idPackage);
		}

		private string PackageNameStub(int idPackage)
		{
			return String.Format("Pkg{0}",idPackage.ToString("000"));
		}

		private string GenPackageName(int idPackage)
		{
			string name = String.Format("{0}_{1}",PackageNameStub(idPackage), General.DBNow().ToString("G"));
			name = name.Replace(" ","_");
			name = name.Replace(":","_");
			name = name.Replace("/","_");
			return name;
		}

		private void LoadDirSet(Stage stage, LevelDirection levelDir)
		{
			DirSetClass dsclass = ClassifyDirSetClass(stage,levelDir);
			string sql = "SELECT sSerialize, sPackage, sTransport FROM tblMMD_PackageDirSet";
			sql += String.Format(" WHERE idPackageLevel = {0} AND idDirSetClass = {1}",(int)_ThisLevel,(int)dsclass);
			SqlDataReader dr = _Conn.execSQL_DR(sql);
			if (!dr.Read())
				throw new Exception(String.Format("No dir set '{0}' defined for package level '{0}'",dsclass.ToString(),_ThisLevel.ToString()));
			_ActiveDirSet.sSerialize = dr.GetString(0);
			_ActiveDirSet.sPackage = dr.GetString(1);
			_ActiveDirSet.sTransport = dr.GetString(2);
			dr.Close();
			// make sure out paths exist
			FilePath.CreatePath(_ActiveDirSet.sSerialize);
			FilePath.CreatePath(_ActiveDirSet.sPackage);
			FilePath.CreatePath(_ActiveDirSet.sTransport);
		}

		private DirSetClass ClassifyDirSetClass(Stage stage, LevelDirection lvlDir)
		{
			if (stage == Stage.Inbound)
			{
				if (lvlDir == LevelDirection.Down)
					return DirSetClass.InDown;
				else
					return DirSetClass.InUp;
			}
			if (lvlDir == LevelDirection.Down)
				return DirSetClass.OutDown;
			else
				return DirSetClass.OutUp;
		}

		private void LookupEndpoints(int idPackage, out Level src, out Level dest)
		{
			string sql = "select idSourceLevel, idDestLevel from tblMMD_Package where idPackage = " + idPackage.ToString();
			NamedFieldReader nfr = new NamedFieldReader(_Conn.execSQL_DR(sql));
			nfr.Read();
			int sourceLevel = nfr.GetInt("idSourceLevel");
			src = (Level)Enum.Parse(typeof(Level),sourceLevel.ToString());
			int destLevel = nfr.GetInt("idDestLevel");
			dest = (Level)Enum.Parse(typeof(Level),destLevel.ToString());
			nfr.Close();
		}

		private LevelDirection ClassifyLevelDirection(Level source, Level dest)
		{
			// there are only four flows, two up, two down.  detect both ups.
			if (source == Level.Store)
				return LevelDirection.Up;
			if (dest == Level.Master)
				return LevelDirection.Up;
			// must be a down.
			return LevelDirection.Down;
		}

		private Stage ClassifyStage(Level source, Level dest)
		{
			if (_ThisLevel == source)
				return Stage.Outbound;
			else if (_ThisLevel == dest)
				return Stage.Inbound;
			else
				throw new Exception(String.Format("Package shouldn't run at level {1}",_ThisLevel.ToString()));
		}

		private void TransferInbound(int idPackage)
		{
			RunChannelInbound(idPackage);
			PackageInbound(idPackage);
			SerializeInbound(idPackage);
		}

		private void TransferOutbound(int idPackage)
		{
			_PackageName = GenPackageName(idPackage);
			SerializeOutbound(idPackage);
			PackageOutbound(idPackage);
			RunChannelOutbound(idPackage);
		}

		public void RunChannelInbound(int idPackage)
		{
			string pkgstub = PackageNameStub(idPackage);
			if (_LevelDir == LevelDirection.Down)
			{
				// down flow => active reception
				FtpSocketClient ftp = new FtpSocketClient();
				// TODO : verify that we'll really use this version of ftp server, or impl the correct one.
				FtpServerInterpreter_MSFTPV5 parser = new FtpServerInterpreter_MSFTPV5();
				
				string ftpdir = _TransportSubdir.Replace("\\","/");
		
				ftp.Host = _Config.FtpClient.Host;
				ftp.Port = _Config.FtpClient.Port;
				ftp.User = _Config.FtpClient.User;
				ftp.Password = _Config.FtpClient.Password;
				ftp.Connect();
				if (!parser.CanHandleConnectResponse(ftp.LastResponse))
					throw new Exception(String.Format("ftp server parser '{0}' can't handle connect response from {1}",parser.ServerDescription(),ftp.Host));
				ftp.Login();
				ftp.MakeDirRecursive(ftpdir);
				ftp.CD(ftpdir);
				
				string listing = ftp.List();
				string [] files = parser.FileNamesFromListing(listing);
				foreach(string file in files)
				{
					if (file.StartsWith(pkgstub))
					{
						// I could almost just drop this straight into the package dir, but if I 
						// move it there later, I'll know it's b/c I've already finished all transport.
						string localpath = _ActiveDirSet.sTransport + _TransportSubdir + file;
						FilePath.CreatePath(localpath);
						ftp.GetFile(file,localpath);
					}
				}
				ftp.Disconnect();
			}
			// else, passive reception => just move whatever is in the transport dir to the next stage.
			// this will happen for either direction.
			string [] packages = Directory.GetFiles(_ActiveDirSet.sTransport + _TransportSubdir, pkgstub + "*." + _PackageExtension);
			foreach (string file in packages)
				File.Move(file,_ActiveDirSet.sPackage + FilePath.NameFromPath(file));
		}

		public void RunChannelOutbound(int idPackage)
		{
			if (_LevelDir == LevelDirection.Up)
			{
				string file = _ActiveDirSet.sTransport + _PackageHeader.TransportSubdir + _PackageName + "." + _PackageExtension;

				// actively push the files up via ftp.
				FtpSocketClient ftp = new FtpSocketClient();
				FtpEndpoint info = _Config.FtpClient;
				ftp.Host = info.Host;
				ftp.Port = info.Port;
				ftp.User = info.User;
				ftp.Password = info.Password;

				ftp.Connect();
				ftp.Login();
				string ftpdir = _PackageHeader.TransportSubdir.Replace("\\","/");
				ftp.MakeDirRecursive(ftpdir);
				ftp.CD(ftpdir);
				ftp.PutFile(file,FilePath.NameFromPath(file));
				// TODO : archival support.
				File.Delete(file);
			}
			// else, just leave the packages in the outgoing dir to be picked up from afar.
		}
		public void PackageInbound(int idPackage)
		{
			string[] pkgfiles = Directory.GetFiles(_ActiveDirSet.sPackage,"*." + _PackageExtension);
			
			// todo : process the packages in order somehow, to guarantee that e.g. scripts are handled
			// correctly.

			RecursiveZip zip = new RecursiveZip();
			foreach (string pkgfile in pkgfiles)
			{
				zip.Filename = pkgfile;
				string packageroot = zip.UnzipToDir();
				File.Delete(pkgfile);
				
				// move the unzipped dir to the next stage.
				string serdir = _ActiveDirSet.sSerialize + FilePath.NameFromPath(packageroot);
				if (Directory.Exists(serdir))
					Directory.Delete(serdir,true);
				Directory.Move(packageroot,serdir);
			}
		}

		public void PackageOutbound(int idPackage)
		{
			string packageFilename = String.Format("{0}.{1}",_PackageName, _PackageExtension);
			string packagePath = _ActiveDirSet.sPackage + packageFilename;
			string treeRoot = _ActiveDirSet.sPackage + _PackageName + "\\";

			RecursiveZip rz = new RecursiveZip();
			rz.TreeRoot = treeRoot;
			rz.Filename = packagePath;
			rz.ZipDir();
			
			FilePath.EnsureDir(ref _PackageHeader.TransportSubdir);
			string dest = _ActiveDirSet.sTransport + _PackageHeader.TransportSubdir + packageFilename;
			FilePath.CreatePath(dest);
			File.Move(packagePath,dest);
			Directory.Delete(treeRoot ,true);
		}

		private PackageContent [] LoadPackageContents(int idPackage)
		{
			// grrr.
			string sql = "select count(*) as contents from tblmmd_packageContent where idPackage = " + idPackage;
			NamedFieldReader nfr = new NamedFieldReader(_Conn.execSQL_DR(sql));
			nfr.Read();
			int nContents = nfr.GetInt("contents");
			nfr.Close();

			if (nContents == 0)
				throw new Exception("no contents defined in package " + idPackage);

			sql = String.Format("exec spr_PKGGetPackageContents {0}",idPackage);
			nfr = new NamedFieldReader(_Conn.execSQL_DR(sql));
				
			PackageContent [] contents = new PackageContent[nContents];

			int i = 0;
			while (nfr.Read() && i < nContents)
			{
				contents[i].bDelSent = nfr.GetBoolean("bDelSent");
				contents[i].DataType = (PackageDataType) Enum.Parse(typeof(PackageDataType), nfr.GetInt("idDataType").ToString());
				contents[i].DeltaStyle = (DeltaStyle) Enum.Parse(typeof(DeltaStyle), nfr.GetInt("idDeltaStyle").ToString());
				contents[i].sDirTreeURL = nfr.SafeGetString("sDirTreeURL","");
				contents[i].sTable = nfr.SafeGetString("sTable","");
				contents[i].sSelectClause = nfr.SafeGetString("sSelectClause","");
				contents[i].idDirTreeClass = nfr.SafeGetInt("idDirTreeClass",0);
				contents[i].sDirTreeClass = nfr.SafeGetString("sDirTreeClass","");
				contents[i].dtLastRun = nfr.SafeGetDateTime("dtLastRun",DateTime.Parse("01/01/1990"));
				contents[i].iCompletionStatus = 0;
				++i;
			}
			nfr.Close();
			return contents;
		}

		private PackageHeader LoadPackageHeader(int idPackage)
		{
			PackageHeader ph;
			string sql = String.Format("exec spr_PKGGetPackageHeader {0}, {1}",idPackage,(int)_Config.TransferLevel);
			NamedFieldReader nfr = new NamedFieldReader(_Conn.execSQL_DR(sql));
			nfr.Read();
			ph.Description = nfr.SafeGetString("sDesc","");
			ph.DestLevel = (Level) Enum.Parse(typeof(Level),nfr.GetInt("idDestLevel").ToString());
			ph.SourceLevel = (Level) Enum.Parse(typeof(Level),nfr.GetInt("idSourceLevel").ToString());
			ph.LastRun = nfr.SafeGetDateTime("dtLastRun",DateTime.Parse("01/01/1990"));
			ph.TransportSubdir = nfr.SafeGetString("sTransportSubdir","");
			nfr.Close();
			return ph;
		}

		private void SerializeInbound(int idPackage)
		{
			PackageContent [] contents = LoadPackageContents(idPackage);
		
			string pkgstub = PackageNameStub(idPackage);

			string [] packages = Directory.GetDirectories(_ActiveDirSet.sSerialize,pkgstub + "*");

			foreach (string dir in packages)
			{
				string packageroot = dir;
				FilePath.EnsureDir(ref packageroot);
				_PackageName = FilePath.NameFromPath(packageroot);
				if (File.Exists(packageroot + _ManifestFilename))
				{
					XmlDocument xman = new XmlDocument();
					xman.Load(packageroot + _ManifestFilename);
					// TODO : add'l processing of manifest information.

					xman = null;
				}
					
				string scriptdir = packageroot + _PackageScriptDir + "\\";
				if (Directory.Exists(scriptdir))
				{
					string [] scriptfiles = Directory.GetFiles(scriptdir, "*." + _ScriptExtension);
					// TODO : sort the script files to guarantee fifo behavior.

					// not sorting these particularly, so if they're interdependant and order specific, 
					// they need to be in the same file.
					// TODO : some mechnism for getting script files into the package.  likely a new package content type.
					foreach (string scriptfile in scriptfiles)
					{
						FileStream fs = File.OpenRead(scriptfile);
						if (fs.Length > _MaxScriptLength)
							throw new Exception(String.Format("unacceptably large sql script received : {0} bytes",fs.Length));
						Byte[] scriptbytes = new Byte[fs.Length+1];
						fs.Read(scriptbytes, 0, (int)fs.Length);
						fs.Close();
						string script = System.Text.Encoding.ASCII.GetString(scriptbytes,0,(int)fs.Length);
						scriptbytes = null;
						int rows = _Conn.execSQL(script);
						// TODO : log rows affected?
					}
				}
				foreach (PackageContent content in contents)
				{
					if (content.DataType == PackageDataType.DirTree)
						SerializeDirTreeInbound(content);
					else if (content.DataType == PackageDataType.Table)
						SerializeTableInbound(content);
					else
						throw new Exception("unhandled package content type " + content.DataType.ToString());
				}
				
				// TODO : something else here, configurable, whatever.
				FilePath.CreatePath(@"c:\transfer\incoming\archive\");
				GetFileOutOfWay(@"c:\transfer\incoming\archive\" + FilePath.NameFromPath(packageroot) + "\\");
				Directory.Move(packageroot,@"c:\transfer\incoming\archive\" + FilePath.NameFromPath(packageroot));
				//Directory.Delete(packageroot, true);
			}
		}

		private void GetFileOutOfWay(string filename)
		{
			if (FilePath.IsDir(filename))
			{
				if (Directory.Exists(filename))
				{
					int iBackup = 1;
					string backup;
					do 
					{
						backup = filename.Substring(0,filename.Length-1) + "_bak" + iBackup.ToString("00") + "\\";
						++iBackup;
					} while (Directory.Exists(backup));
					Directory.Move(filename,backup);
				}
			}
			else
			{
				if (File.Exists(filename))
				{
					int iBackup = 1;
					string backup;
					do 
					{
						backup = FilePath.StripExtension(filename) + "_bak" + iBackup.ToString("00") + FilePath.NameExtension(filename);
						++iBackup;
					} while (File.Exists(backup));
					File.Move(filename,backup);
				}
			}
		}

		private void SerializeOutbound(int idPackage)
		{
			PackageContent [] contents = LoadPackageContents(idPackage);
	
			_PackageStarted = General.DBNow();
			foreach (PackageContent content in contents)
			{
				if (content.DataType == PackageDataType.DirTree)
					SerializeDirTreeOutbound(content);
				else if (content.DataType == PackageDataType.Table)
					SerializeTableOutbound(content);
				else
					throw new Exception("unhandled package content type " + content.DataType.ToString());
				RecordPackageContentHistory(idPackage, content);
			}
			Directory.Move(_ActiveDirSet.sSerialize + _PackageName + "\\", _ActiveDirSet.sPackage + _PackageName + "\\");
		}

		private void RecordPackageContentHistory(int idPackage, PackageContent content)
		{
			string sql = "INSERT INTO tblMMD_PackageContentHistory (idPackage, idPackageLevel, idStore, idDataType, sTable, idDirTreeClass, iCompletionStatus, dtLogged)";
			sql += String.Format(" VALUES ({0}, {1}, {2}, {3}, '{4}', {5}, {6}, '{7}')",
				idPackage, _Config.TransferLevel.ToString("d"), _Config.StoreNumber, content.DataType.ToString("d"), content.sTable, content.idDirTreeClass, content.iCompletionStatus, _PackageStarted);
			if (_Conn.execSQL(sql) != 1)
				throw new Exception("couldn't insert into package content history");
		}

		private void SerializeTableInbound(PackageContent content)
		{
			string sDestTable;
		
			if (content.DeltaStyle == DeltaStyle.Replace)
			{
				sDestTable = content.sTable;
				string sql = "DELETE FROM " + sDestTable;
				_Conn.execSQL(sql);
			}
			else
			{
				sDestTable = GetDeltaTableName(content.sTable);
				CreateTempDeltaTable(sDestTable,content.sTable,_Config.DBDatabase);
			}

			string tabledir = _ActiveDirSet.sSerialize + _PackageName + "\\" + _PackageTableDir + "\\";
			string file = content.sTable + "." + _TableExtension;
			if (!File.Exists(tabledir + file))
				throw new Exception("Package table missing: " + file);

			FlatfileReader ffr = new FlatfileReader();
			ffr.Table = sDestTable;
			ffr.Filename = tabledir + file;
			ffr.Database = _Config.DBDatabase;
			ffr.Password = _Config.DBPassword;
			ffr.Server = _Config.DBServer;
			ffr.User = _Config.DBUser;

			ffr.ToTable();

			if (content.DeltaStyle == DeltaStyle.Delta)
			{
				DeltaTable dt = new DeltaTable();
				dt.MergeDelta(content.sTable, sDestTable, _Config.DBDatabase);
				DropTempDeltaTable(sDestTable);
			}
		}

		private string GetDeltaTableName(string baseTable)
		{
			return "tblDLT_" + baseTable;
		}
		private void CreateTempDeltaTable(string sDeltaTable, string sTable, string sCatalog)
		{
			DropTempDeltaTable(sDeltaTable);

			string sql = String.Format("SELECT column_name, data_type, character_maximum_length FROM information_schema.columns WHERE table_name = '{0}' and table_catalog = '{1}' ORDER BY ordinal_position",sTable,sCatalog);
			NamedFieldReader nfr = new NamedFieldReader (_Conn.execSQL_DR(sql));
			ArrayList columns = new ArrayList();
			ArrayList types = new ArrayList();
			while (nfr.Read())
			{
				columns.Add(nfr.GetString("column_name"));
				string basetype = nfr.GetString("data_type");
				if (basetype == "varchar")
				{
					int maxlen = nfr.GetInt("character_maximum_length");
					types.Add(String.Format("[varchar] ({0})",maxlen));
				}
				else
					types.Add("[" + basetype + "]");
			}
			nfr.Close();

			sql = String.Format("CREATE TABLE dbo.[{0}] (\n",sDeltaTable);
			bool bFirst = true;
			for (int i = 0; i < columns.Count; ++i)
			{
				if (!bFirst)
					sql += ", ";
				bFirst = false;
				sql += String.Format("[{0}] {1} NULL\n",columns[i],types[i]);
			}
			sql += ")";

			_Conn.execSQL(sql);
		}
		private void DropTempDeltaTable(string delta)
		{
			string sql = "drop table " + delta;
			try { _Conn.execSQL(sql); }
			catch{}
		}

		private void SerializeTableOutbound(PackageContent content)
		{
			string sSourceTable;

			string tabledir = _ActiveDirSet.sSerialize + _PackageName + "\\" + _PackageTableDir + "\\";
			FilePath.CreatePath(tabledir);
		
			if (content.DeltaStyle == DeltaStyle.Replace)
				sSourceTable = content.sTable;
			else
			{
				sSourceTable = GetDeltaTableName(content.sTable);
				CreateTempDeltaTable(sSourceTable, content.sTable, _Config.DBDatabase);
				DeltaTable dt = new DeltaTable();
				//_Conn.execSQL("delete from " + sSourceTable);
				dt.GenDelta(content.sTable,sSourceTable,content.sSelectClause,content.dtLastRun,_PackageStarted,_Config.DBDatabase, content.bDelSent);
			}

			string file = content.sTable + "." + _TableExtension;
			if (File.Exists(tabledir + file))
				File.Delete(tabledir + file);

			FlatfileWriter ffw = new FlatfileWriter();
			ffw.Table = sSourceTable;
			ffw.Filename = tabledir + file;
			ffw.Database = _Config.DBDatabase;
			ffw.Password = _Config.DBPassword;
			ffw.Server = _Config.DBServer;
			ffw.User = _Config.DBUser;

			ffw.ToFile();

			if (content.DeltaStyle == DeltaStyle.Delta)
			{
				DropTempDeltaTable(sSourceTable);
			}
		}

		private string GetDirTreeName(PackageContent content)
		{
			return String.Format("Tree{0}_{1}",content.idDirTreeClass,content.sDirTreeClass);
		}

		private void SerializeDirTreeInbound(PackageContent content)
		{
			string treename = GetDirTreeName(content);
			string sourceroot = _ActiveDirSet.sSerialize + _PackageName + "\\" + _PackageDirTreeDir + "\\" + treename + "\\";
			string destroot = content.sDirTreeURL;

			// there may not be any data for this, so just exit if that's the case.
			if (!Directory.Exists(sourceroot))
				return;

			if (content.DeltaStyle == DeltaStyle.Replace)
			{
				// this could be catastrophic if configured wrongly.  TODO : ensure this is correct intent, install safeguards.
				Directory.Delete(destroot,true);
				Directory.CreateDirectory(destroot);
			}

			SerializeDirTreeInboundAux(sourceroot, content.sDirTreeURL, 0);
		}

		private void SerializeDirTreeInboundAux(string sourcePath, string destPath, int iDepth)
		{
			if (iDepth > _MaxRecursionDepth)
				throw new Exception(String.Format("Dir Tree for transfer '{0}' at '{1}' more than {2} levels deep",_PackageName,sourcePath,_MaxRecursionDepth));

			// lay files over
			string [] files = Directory.GetFiles(sourcePath);
			foreach (string file in files)
			{
				File.Copy(file,destPath + FilePath.NameFromPath(file));
			}
			files = null;
			// and recurse
			string [] dirs = Directory.GetDirectories(sourcePath);
			foreach (string dir in dirs)
			{
				string dirname = FilePath.NameFromPath(dir);
				string destpath = destPath + dirname + "\\";
				Directory.CreateDirectory(destpath);
				SerializeDirTreeInboundAux(dir,destpath,iDepth+1);
			}
		}

		private void SerializeDirTreeOutbound(PackageContent content)
		{
			// clear out the destination staging area.
			string destroot = _ActiveDirSet.sSerialize + _PackageName + "\\" + _PackageDirTreeDir + "\\" + GetDirTreeName(content) + "\\";
			string parent = FilePath.ParentPathFromPath(destroot);
			FilePath.CreatePath(parent);
			if (Directory.Exists(destroot))
				Directory.Delete(destroot,true);
			FilePath.CreatePath(destroot);
			FilePath.CreatePath(content.sDirTreeURL);
			SerializeDirTreeOutboundAux(content.sDirTreeURL, destroot, 0, content.dtLastRun);
		}

		private void SerializeDirTreeOutboundAux(string inpath, string outpath, int iDepth, DateTime dtLastRun)
		{
			if (iDepth > _MaxRecursionDepth)
				throw new Exception(String.Format("Dir Tree for transfer '{0}' at '{1}' more than {2} levels deep",_PackageName,inpath,_MaxRecursionDepth));
			
			string [] files = Directory.GetFiles(inpath);
			foreach (string file in files)
			{
				DateTime lastwrite = File.GetLastWriteTime(file);
				if (lastwrite > dtLastRun)
					File.Copy(file,outpath + FilePath.NameFromPath(file));
			}
			files = null;
			string [] dirs = Directory.GetDirectories(inpath);
			foreach (string dir in dirs)
			{
				string dirname = FilePath.NameFromPath(dir);
				string destpath = outpath + dirname + "\\";
				Directory.CreateDirectory(destpath);
				SerializeDirTreeOutboundAux(dir,destpath,iDepth+1,dtLastRun);
			}
		}
	}
}
