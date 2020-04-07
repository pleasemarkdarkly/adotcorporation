using System;
using System.Data;
using System.IO;
using System.Web;
using MMD.Central.CentralLib.Data;
using MMD.Central.CentralLib.Utilities;
using MMD.Central.CentralLib.XmlObjects;


namespace MMD.Central.CentralLib.Commands
{
	public class StoreUpdate : CommandBase
	{
		public StoreUpdate(CSWContext ctx) : base(ctx) {}

		int idStore;
		StorePersister sp;
		Store store;

		public override void InitParameters()
		{
			idStore = Context.Params.Int("idStore",0);
			sp = new StorePersister(Context,true);
			store = sp.GetStoreById(idStore);
			store.sName = Context.Params["sName"];
			store.sAddress = Context.Params["sAddress"];
			store.sCity = Context.Params["sCity"];
			store.sState = Context.Params["sState"];
			store.sZip = Context.Params["sZip"];
			store.sCountry = Context.Params["sCountry"];
			store.sPhone = Context.Params["sPhone"];
			store.sMarEmail = Context.Params["sMarEmail"];
			store.sProblemEmail = Context.Params["sProblemEmail"];
		}

		public override void Execute()
		{
			sp.UpdateStore(store);		
			Context.SetStatusMessage(StatusMsg.StoreUpdated);
		}
	}

	public class DeviceUpdate : CommandBase
	{
		public DeviceUpdate(CSWContext ctx) : base(ctx) {}

		int idStore;
		int idDevice;
		StorePersister sp;
		Device device;

		public override void InitParameters()
		{
			idStore = Context.Params.Int("idStore",0);
			idDevice = Context.Params.Int("idDevice",0);
			sp = new StorePersister(Context,true);
			device = sp.GetDeviceById(idStore,idDevice);
			device.idLocationMode = Context.Params.Int("idLocationMode",device.idLocationMode);
			device.idIdleLoop = Context.Params.Int("idIdleLoop",device.idIdleLoop);
			device.cSecsUntilIdle = Context.Params.Int("cSecsUntilIdle",device.cSecsUntilIdle);
			int idDefaultMediaType = Context.Params.Int("idDefaultMediaType",-1);
			device.DefaultMediaType = (idDefaultMediaType == -1) ? null : NullableInt.CreateInstance(idDefaultMediaType);
			int idDefaultGenre = Context.Params.Int("idDefaultGenre",-1);
			device.DefaultGenre = (idDefaultGenre == -1) ? null : NullableInt.CreateInstance(idDefaultGenre);
		}

		public override void Execute()
		{
			sp.UpdateDevice(device);
			Context.SetStatusMessage(StatusMsg.DeviceUpdated);
		}
	}

	public class SyncReports : CommandBase
	{
		public SyncReports(CSWContext ctx) : base(ctx) {}

		public override void Execute()
		{
			Context.Config.MainDatabase.execSP("sprCSW_ReportProcessLogs",null);
			Context.SetStatusMessage("Reports updated from logged device usage.");
		}
	}
}
