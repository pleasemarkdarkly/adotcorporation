using System;
using System.Data;
using System.IO;
using System.Web;
using MMD.Central.CentralLib.Data;
//using MMD.Central.CentralLib.DataObjects;
using MMD.Central.CentralLib.Utilities;
using MMD.Central.CentralLib.XmlObjects;

namespace MMD.Central.CentralLib.Commands
{
	/// <summary>
	/// Summary description for Commands.
	/// </summary>
	public class GroupTypeDelete : CommandBase
	{
		int idGroupType;
		
		public GroupTypeDelete(CSWContext ctx) : base(ctx)	{}

		public override void InitParameters() 
		{
			idGroupType = int.Parse(Context.Params["idGroupType"]);
		}
		
		public override void Execute() 
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idGroupType", DbType.Int32, idGroupType );
			cParams.Add("@sTypeName", DbType.String, ParameterDirection.Output, null, 255);
			cParams = Context.Config.MainDatabase.execSP_OutputParams("sprCSW_GroupTypeDelete", cParams);
			Context.SetStatusMessage( StatusMsg.GroupTypeDeleted, cParams["@sTypeName"].Value );
		}
	}


	public class GroupTypeAdd : CommandBase
	{
		string sTypeName;

		public GroupTypeAdd(CSWContext ctx) : base(ctx) {}
		public override void InitParameters()
		{
			sTypeName = Context.Params["sTypeName"];
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@sTypeName", DbType.String, sTypeName, 255 );
			Context.Config.MainDatabase.execSP("sprCSW_GroupTypeAdd", cParams);
			Context.SetStatusMessage( StatusMsg.GroupTypeAdded, sTypeName );
		}
	}

	public class GroupTypeUpdate : CommandBase
	{
		int idGroupType;
		string sTypeName;
		string sOldTypeName;

		public GroupTypeUpdate(CSWContext ctx) : base(ctx) {}

		public override void InitParameters()
		{
			idGroupType = int.Parse(Context.Params["idGroupType"]);
			sTypeName = Context.Params["sTypeName"];
			sOldTypeName = Context.Params["sOldTypeName"];
		}

		public override void Execute()
		{
			if(sTypeName != sOldTypeName)
			{
				Parameters cParams = new Parameters();
				cParams.AddInput("@idGroupType", DbType.Int32, idGroupType);
				cParams.AddInput("@sTypeName", DbType.String, sTypeName, 255);
				Context.Config.MainDatabase.execSP("sprCSW_GroupTypeUpdate", cParams);
			}
			Context.SetStatusMessage( StatusMsg.GroupTypeUpdated, sOldTypeName, sTypeName);
		}
	}

	public class GroupAdd : CommandBase
	{
		int idGroupType;
		string sTypeName;
		string sInfo;
		string sName;

		public GroupAdd(CSWContext ctx) : base(ctx) {}
		
		public override void InitParameters()
		{
			idGroupType = int.Parse(Context.Params["idGroupType"]);
			sTypeName = Context.Params["sTypeName"];
			sName = Context.Params["sName"];
			sInfo = Context.Params["sInfo"];
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idGroupType", DbType.Int32, idGroupType);
			cParams.AddInput("@sName", DbType.String, sName, 255);
			cParams.AddInput("@sInfo", DbType.String, sInfo, 1000);
			Context.Config.MainDatabase.execSP("spCSW_GroupAdd", cParams);
			Context.SetStatusMessage( StatusMsg.GroupAdded, sName, sTypeName );
		}
	}

	public class GroupDelete : CommandBase
	{
		int idGroup;
		string sName;
		string sTypeName;

		public GroupDelete(CSWContext ctx) : base(ctx) {}

		public override void InitParameters()
		{
			idGroup = int.Parse(Context.Params["idGroup"]);
			sName = Context.Params["sName"];
			sTypeName = Context.Params["sTypeName"];
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idGroup", DbType.Int32, idGroup );
			Context.Config.MainDatabase.execSP("spCSW_GroupDelete", cParams);
			Context.SetStatusMessage( StatusMsg.GroupDeleted, sName, sTypeName );
		}
	}

	public class GroupUpdate : CommandBase
	{
		public GroupUpdate(CSWContext ctx) : base(ctx) {}

		int idGroup;
		string sName, sOldName, sTypeName, sInfo;

		public override void InitParameters()
		{
			idGroup = int.Parse(Context.Params["idGroup"]);
			sName = Context.Params["sName"];
			sOldName = Context.Params["sOldName"];
			sTypeName = Context.Params["sTypeName"];
			sInfo = Context.Params["sInfo"];
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idGroup", DbType.Int32, idGroup);
			cParams.AddInput("@sName", DbType.String, sName, 255);
			cParams.AddInput("@sInfo", DbType.String, sInfo, 1000);
			Context.Config.MainDatabase.execSP("spCSW_GroupUdate", cParams);
			Context.SetStatusMessage( StatusMsg.GroupUpdated, sOldName, sName, sTypeName );
		}
	}

/*
	public class BannerAdd : CommandBase
	{
		public BannerAdd(CSWContext ctx) : base(ctx) {}

		int idBannerType;
		string sName;
		HttpPostedFile File;
		string sAssetInfo;
		string sURL;
		string sDeviceLink;
		string sCreatedBy;
		
		public override void InitParameters()
		{
			idBannerType = Context.Params.Int("idBannerType",0);
			sName = Context.Params["sName"];
			File = Context.Request.Files[0];
			DataObjects.PostedFile.ValidateBannerPosting(File);
			sAssetInfo = DataObjects.PostedFile.GetAssetInfo(File);
			sURL = PostedFile.GenerateUniqueFileName(File);
			sDeviceLink = Context.Params["sDeviceLink"];
			if(sDeviceLink == "")	sDeviceLink = null;
			sCreatedBy = Context.UserName;
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idBannerType",DbType.Int32, idBannerType);
			cParams.AddInput("@sName",DbType.String, sName, 100);
			cParams.AddInput("@sURL",DbType.String, sURL, 500);
			if(sDeviceLink != null) cParams.AddInput("@sDeviceLink", DbType.String, sDeviceLink, 500);
			cParams.AddInput("@sCreatedBy", DbType.String, sCreatedBy, 100);
			cParams.AddInput("@sAssetInfo", DbType.String, sAssetInfo, 500); 
			cParams.Add("@iReturn", DbType.Int32, ParameterDirection.ReturnValue);
			cParams = Context.Config.MainDatabase.execSP_OutputParams("sprCSW_BannerAdd",cParams);
			int iReturn = Convert.ToInt32(cParams["@iReturn"].Value);
			if(iReturn == 0)
			{
				Context.LoadListViewFromSession = false;
				Context.SetStatusMessage(StatusMsg.BannerAdded,sName);
				string sFileName = Context.Server.MapPath( Context.Config.AdvertisementDirectory(true) + "/" + sURL );
				File.SaveAs( sFileName );
			}
			else
				Context.SetMessage(iReturn);

		}
	}

	public class BannerUpdate : CommandBase
	{
		public BannerUpdate(CSWContext ctx) : base(ctx) {}

		int idBanner;
		int idBannerType;
		string sName;
		HttpPostedFile File;
		string sAssetInfo;
		string sURL;
		string sDeviceLink;
		string sChangedBy;
		
		public override void InitParameters()
		{
			idBanner = Context.Params.Int("idBanner",0);
			idBannerType = Context.Params.Int("idBannerType",0);
			sName = Context.Params["sName"];
			File = Context.Request.Files[0];
			DataObjects.PostedFile.ValidateBannerPosting(File);
			sAssetInfo = DataObjects.PostedFile.GetAssetInfo(File);
			sURL = PostedFile.GenerateUniqueFileName(File);
			sDeviceLink = Context.Params["sDeviceLink"];
			if(sDeviceLink == "")	sDeviceLink = null;
			sChangedBy = Context.UserName;
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idBanner",DbType.Int32, idBanner);
			cParams.AddInput("@idBannerType",DbType.Int32, idBannerType);
			cParams.AddInput("@sName",DbType.String, sName, 100);
			cParams.AddInput("@sURL",DbType.String, sURL, 500);
			if(sDeviceLink != null) cParams.AddInput("@sDeviceLink", DbType.String, sDeviceLink, 500);
			cParams.AddInput("@sChangedBy", DbType.String, sChangedBy, 100);
			cParams.AddInput("@sAssetInfo", DbType.String, sAssetInfo, 500); 
			cParams.Add("@iReturn", DbType.Int32, ParameterDirection.ReturnValue);
			cParams = Context.Config.MainDatabase.execSP_OutputParams("sprCSW_BannerUpdate",cParams);
			int iReturn = Convert.ToInt32(cParams["@iReturn"].Value);
			if(iReturn == 0)
			{
				Context.LoadListViewFromSession = false;
				Context.SetStatusMessage(StatusMsg.BannerUpdated,sName);
				string sFileName = Context.Server.MapPath( Context.Config.AdvertisementDirectory(true) + "/" + sURL );
				File.SaveAs( sFileName );
			}
			else
				Context.SetMessage(iReturn);
		}
	}

	public class BannerDelete : CommandBase
	{
		public BannerDelete(CSWContext ctx) : base(ctx) {}

		int idBanner;

		public override void InitParameters() 
		{
			idBanner = Context.Params.Int("idBanner",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idBanner",DbType.Int32, idBanner);
			Context.Config.MainDatabase.execSP("sprCSW_BannerDelete",cParams);
			Context.SetStatusMessage(StatusMsg.BannerDeleted);
		}
	}

	public class BannerUndo : CommandBase
	{
		public BannerUndo(CSWContext ctx) : base(ctx) {}

		int idBanner;

		public override void InitParameters() 
		{ 
			idBanner = Context.Params.Int("idBanner",0); 
		}
		
		public override void Execute()
		{
			DataSet dsStaging = Context.Config.MainDatabase.execSQL_DS("select * from tblMMD_BannerStaging where idBanner = " + idBanner);
			StagingStatus byStatus = (StagingStatus)(byte)dsStaging.Tables[0].Rows[0]["byStatus"];

			if(byStatus == StagingStatus.New || byStatus == StagingStatus.Changed)
			{
				//delete staging file
				System.IO.File.Delete( Context.Server.MapPath( Context.Config.AdvertisementDirectory(true) +"/" + dsStaging.Tables[0].Rows[0]["sURL"]));
			}

			if(byStatus == StagingStatus.Changed)
			{
				//copy deployed file to staging directory
				DataSet dsDeploy = Context.Config.MainDatabase.execSQL_DS("select * from tblMMD_Banner where idBanner = " + idBanner);
				System.IO.File.Copy(
					Context.Server.MapPath( Context.Config.AdvertisementDirectory(false) + "/" + dsDeploy.Tables[0].Rows[0]["sURL"] ),
					Context.Server.MapPath( Context.Config.AdvertisementDirectory(true) + "/" + dsDeploy.Tables[0].Rows[0]["sURL"] ),
					true);
			}


			Parameters cParams = new Parameters();
			cParams.AddInput("@idBanner",DbType.Int32, idBanner);
			Context.Config.MainDatabase.execSP("sprCSW_BannerUndo",cParams);
			Context.SetStatusMessage(StatusMsg.BannerReverted);
			Context.LoadListViewFromSession = false;
		}
	}

	public class BannerSubmit : CommandBase
	{
		public BannerSubmit(CSWContext ctx) : base(ctx) {}

		int idBanner;

		public override void InitParameters() 
		{ 
			idBanner = Context.Params.Int("idBanner",0); 
		}
		
		public override void Execute()
		{
			DataSet dsStaging = Context.Config.MainDatabase.execSQL_DS("select * from tblMMD_BannerStaging where idBanner = " + idBanner);
			DataSet dsDeploy = Context.Config.MainDatabase.execSQL_DS("select * from tblMMD_Banner where idBanner = " + idBanner);

			switch((StagingStatus)(byte)dsStaging.Tables[0].Rows[0]["byStatus"])
			{
				case StagingStatus.Changed : 
					//delete deploy, copy staging to deploy
					System.IO.File.Delete( Context.Server.MapPath( Context.Config.AdvertisementDirectory(false) + "/" + dsDeploy.Tables[0].Rows[0]["sURL"]) );
					goto case StagingStatus.New;
				case StagingStatus.New : 
					//copy staging to deploy
					System.IO.File.Copy(
						Context.Server.MapPath( Context.Config.AdvertisementDirectory(true) + "/" + dsStaging.Tables[0].Rows[0]["sURL"] ),
						Context.Server.MapPath( Context.Config.AdvertisementDirectory(false) + "/" + dsStaging.Tables[0].Rows[0]["sURL"] ),
						true);
					break;
				case StagingStatus.Deleted :
					//delete from staging and deploy
					System.IO.File.Delete( Context.Server.MapPath( Context.Config.AdvertisementDirectory(true) + "/" + dsStaging.Tables[0].Rows[0]["sURL"]) );
					System.IO.File.Delete( Context.Server.MapPath( Context.Config.AdvertisementDirectory(false) + "/" + dsDeploy.Tables[0].Rows[0]["sURL"]) );
					break;					
			}

			Parameters cParams = new Parameters();
			cParams.AddInput("@idBanner",DbType.Int32, idBanner);
			Context.Config.MainDatabase.execSP("sprCSW_BannerSubmit",cParams);
			Context.SetStatusMessage(StatusMsg.BannerSubmit);
			Context.LoadListViewFromSession = false;
		}
	}

	public class BannerSubmitType : CommandBase
	{
		public BannerSubmitType(CSWContext ctx) : base(ctx) {}

		int idBannerType;

		public override void InitParameters() 
		{ 
			idBannerType = Context.Params.Int("idBannerType",0); 
		}
		
		public override void Execute()
		{
			DataSet dsStaging = Context.Config.MainDatabase.execSQL_DS("select * from tblMMD_BannerStaging where idBannerType = " + idBannerType);
			DataSet dsDeploy = Context.Config.MainDatabase.execSQL_DS("select * from tblMMD_Banner where idBannerType = " + idBannerType);
			DataRow[] rows;
			foreach(DataRow row in dsStaging.Tables[0].Rows)
			{
				switch((StagingStatus)(byte)row["byStatus"])
				{
					case StagingStatus.Changed : 
						//delete deploy, copy staging to deploy
						rows = dsDeploy.Tables[0].Select("idBanner = " + row["idBanner"]);
						System.IO.File.Delete( Context.Server.MapPath( Context.Config.AdvertisementDirectory(false) + "/" + rows[0]["sURL"]) );
						goto case StagingStatus.New;
					case StagingStatus.New : 
						//copy staging to deploy
						System.IO.File.Copy(
							Context.Server.MapPath( Context.Config.AdvertisementDirectory(true) + "/" + dsStaging.Tables[0].Rows[0]["sURL"] ),
							Context.Server.MapPath( Context.Config.AdvertisementDirectory(false) + "/" + dsStaging.Tables[0].Rows[0]["sURL"] ),
							true);
						break;
					case StagingStatus.Deleted :
						//delete from staging and deploy
						rows = dsDeploy.Tables[0].Select("idBanner = " + row["idBanner"]);
						System.IO.File.Delete( Context.Server.MapPath( Context.Config.AdvertisementDirectory(true) + "/" + dsStaging.Tables[0].Rows[0]["sURL"]) );
						System.IO.File.Delete( Context.Server.MapPath( Context.Config.AdvertisementDirectory(false) + "/" + rows[0]["sURL"]) );
						break;					
				}
			}

			Parameters cParams = new Parameters();
			cParams.AddInput("@idBannerType",DbType.Int32, idBannerType);
			Context.Config.MainDatabase.execSP("sprCSW_BannerSubmitBannerType",cParams);
			Context.SetStatusMessage(StatusMsg.BannerSubmitType);
			Context.LoadListViewFromSession = false;
		}
	}
*/

	public class ScheduleAdd : CommandBase
	{
		public ScheduleAdd(CSWContext ctx) : base(ctx) {}

		DateTime dtBegin;
		DateTime dtEnd;
		DateTime dtTimeStart;
		DateTime dtTimeEnd;
		byte bfDayOfWeek;
		int iPriority;

		public override void InitParameters()
		{
			dtBegin = Convert.ToDateTime(Context.Params["dtBegin"]);
			dtEnd = Convert.ToDateTime(Context.Params["dtEnd"]);
			dtTimeStart = Convert.ToDateTime(Context.Params["dtTimeStart"]);
			dtTimeEnd = Convert.ToDateTime(Context.Params["dtTimeEnd"]);
			iPriority = Context.Params.Int("iPriority",0);

			bfDayOfWeek = 0;
			for(int i=1;i<=7;i++)
				if(0 != Context.Params.Int("dow_" + i,0))
					bfDayOfWeek |= Convert.ToByte(1 << i);
			
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@dtBegin",DbType.DateTime, dtBegin);
			cParams.AddInput("@dtEnd",DbType.DateTime,dtEnd);
			cParams.AddInput("@dtTimeStart",DbType.DateTime,dtTimeStart);
			cParams.AddInput("@dtTimeEnd",DbType.DateTime,dtTimeEnd);
			cParams.AddInput("@iPriority",DbType.Int32,iPriority);
			cParams.AddInput("@bfDayOfWeek",DbType.Byte,bfDayOfWeek);
			cParams.Add("@idSchedule",DbType.Int32, ParameterDirection.Output);
			cParams = Context.Config.MainDatabase.execSP_OutputParams("sprCSW_ScheduleAdd",cParams);

			Context.SetStatusMessage(StatusMsg.ScheduleAdded);

			int idSchedule = Convert.ToInt32(cParams["@idSchedule"].Value);

			//insert into reltable ( idSchedule, reltablekey ) values ( @idSchedule, 
			string sRelTable = Context.Params.SessionString("sched_in_reltable");
			string sRelTableKey = Context.Params.SessionString("sched_in_reltablekey");
			string sKeyValue = Context.Params.SessionString("sched_in_keyvalue");

			string sSQL = "insert into " + sRelTable + " ( idSchedule, " + sRelTableKey + " ) " +
					"values ( " + idSchedule + ", " + sKeyValue + " )" ;

			Context.Config.MainDatabase.execSQL(sSQL);

			Context.Params["sched_out_updated"] = "1";
			Context.Params["sched_out_id"] = idSchedule.ToString();
		}
	}

	public class ScheduleUpdate : CommandBase
	{
		public ScheduleUpdate(CSWContext ctx) : base(ctx) {}

		int idSchedule;
		DateTime dtBegin;
		DateTime dtEnd;
		DateTime dtTimeStart;
		DateTime dtTimeEnd;
		byte bfDayOfWeek;
		int iPriority;

		public override void InitParameters()
		{
			idSchedule = Context.Params.Int("idSchedule",0);
			dtBegin = Convert.ToDateTime(Context.Params["dtBegin"]);
			dtEnd = Convert.ToDateTime(Context.Params["dtEnd"]);
			dtTimeStart = Convert.ToDateTime(Context.Params["dtTimeStart"]);
			dtTimeEnd = Convert.ToDateTime(Context.Params["dtTimeEnd"]);
			iPriority = Context.Params.Int("iPriority",0);

			bfDayOfWeek = 0;
			for(int i=1;i<=7;i++)
				if(0 != Context.Params.Int("dow_" + i,0))
					bfDayOfWeek |= Convert.ToByte(1 << i);
			
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@dtBegin",DbType.DateTime, dtBegin);
			cParams.AddInput("@dtEnd",DbType.DateTime,dtEnd);
			cParams.AddInput("@dtTimeStart",DbType.DateTime,dtTimeStart);
			cParams.AddInput("@dtTimeEnd",DbType.DateTime,dtTimeEnd);
			cParams.AddInput("@iPriority",DbType.Int32,iPriority);
			cParams.AddInput("@bfDayOfWeek",DbType.Byte,bfDayOfWeek);
			cParams.AddInput("@idSchedule",DbType.Int32, idSchedule);
			Context.Config.MainDatabase.execSP("sprCSW_ScheduleUpdate",cParams);

			Context.SetStatusMessage(StatusMsg.ScheduleUpdated);

			Context.Params["sched_out_updated"] = "1";
			Context.Params["sched_out_id"] = idSchedule.ToString();
		}
	}


	public class ScheduleDelete : CommandBase
	{
		public ScheduleDelete(CSWContext ctx) : base(ctx) {}

		int idSchedule;

		public override void InitParameters()
		{
			idSchedule = Context.Params.Int("idSchedule",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idSchedule",DbType.Int32, idSchedule);
			Context.Config.MainDatabase.execSP("sprCSW_ScheduleDelete",cParams);
			Context.SetStatusMessage(StatusMsg.ScheduleDeleted);

			//insert into reltable ( idSchedule, reltablekey ) values ( @idSchedule, 
			string sRelTable = Context.Params.SessionString("sched_in_reltable");

			string sSQL = "delete from " + sRelTable + " where idSchedule = " + idSchedule;
			Context.Config.MainDatabase.execSQL(sSQL);


			Context.Params["sched_out_updated"] = "1";
		}
	}
/*
	public class IdleLoopAdd : CommandBase
	{
		public IdleLoopAdd(CSWContext ctx) : base(ctx) {}

		string sName;
		string sInfo;

		public override void InitParameters()
		{
			sName = Context.Params["sName"];
			sInfo = Context.Params["sInfo"];
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@sName",DbType.String, sName, 100);
			cParams.AddInput("@sInfo",DbType.String, sInfo, 1000);
			cParams.AddInput("@sUserName", DbType.String, Context.UserName, 100);
			Context.Config.MainDatabase.execSP("sprCSW_IdleLoopAdd", cParams);
			Context.SetStatusMessage(StatusMsg.IdleLoopAdded,sName);
			Context.LoadListViewFromSession = false;
		}
	}

	public class IdleLoopUpdate : CommandBase
	{
		public IdleLoopUpdate(CSWContext ctx) : base(ctx) {}
		
		int idIdleLoop;
		string sOldName, sName;
		string sInfo;

		public override void InitParameters()
		{
			idIdleLoop = Context.Params.Int("idIdleLoop",0);
			sName = Context.Params["sName"];
			sOldName = Context.Params["sOldName"];
			if(sOldName == null)
			{
				DataObjects.IdleLoop idleLoop = new DataObjects.IdleLoop(Context, idIdleLoop, false);
				sOldName = idleLoop.sName;
			}
			sInfo = Context.Params["sInfo"];
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idIdleLoop",DbType.Int32, idIdleLoop);
			cParams.AddInput("@sName",DbType.String, sName,100);
			cParams.AddInput("@sInfo",DbType.String, sInfo, 1000);
			cParams.AddInput("@sUserName",DbType.String, Context.UserName, 100);
			Context.Config.MainDatabase.execSP("sprCSW_IdleLoopUpdate", cParams);
			Context.SetStatusMessage(StatusMsg.IdleLoopUpdated,sOldName,sName);
		}
	}

	public class IdleLoopDelete : CommandBase
	{
		public IdleLoopDelete(CSWContext ctx) : base(ctx) {}
		
		int idIdleLoop;

		public override void InitParameters()
		{
			idIdleLoop = Context.Params.Int("idIdleLoop",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idIdleLoop", DbType.Int32, idIdleLoop);
			Context.Config.MainDatabase.execSP("sprCSW_IdleLoopDelete", cParams);
			DataObjects.IdleLoop idleLoop = new DataObjects.IdleLoop(this.Context, idIdleLoop, false);
			Context.SetStatusMessage(StatusMsg.IdleLoopDeleted,idleLoop.sName);
		}	
	}
*/
	//doubles for adding idle loop elements when idIdleLoopElement = 0
/*
 * 	public class IdleLoopElementUpdate : CommandBase
	{
		public IdleLoopElementUpdate(CSWContext ctx) : base(ctx) {}

		int idIdleLoop;
		int idIdleLoopElement;
		string sName;
		int cSecsPlayingTime;
		HttpPostedFile File;
		string sAssetInfo;
		string sURL;
		int iSeq;

		public override void InitParameters()
		{
			idIdleLoop = Context.Params.Int("idIdleLoop",0);
			idIdleLoopElement = Context.Params.Int("idIdleLoopElement",0);
			sName = Context.Params["sName"];
			cSecsPlayingTime = Context.Params.Int("cSecsPlayingtime",0);
			iSeq = Context.Params.Int("iSeq",0);
			File = Context.Request.Files[0];
			DataObjects.PostedFile.ValidateFramePosting(File);
			sAssetInfo = DataObjects.PostedFile.GetAssetInfo(File);
			sURL = DataObjects.PostedFile.GenerateUniqueFileName(File);
		}

		public override void Execute()
		{	
			DataObjects.IdleLoop idleLoop = new DataObjects.IdleLoop(Context, idIdleLoop, false);
			
			DataObjects.IdleLoopElement idleElement = null;
			if(idIdleLoopElement != 0)
				idleElement = new DataObjects.IdleLoopElement(Context, idIdleLoopElement);
			
			Parameters cParams = new Parameters();
			cParams.AddInput("@idIdleLoop", DbType.Int32, idIdleLoop);
			cParams.Add("@idIdleLoopElement", DbType.Int32, ParameterDirection.InputOutput , idIdleLoopElement);
			cParams.AddInput("@sName", DbType.String, sName, 100);
			cParams.AddInput("@iSeq", DbType.Int32, iSeq);
			cParams.AddInput("@sMIMEType", DbType.String, File.ContentType, 100);
			cParams.AddInput("@sURL", DbType.String, sURL, 512);
			cParams.AddInput("@cSecsPlayingTime", DbType.Int32, cSecsPlayingTime);
			cParams.AddInput("@sChangedBy", DbType.String, Context.UserName, 100);
			cParams.AddInput("@sAssetInfo", DbType.String, sAssetInfo, 512);
			cParams = Context.Config.MainDatabase.execSP_OutputParams("sprCSW_IdleLoopElementUpdate",cParams);

			string sFileName = Context.Config.IdleLoopLocalPath(true) + "\\" + sURL;
			File.SaveAs( sFileName );
	
			if(idIdleLoopElement == 0)
			{
				Context.Params["idIdleLoopElement"] = cParams["@idIdleLoopElement"].Value.ToString();
				Context.SetStatusMessage(StatusMsg.IdleLoopElementAdded,sName,idleLoop.sName);
				Context.LoadListViewFromSession = false;
			}
			else
				Context.SetStatusMessage(StatusMsg.IdleLoopElementUpdated,idleElement.sName,idleLoop.sName,sName);
		}
	}

	public class IdleLoopElementDelete : CommandBase 
	{
		public IdleLoopElementDelete(CSWContext ctx) : base(ctx) {}

		int idIdleLoopElement;

		public override void InitParameters()
		{
			idIdleLoopElement = Context.Params.Int("idIdleLoopElement",0);
		}

		public override void Execute()
		{
			DataObjects.IdleLoopElement idleElement = new DataObjects.IdleLoopElement(Context, idIdleLoopElement);
			DataObjects.IdleLoop idleLoop = new DataObjects.IdleLoop(Context, idleElement.idIdleLoop, false);

			Parameters cParams = new Parameters();
			cParams.AddInput("@idIdleLoopElement", DbType.Int32, idIdleLoopElement);
			Context.Config.MainDatabase.execSP("sprCSW_IdleLoopElementDelete",cParams);

			System.IO.File.Delete( Context.Config.IdleLoopLocalPath(true) + "\\" + idleElement.sURL );
			Context.LoadListViewFromSession = false;
			Context.SetStatusMessage(StatusMsg.IdleLoopElementDeleted,idleElement.sName,idleLoop.sName);
		}
	}

	public class IdleLoopElementInsert : CommandBase
	{
		public IdleLoopElementInsert(CSWContext ctx) : base(ctx) {}
		
		int idIdleLoopElement;
		int idElementInsertBefore;

		public override void InitParameters()
		{
			idIdleLoopElement = Context.Params.Int("idIdleLoopElement",0);
			string sValue = ListView.ListView.FindFirstCheckboxValue(Context);
			if(sValue == null)
				throw new CSWCommandException((int)ErrMsg.IdleLoopElementInsertNotChecked);
			
			idElementInsertBefore = Convert.ToInt32(sValue);
		}

		public override void Execute()
		{
			DataObjects.IdleLoopElement ile = new DataObjects.IdleLoopElement(Context, idIdleLoopElement);
			DataObjects.IdleLoop il = new DataObjects.IdleLoop(Context,ile.idIdleLoop);
			Parameters cParams = new Parameters();
			cParams.AddInput("@idIdleLoopElement", DbType.Int32, idIdleLoopElement);
			cParams.AddInput("@idElementInsertBefore", DbType.Int32, idElementInsertBefore);
			Context.Config.MainDatabase.execSP("sprCSW_IdleLoopElementInsert", cParams);
			Context.SetStatusMessage(StatusMsg.IdleLoopElementInserted,ile.sName,il.sName,ile.iSeq,il.Elements.Count);
		}
	}


	public class IdleLoopUndo : CommandBase
	{
		public IdleLoopUndo(CSWContext ctx) : base(ctx) {}

		int idIdleLoop;

		public override void InitParameters() 
		{ 
			idIdleLoop = Context.Params.Int("idIdleLoop",0); 
		}
		
		public override void Execute()
		{
			DataObjects.IdleLoop idleLoop = new DataObjects.IdleLoop(Context,idIdleLoop);

			//setup files
			StagingStatus byStatus = idleLoop.byStatus;

			if(byStatus == StagingStatus.New || byStatus == StagingStatus.Changed)
			{
				//delete staging file
				foreach(DataObjects.IdleLoopElement ile in idleLoop.Elements.Array)
					System.IO.File.Delete( Context.Config.IdleLoopLocalPath(true) +"\\" + ile.sURL );
			}

			if(byStatus == StagingStatus.Changed)
			{
				//get deployed idleloop
				DataObjects.IdleLoop ilDeployed = new DataObjects.IdleLoop(Context,idIdleLoop,true,DataObjects.DataObjectLocation.Deployed);
				
				//copy deployed file to staging directory
				foreach(DataObjects.IdleLoopElement ile in ilDeployed.Elements.Array)
					System.IO.File.Copy(
						Context.Config.IdleLoopLocalPath(false) + "\\" + ile.sURL ,
						Context.Config.IdleLoopLocalPath(true) + "\\" + ile.sURL ,
						true);
			}

			if(byStatus == StagingStatus.Changed) 
				idleLoop = new DataObjects.IdleLoop(Context,idIdleLoop);

			Parameters cParams = new Parameters();
			cParams.AddInput("@idIdleLoop",DbType.Int32, idIdleLoop);
			Context.Config.MainDatabase.execSP("sprCSW_IdleLoopUndo",cParams);
			Context.SetStatusMessage(StatusMsg.IdleLoopReverted,idleLoop.sName);
			Context.LoadListViewFromSession = false;
		}
	}

	public class IdleLoopSubmit : CommandBase
	{
		public IdleLoopSubmit(CSWContext ctx) : base(ctx) {}

		int idIdleLoop;

		public override void InitParameters() 
		{ 
			idIdleLoop = Context.Params.Int("idIdleLoop",0); 
		}
		
		public override void Execute()
		{
			DataObjects.IdleLoop ilStaging = new DataObjects.IdleLoop(Context,idIdleLoop,true,DataObjects.DataObjectLocation.Staging);
			DataObjects.IdleLoop ilDeploy = null;
			if(ilStaging.byStatus != StagingStatus.New) 
				ilDeploy = new DataObjects.IdleLoop(Context,idIdleLoop,true,DataObjects.DataObjectLocation.Deployed);

			switch(ilStaging.byStatus)
			{
				case StagingStatus.Changed : 
					//delete deploy, copy staging to deploy
					foreach(DataObjects.IdleLoopElement ile in ilDeploy.Elements.Array)
						System.IO.File.Delete( Context.Config.IdleLoopLocalPath(false) + "\\" +  ile.sURL );
					goto case StagingStatus.New;
				case StagingStatus.New : 
					//copy staging to deploy
					foreach(DataObjects.IdleLoopElement ile in ilStaging.Elements.Array)
						System.IO.File.Copy(
							Context.Config.IdleLoopLocalPath(true) + "\\" + ile.sURL,
							Context.Config.IdleLoopLocalPath(false) + "\\" + ile.sURL,
							true);
					break;
				case StagingStatus.Deleted :
					//delete from staging and deploy
					foreach(DataObjects.IdleLoopElement ile in ilStaging.Elements.Array)
						System.IO.File.Delete( Context.Config.IdleLoopLocalPath(true) + "\\" + ile.sURL);

					foreach(DataObjects.IdleLoopElement ile in ilDeploy.Elements.Array)
						System.IO.File.Delete( Context.Config.IdleLoopLocalPath(false) + "\\" + ile.sURL);
					
					break;					
			}

			Parameters cParams = new Parameters();
			cParams.AddInput("@idIdleLoop",DbType.Int32, idIdleLoop);
			Context.Config.MainDatabase.execSP("sprCSW_IdleLoopSubmit",cParams);
			Context.SetStatusMessage(StatusMsg.IdleLoopSubmit,ilStaging.sName);
			Context.LoadListViewFromSession = false;
		}
	}


	public class FileDelete : CommandBase
	{
		public FileDelete(CSWContext ctx) : base(ctx) {}

		public override void Execute()
		{
			
		}
	}
*/
}
