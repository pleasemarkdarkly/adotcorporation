using System;
using System.Collections;
using System.Data;
using System.Data.SqlClient;
using System.Web;
using System.Xml.Serialization;
using MMD.Central.CentralLib;
using MMD.Central.CentralLib.Data;
using MMD.Central.CentralLib.Utilities;

namespace MMD.Central.CentralLib.DataObjects
{
	public enum DataObjectLocation
	{
		Staging,
		Deployed,
	}
/*
	public class BannerType : CSWObjectBase
	{
		public int _idBannerType;
		public string _sName;
		public int _iWidth;
		public int _iHeight;

		public BannerType(CSWContext ctx, int idBannerType) : base(ctx)
		{
			Load(idBannerType);	
		}

		public void Load(int idBannerType)
		{
			DataSet ds = Context.Config.MainDatabase.execSQL_DS("select * from tblMMD_BannerType where idBannerType = " + idBannerType);
			DataRow row = ds.Tables[0].Rows[0];

			_idBannerType = (int)row["idBannerType"];
			_sName = (string)row["sName"];
			_iWidth = (int)row["iWidth"];
			_iHeight = (int)row["iHeight"];
		}
	}

	public class Banner : CSWObjectBase
	{
		public int _idBanner;
		public int _idBannerType;
		public string _sName;
		public string _sURL;
		public string _sDeviceLink;
		public object _dtCreated;
		public object _dtChanged;
		public string _sCreatedBy;
		public string _sChangedBy;
		public StagingStatus _byStatus;
		public string _sAssetInfo;

		public BannerType _BannerType;

		public Banner(CSWContext ctx, int idBanner) : base(ctx) { Load(idBanner); }

		public void Load(int idBanner)
		{
			DataSet ds = Context.Config.MainDatabase.execSQL_DS("select * from tblMMD_BannerStaging where idBanner = " + idBanner);
			DataRow row = ds.Tables[0].Rows[0];

			_idBanner = (int)row["idBanner"];
			_idBannerType = (int)row["idBannerType"];
			_sName = (string)row["sName"];
			_sURL = Context.Config.BannerWebPath(true) + "/" + (string)row["sURL"];
			_sDeviceLink = Convert.ToString(row["sDeviceLink"]);
			_dtCreated = SQL.SafeGetDate(row["dtCreated"]);
			_dtChanged = SQL.SafeGetDate(row["dtChanged"]);
			_sCreatedBy = Convert.ToString(row["sCreatedBy"]);
			_sChangedBy = Convert.ToString(row["sChangedBy"]);
			_byStatus = (StagingStatus)(int)(byte)row["byStatus"];
			_sAssetInfo = Convert.ToString(row["sAssetInfo"]);

			_BannerType = new BannerType(this.Context, _idBannerType);

		}
		
	}
*/

	public class IdleLoop : CSWObjectBase
	{
		public IdleLoop(CSWContext ctx, int idIdleLoop) : this(ctx, idIdleLoop, true) {}
		public IdleLoop(CSWContext ctx, int idIdleLoop, bool bLoadIdleLoopElements, DataObjectLocation loc) : base(ctx) { Load(idIdleLoop,bLoadIdleLoopElements,loc); }
		public IdleLoop(CSWContext ctx, int idIdleLoop, bool bLoadIdleLoopElements) : this(ctx, idIdleLoop, bLoadIdleLoopElements, DataObjectLocation.Staging) {}
		public int idIdleLoop;
		public string sName;
		public string sInfo;
		public object dtChanged;
		public object dtCreated;
		public string sChangedBy;
		public string sCreatedBy;
		public StagingStatus byStatus;
		public IdleLoopElements Elements = null;

		//public void Load(int idIdleLoop) { Load(idIdleLoop,true); }

		public void Load(int idIdleLoop, bool bLoadIdleLoopElements, DataObjectLocation loc)
		{
			DataSet ds = Context.Config.MainDatabase.execSQL_DS("select * from " + Table(loc) + " where idIdleLoop = " + idIdleLoop);
			DataRow row = ds.Tables[0].Rows[0];
		
			this.idIdleLoop = (int)row["idIdleLoop"];
			sName = (string)row["sName"];
			sInfo = SQL.SafeGetString(row["sInfo"]);
			dtChanged = SQL.SafeGetDate(row["dtChanged"]);
			dtCreated = SQL.SafeGetDate(row["dtCreated"]);
			sChangedBy = SQL.SafeGetString(row["sChangedBy"]);
			sCreatedBy = SQL.SafeGetString(row["sCreatedBy"]);
			if(loc == DataObjectLocation.Staging)
				byStatus = (StagingStatus)(byte)row["byStatus"];
			
			if(bLoadIdleLoopElements) Elements = new IdleLoopElements(this.Context,idIdleLoop,loc);
		}

		private string Table(DataObjectLocation loc)
		{
			if(loc == DataObjectLocation.Staging)
					return "tblMMD_IdleLoopStaging";
			if(loc == DataObjectLocation.Deployed)
					return "tblMMD_IdleLoop";
			return "";
		}

	}

	public class IdleLoopElements : CSWObjectBase
	{
		public IdleLoopElements(CSWContext ctx, int idIdleLoop, DataObjectLocation loc) : base(ctx) { Load(idIdleLoop,loc); }

		ArrayList arElements;

		public void Load(int idIdleLoop, DataObjectLocation loc)
		{

			DataSet ds = Context.Config.MainDatabase.execSQL_DS("select * from " + Table(loc) + " where idIdleLoop = " + idIdleLoop + " order by iSeq");
			DataRowCollection rows = ds.Tables[0].Rows;

			arElements = new ArrayList();
			
			foreach(DataRow row in rows)
			{
				IdleLoopElement ile = new IdleLoopElement(this.Context, row);
				arElements.Add(ile);
			}
		}
		
		private string Table(DataObjectLocation loc)
		{
			if(loc == DataObjectLocation.Staging)
				return "tblMMD_IdleLoopElementStaging";
			if(loc == DataObjectLocation.Deployed)
				return "tblMMD_IdleLoopElement";
			return "";
		}

		public ArrayList Array { get { return arElements; } }
		public int Count { get { return arElements.Count; } }
		public IdleLoopElement this[int i] { get { return (IdleLoopElement)arElements[i]; } }
	}

	public class IdleLoopElement : CSWObjectBase
	{
		public IdleLoopElement(CSWContext ctx, int idIdleLoopElement, DataObjectLocation loc) : base(ctx) { Load(idIdleLoopElement,loc); }
		public IdleLoopElement(CSWContext ctx, int idIdleLoopElement) : this(ctx,idIdleLoopElement,DataObjectLocation.Staging) {}
		public IdleLoopElement(CSWContext ctx, DataRow row) : base(ctx) { Load(row); }
	
		public void Load(int idIdleLoopElement, DataObjectLocation loc)
		{
			DataSet ds = Context.Config.MainDatabase.execSQL_DS("select * from " + Table(loc) + " where idIdleLoopElement = " + idIdleLoopElement);
			Load(ds.Tables[0].Rows[0]);
		}

		private string Table(DataObjectLocation loc)
		{
			if(DataObjectLocation.Staging == loc)
				return "tblMMD_IdleLoopElementStaging";
			if(DataObjectLocation.Deployed == loc)
				return "tblMMD_IdleLoopElement";
			return "";
		}

		public int idIdleLoopElement;
		public int idIdleLoop;
		public int iSeq;
		public string sName;
		public string sMIMEType;
		public string sURL;
		public int cSecsPlayingTime;
		public object dtChanged;
		public string sChangedBy;
		public string sAssetInfo;

		public void Load(DataRow row)
		{
			idIdleLoopElement = (int)row["idIdleLoopElement"];
			idIdleLoop = (int)row["idIdleLoop"];
			iSeq = (int)row["iSeq"];
			sName = (string)row["sName"];
			sMIMEType = SQL.SafeGetString(row["sMIMEType"]);
			sURL = (string)row["sURL"];
			cSecsPlayingTime = (int)row["cSecsPlayingTime"];
			dtChanged = SQL.SafeGetDate(row["dtChanged"]);
			sChangedBy = SQL.SafeGetString(row["sChangedBy"]);
			sAssetInfo = SQL.SafeGetString(row["sAssetInfo"]);
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
			return Utilities.File.GetFileName(File.FileName) /* + "_" + Guid.NewGuid().ToString("N").Substring(0,8) */ + Utilities.File.GetFileExtension(File.FileName);
		}
	}

}

/*
namespace MMD.Central.CentralLib.XmlObjects
{
	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class ScheduleItem
	{
		public ScheduleItem() {}

		public DateTime dtBegin;
		public DateTime dtEnd;
		public bool[] arRunOnDayOfWeek;
		public DateTime dtTimeStart;
		public DateTime dtTimeEnd;
		public int iPriority;
	}
	
	public class BannerType
	{
		public BannerType() {}

		public int idBannerType;
		public string sName;
		public int iWidth;
		public int iHeight;
	}	

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class Banner
	{
		public Banner() {}

		public int idBanner;
		public int idBannerType;
		public string sName;
		public string sURL;
		public string sDeviceLink;

		public NullableDateTime dtCreated;

		public NullableDateTime dtChanged;

		public string sCreatedBy;
		public string sChangedBy;
		public StagingStatus byStatus;
		public string sAssetInfo;

		public BannerType BannerType;
		public ScheduleItem[] ScheduleItems;
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class IdleLoop
	{
		public IdleLoop() {}
		public int idIdleLoop;
		public string sName;
		public string sInfo;
		
		public NullableDateTime dtChanged;
		
		public NullableDateTime dtCreated;
		
		public string sChangedBy;
		public string sCreatedBy;
		public StagingStatus byStatus;
		public IdleLoopElement[] IdleLoopElements;
		public ScheduleItem[] ScheduleItems;
	}

	public class IdleLoopElement
	{
		public IdleLoopElement() {}
		public int idIdleLoopElement;
		public int idIdleLoop;
		public int iSeq;
		public string sName;
		public string sMIMEType;
		public string sURL;
		public int cSecsPlayingTime;
		
		public object dtChanged;
		
		public string sChangedBy;
		public string sAssetInfo;
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd",IsNullable=true)]
	public class NullableDateTime
	{
		public NullableDateTime() {}

		public DateTime dt;
		public bool bIsNull;
	}
}

*/