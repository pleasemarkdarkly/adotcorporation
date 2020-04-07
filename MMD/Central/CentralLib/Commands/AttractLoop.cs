using System;
using System.Data;
using System.IO;
using System.Web;
using MMD.Central.CentralLib.Data;
using MMD.Central.CentralLib.Utilities;
using MMD.Central.CentralLib.XmlObjects;

namespace MMD.Central.CentralLib.Commands
{
	/// <summary>
	/// Summary description for CommandsAttractLoop.
	/// </summary>
	public class FileAdd : CommandBase
	{
		public FileAdd(CSWContext ctx) : base(ctx)	{}

		public enum FileType
		{
			Image = 1,
			Video = 2,
			ScreenML = 3
		}
		
		string sName;
		FileType fileType;
		HttpPostedFile file;
		string sFileName;
		string sMIMEType;
		string sAssetInfo;
		int idStoreConcept;
		MMDPostedFileType postedFileType;
		string sSuccessPage;

		public override void InitParameters() 
		{
			if(Context.Request.Files.Count == 0)
				throw new CSWCommandException((int)ErrMsg.ImageInvalidMIMEType,sMIMEType);

			file = Context.Request.Files[0];
			sName = Context.Params["sName"];
			fileType = (FileType)Context.Params.Int("iType",0);
			sMIMEType = file.ContentType;
			sAssetInfo = PostedFile.GetAssetInfo(file);
			sFileName = PostedFile.GenerateUniqueFileName(file);
			idStoreConcept = Context.Params.Int("idStoreConcept",0);
			postedFileType = (MMDPostedFileType)Context.Params.Int("MMDPostedFileType",0);
			sSuccessPage = Context.Params["sSuccessPage"];

			switch(fileType)
			{
				case FileType.Image: PostedFile.ValidateImagePosting(sMIMEType); break;
				case FileType.ScreenML : PostedFile.ValidateScreenMLPosting(sMIMEType); break;
				case FileType.Video : PostedFile.ValidateVideoPosting(sMIMEType); break;
			}

			PostedFile.CheckFileExists(Context,idStoreConcept,postedFileType,sFileName);

		}
		
		public override void Execute() 
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@sName", DbType.String, sName, 200);
			cParams.AddInput("@idStoreConcept", DbType.Int32, idStoreConcept);
			cParams.AddInput("@iType", DbType.Byte, (byte)postedFileType);
			cParams.AddInput("@iFileMediaType", DbType.Byte, (byte)fileType);
			cParams.AddInput("@sMIMEType", DbType.String, sMIMEType, 100);
			cParams.AddInput("@sFileName", DbType.String, sFileName, 256);
			cParams.AddInput("@sChangedBy", DbType.String, Context.UserName, 30);
			cParams.AddInput("@sAssetInfo", DbType.String, sAssetInfo, 1000);
			cParams.Add("@idFile",DbType.Int32, ParameterDirection.Output);
			
			cParams = Context.Config.MainDatabase.execSP_OutputParams("sprCSW_FileAdd", cParams);

			Context.SetStatusMessage( StatusMsg.FileAdded  );
			

			sFileName =  Context.Config.GetLocalPath(idStoreConcept, postedFileType, sFileName);
			file.SaveAs( sFileName );

			if(sSuccessPage != null)
				Context.TargetPage = sSuccessPage;
		}
	}

	public class FileUpdate : CommandBase
	{

		
		int idFile;
		string sName;
		FileMediaType fileType;
		HttpPostedFile file;
		string sFileName;
		string sMIMEType;
		string sAssetInfo;
		int bNewFile;
		int idStoreConcept;
		MMDPostedFileType postedFileType;
		string sSuccessPage;

		public FileUpdate(CSWContext ctx) : base(ctx)	{}

		public override void InitParameters() 
		{
			sSuccessPage = Context.Params["sSuccessPage"];
			sName = Context.Params["sName"];
			idStoreConcept = Context.Params.Int("idStoreConcept",0);
			idFile = Context.Params.Int("idFile",0);
			bNewFile = Context.Params.Int("bNewFile",0);
			postedFileType = (MMDPostedFileType)Context.Params.Int("MMDPostedFileType",0);

			if(bNewFile == 1 && Context.Request.Files.Count > 0)
				file = Context.Request.Files[0];
			else
				file = null;

			if(bNewFile == 1 && file == null)
				throw new CSWCommandException((int)ErrMsg.ImageInvalidMIMEType,sMIMEType);
			

			if(file != null)
			{
				fileType = (FileMediaType)Context.Params.Int("iType",0);
				sMIMEType = file.ContentType;
				sAssetInfo = PostedFile.GetAssetInfo(file);
				sFileName = PostedFile.GenerateUniqueFileName(file);
				switch(fileType)
				{
					case FileMediaType.Image: PostedFile.ValidateImagePosting(sMIMEType); break;
					case FileMediaType.ScreenML : PostedFile.ValidateScreenMLPosting(sMIMEType); break;
					case FileMediaType.Video : PostedFile.ValidateVideoPosting(sMIMEType); break;
				}
			}
			else
			{
				fileType = (FileMediaType)0;
				sMIMEType = "";
				sAssetInfo = "";
				sFileName = "";
			}




		}
		
		public override void Execute() 
		{
			MMDFilePersister fp = new MMDFilePersister(Context,false,Location.Staging);
			MMDFile mmdfile = fp.GetFileById(idFile);

			if(bNewFile == 1)
			{
				PostedFile.CheckFileExists(Context,idStoreConcept,postedFileType,sFileName);

				//delete existing file
				mmdfile.DeleteFile(Context);
				file.SaveAs( Context.Config.GetLocalPath(idStoreConcept,postedFileType,sFileName)  );
			}
			else if(mmdfile.idStoreConcept != idStoreConcept)
			{
				PostedFile.CheckFileExists(Context,idStoreConcept,postedFileType,mmdfile.sFileName);

				//move file to new store concept
				System.IO.File.Move( Context.Config.GetLocalPath(mmdfile.idStoreConcept,mmdfile.FileType,mmdfile.sFileName),
							         Context.Config.GetLocalPath(idStoreConcept,postedFileType,mmdfile.sFileName) );
			}

			Parameters cParams = new Parameters();
			cParams.AddInput("@idFile",DbType.Int32, idFile);
			cParams.AddInput("@idStoreConcept", DbType.Int32, idStoreConcept);
			cParams.AddInput("@sName", DbType.String, sName, 200);
			cParams.AddInput("@iNewFile", DbType.Int32, bNewFile);
			cParams.AddInput("@iType", DbType.Byte, (byte)postedFileType);
			cParams.AddInput("@iFileMediaType", DbType.Byte, (byte)fileType);
			cParams.AddInput("@sMIMEType", DbType.String, sMIMEType, 100);
			cParams.AddInput("@sFileName", DbType.String, sFileName, 256);
			cParams.AddInput("@sChangedBy", DbType.String, Context.UserName, 30);
			cParams.AddInput("@sAssetInfo", DbType.String, sAssetInfo, 1000);
			
			Context.Config.MainDatabase.execSP("sprCSW_FileUpdate", cParams);

			Context.SetStatusMessage( StatusMsg.FileUpdated   );
			if(sSuccessPage != null)
				Context.TargetPage = sSuccessPage;
		}
	}

	public class FileRemove : CommandBase
	{
		
		int idFile;

		public FileRemove(CSWContext ctx) : base(ctx)	{}

		public override void InitParameters() 
		{
			idFile = Context.Params.Int("idFile",0);
		}
		
		public override void Execute() 
		{
			MMDFilePersister fp = new MMDFilePersister(Context,false,Location.Staging);
			MMDFile mmdfile = fp.GetFileById(idFile);

			mmdfile.DeleteFile(Context);
			
			Context.Config.MainDatabase.execSQL("delete tblMMD_File where idFile = " + idFile);

			Context.SetStatusMessage( StatusMsg.FileRemoved   );
		}
	}

	public class AddAttractLoopSequence : CommandBase
	{
		public AddAttractLoopSequence(CSWContext ctx) : base(ctx) {}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			string sName = "New Attract Loop";
			cParams.AddInput("@sName",DbType.String, sName, 200);
			cParams.AddInput("@sChangedBy", DbType.String, Context.UserName, 100);
			cParams.Add("@idIdleLoop", DbType.Int32, ParameterDirection.Output);
			cParams = Context.Config.MainDatabase.execSP_OutputParams("sprCSW_AttractLoopAdd",cParams);
			Context.Params["idIdleLoop"] = cParams["@idIdleLoop"].Value.ToString();
			Context.SetStatusMessage( StatusMsg.IdleLoopAdded, sName);
		}
	}

	public class AttractLoopDelete : CommandBase
	{
		public AttractLoopDelete(CSWContext ctx) : base(ctx) {}
		
		int idIdleLoop;

		public override void InitParameters()
		{
			idIdleLoop = Context.Params.Int("idIdleLoop",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idIdleLoop", DbType.Int32, idIdleLoop);
			Context.Config.MainDatabase.execSP("sprCSW_AttractLoopDelete", cParams);
			//DataObjects.IdleLoop idleLoop = new DataObjects.IdleLoop(this.Context, idIdleLoop, false);
			IdleLoopPersister ilp = new IdleLoopPersister(Context,false);
			
			Context.SetStatusMessage(StatusMsg.IdleLoopDeleted,ilp.GetIdleLoopById(idIdleLoop).sName);
		}	
	}

	public class AttractLoopUndo : CommandBase
	{
		public AttractLoopUndo(CSWContext ctx) : base(ctx) {}

		int idIdleLoop;
		string sName;

		public override void InitParameters() 
		{ 
			idIdleLoop = Context.Params.Int("idIdleLoop",0); 
			sName = Context.Params["sName"];
		}
		
		public override void Execute()
		{

			Parameters cParams = new Parameters();
			cParams.AddInput("@idIdleLoop",DbType.Int32, idIdleLoop);
			Context.Config.MainDatabase.execSP("sprCSW_AttractLoopUndo",cParams);
			Context.SetStatusMessage(StatusMsg.IdleLoopReverted,sName);
			Context.LoadListViewFromSession = false;
		}
	}

	public class AttractLoopSubmit : CommandBase
	{
		public AttractLoopSubmit(CSWContext ctx) : base(ctx) {}

		int idIdleLoop;

		public override void InitParameters() 
		{ 
			idIdleLoop = Context.Params.Int("idIdleLoop",0); 
		}
		
		public override void Execute()
		{
			IdleLoopPersister ilp = new IdleLoopPersister(Context,false);

			Parameters cParams = new Parameters();
			cParams.AddInput("@idIdleLoop",DbType.Int32, idIdleLoop);
			Context.Config.MainDatabase.execSP("sprCSW_AttractLoopSubmit",cParams);
			Context.SetStatusMessage(StatusMsg.IdleLoopSubmit,ilp.GetIdleLoopById(idIdleLoop).sName);
			Context.LoadListViewFromSession = false;
		}
	}

	public class AttractLoopSubmitAll : CommandBase
	{
		public AttractLoopSubmitAll(CSWContext ctx) : base(ctx) {}

		public override void Execute()
		{
			DataSet dsLoops = Context.Config.MainDatabase.execSQL_DS("select idIdleLoop from tblMMD_IdleLoopStaging where byStatus <> 0");
			foreach(DataRow row in dsLoops.Tables[0].Rows)
			{
				Context.Params["idIdleLoop"] = row["idIdleLoop"].ToString();
				Context.ExecuteCommand("AttractLoopSubmit");
			}
			Context.SetStatusMessage(StatusMsg.IdleLoopSubmitAll);
		}
	}

	public class AttractLoopUndoAll : CommandBase
	{
		public AttractLoopUndoAll(CSWContext ctx) : base(ctx) {}
		public override void Execute()
		{
			DataSet dsLoops = Context.Config.MainDatabase.execSQL_DS("select idIdleLoop from tblMMD_IdleLoopStaging where byStatus <> 0");
			foreach(DataRow row in dsLoops.Tables[0].Rows)
			{
				Context.Params["idIdleLoop"] = row["idIdleLoop"].ToString();
				Context.Params["sName"] = "";
				Context.ExecuteCommand("AttractLoopUndo");
			}
			Context.SetStatusMessage(StatusMsg.IdleLoopSubmitAll);
		}
	}

	public class AttractLoopUpdate : CommandBase
	{
		public AttractLoopUpdate(CSWContext ctx) : base(ctx) {}
	
		string sName;
		string sInfo;
		int idIdleLoop;
		int idStoreConcept;
			
		public override void InitParameters()
		{
			idIdleLoop = Context.Params.Int("idIdleLoop",0);
			sName = Context.Params["sName"];
			sInfo = Context.Params["sInfo"];
			idStoreConcept = Context.Params.Int("idStoreConcept",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idIdleLoop", DbType.Int32, idIdleLoop);
			cParams.AddInput("@idStoreConcept", DbType.Int32, idStoreConcept);
			cParams.AddInput("@sName", DbType.String, sName, 100);
			cParams.AddInput("@sInfo", DbType.String, sInfo, 1000);
			cParams.AddInput("@sChangedBy", DbType.String, Context.UserName, 100);
			Context.Config.MainDatabase.execSP("sprCSW_AttractLoopUpdate",cParams);
			Context.SetStatusMessage(StatusMsg.IdleLoopUpdated,sName);
		}
	}

	public class AttractLoopItemUpdate : CommandBase
	{
		public AttractLoopItemUpdate(CSWContext ctx) : base(ctx) {}

		int idIdleLoopElement;
		int idIdleLoop;
		string sName;
		int iFrameType;
		string sTemplate;
		int cShowTime;
		int iStartTime;
		int iEndTime;
		string sLink;
		int idRecListType;
		int idProductType;
		int cItemsPerPage;
		int idItem;
		int idFile;

		public override void InitParameters()
		{
			idIdleLoopElement = Context.Params.Int("idIdleLoopElement",0);
			idIdleLoop = Context.Params.Int("idIdleLoop",0);
			sName = Context.Params["sName"];
			iFrameType = Context.Params.Int("iFrameType",0);
			sTemplate = Context.Params["sTemplate"];
			cShowTime = Context.Params.Int("cShowTime",0);
			iStartTime = Context.Params.Int("iStartTime",0);
			iEndTime = Context.Params.Int("iEndTime",0);
			sLink = Context.Params["sLink"];
			idRecListType = 0;
			idProductType = 0;
			cItemsPerPage = Context.Params.Int("cItemsPerPage",0);
			idItem = Context.Params.Int("idItem",0);
			idFile = Context.Params.Int("idFile",0);

			FrameType frameType = (FrameType)iFrameType;
			if(frameType == FrameType.Promotion || frameType == FrameType.RecList)
			{
				string sID = Context.Params["idRecListType"];
				if(frameType == FrameType.Promotion)
					idRecListType = Convert.ToInt32(sID);
				else if(frameType == FrameType.RecList)
				{
					string[] arIDs = sID.Split(',');
					idRecListType = Convert.ToInt32(arIDs[0]);
					idProductType = Convert.ToInt32(arIDs[1]);
				}
				if(sName == null || sName == "")
				{
					DataSet ds = Context.Config.MainDatabase.execSQL_DS("select display from vwCSW_AttractLoopItemRecList where id = '" + sID + "'");
					sName = Convert.ToString(ds.Tables[0].Rows[0][0]);
				}
			}
		}

		object NullInt(int iValue, int iNullValue)
		{
			if(iValue == iNullValue)
				return null;
			else
				return (object)iValue;
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.Add("@idIdleLoopElement", DbType.Int32, ParameterDirection.InputOutput, idIdleLoopElement);
			cParams.AddInput("@idIdleLoop", DbType.Int32, idIdleLoop);
			cParams.AddInput("@sName", DbType.String, sName, 100);
			cParams.AddInput("@iFrameType", DbType.Int32, iFrameType);
			cParams.AddInput("@idFile", DbType.Int32, idFile);
			cParams.AddInput("@sChangedBy", DbType.String, Context.UserName, 100);
			cParams.AddInput("@sTemplate", DbType.String, sTemplate, 100);
			cParams.AddInput("@cShowTime", DbType.Int32, NullInt(cShowTime,0));
			cParams.AddInput("@iStartTime", DbType.Int32, NullInt(iStartTime,0));
			cParams.AddInput("@iEndTime", DbType.Int32, NullInt(iEndTime,0));
			cParams.AddInput("@sLink", DbType.String, sLink, 500);
			cParams.AddInput("@idRecListType", DbType.Int32, NullInt(idRecListType,0));
			cParams.AddInput("@idProductType", DbType.Int32, NullInt(idProductType,0));
			cParams.AddInput("@cItemsPerPage", DbType.Int32, NullInt(cItemsPerPage,0));
			cParams.AddInput("@idItem", DbType.Int32, NullInt(idItem,0));
			cParams = Context.Config.MainDatabase.execSP_OutputParams("sprCSW_AttractLoopItemUpdate",cParams);
			if(idIdleLoopElement != 0)
				Context.SetStatusMessage(StatusMsg.AttractLoopItemUpdated,sName);
			else
			{
				Context.SetStatusMessage(StatusMsg.AttractLoopItemAdded,sName);
				Context.Params["idIdleLoopElement"] = cParams["@idIdleLoopElement"].Value.ToString();
			}
		}
	}

	public class AttractLoopItemMove : CommandBase
	{
		public AttractLoopItemMove(CSWContext ctx) : base(ctx) {}
		
		int idIdleLoopElement;
		int up;

		public override void InitParameters()
		{
			idIdleLoopElement = Context.Params.Int("idIdleLoopElement",0);
			up = Context.Params.Int("up",0);
		}

		public override void Execute() 
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idIdleLoopElement",DbType.Int32, idIdleLoopElement);
			cParams.AddInput("@up", DbType.Int32, up);
			cParams.AddInput("@sChangedBy", DbType.String, Context.UserName, 100);
			Context.Config.MainDatabase.execSP("sprCSW_AttractLoopItemMove",cParams);
			if(up == 1)
				Context.SetStatusMessage(StatusMsg.AttractLoopItemMovedUp);
			else
				Context.SetStatusMessage(StatusMsg.AttractLoopItemMovedDown);
		}
	}

	public class AttractLoopItemDelete : CommandBase
	{
		public AttractLoopItemDelete(CSWContext ctx) : base(ctx) {}

		int idIdleLoopElement;

		public override void InitParameters()
		{
			idIdleLoopElement = Context.Params.Int("idIdleLoopElement",0);

		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idIdleLoopElement",DbType.Int32, idIdleLoopElement);
			cParams.AddInput("@sChangedBy", DbType.String, Context.UserName, 100);
			Context.Config.MainDatabase.execSP("sprCSW_AttractLoopItemDelete",cParams);
			Context.SetStatusMessage(StatusMsg.AttractLoopItemRemoved);
		}


	}

}