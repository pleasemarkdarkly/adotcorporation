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
	public class StoreConcept
	{
		public StoreConcept() {}

		[XmlAttribute("dbid")]
		public int idStoreConcept;

		[XmlElement("Name",DataType="normalizedString")]
		public string sName;

		[XmlElement("Info",DataType="normalizedString")]
		public string sInfo;

		public StoreConcept(DataRow row, DataSetHolder dsh)
		{
			idStoreConcept = Convert.ToInt32(row["idStoreConcept"]);
			sName = Convert.ToString(row["sName"]);
	//		sInfo = Convert.ToString(row["sInfo"]);
		}
	}

	public class Store
	{
		public Store() {}

		[XmlAttribute("dbid")]
		public int idStore;

		[XmlAttribute("concept")]
		public int idStoreConcept;

		[XmlElement("Name",DataType="normalizedString")]
		public string sName;

		[XmlElement("Address",DataType="normalizedString")]
		public string sAddress;

		[XmlElement("City",DataType="normalizedString")]
		public string sCity;

		[XmlElement("State",DataType="normalizedString")]
		public string sState;

		[XmlElement("Zip",DataType="normalizedString")]
		public string sZip;

		[XmlElement("Country",DataType="normalizedString")]
		public string sCountry;

		[XmlElement("MarEmail",DataType="normalizedString")]
		public string sMarEmail;

		[XmlElement("ProblemEmail",DataType="normalizedString")]
		public string sProblemEmail;

		[XmlElement("HostName",DataType="normalizedString")]
		public string sHostName;

		[XmlElement("ConnectInfo",DataType="normalizedString")]
		public string sConnectInfo;

		[XmlElement("Config",DataType="normalizedString")]
		public string sxConfig;

		[XmlElement("Phone",DataType="normalizedString")]
		public string sPhone;

		[XmlElement("DateLastLogUpdate")]
		public DateTime dtLastLogUpdate;

		//private int idCurrentPackage;

		[XmlElement("CurrentPackage")]
		public Package CurrentPackage;

		[XmlElement("StoreConcept")]
		public StoreConcept storeConcept;

		[XmlElement(ElementName="DateCreated",IsNullable=true)]
		public NullableDateTime dtCreated;

		[XmlElement(ElementName="DateDeactivated",IsNullable=true)]
		public NullableDateTime dtDeactivated;

		[XmlElement(ElementName="Status",IsNullable=false)]
		public byte byStatus;

		[XmlArray("Devices",IsNullable=true)]
		public Device[] Devices;

		public Store(DataRow row, DataSetHolder dsh)
		{
			idStore = Convert.ToInt32(row["idStore"]);
			idStoreConcept = Convert.ToInt32(row["idStoreConcept"]);
			sName = Convert.ToString(row["sName"]);
			sAddress = Convert.ToString(row["sAddress"]);
			sCity = Convert.ToString(row["sCity"]);
			sState = Convert.ToString(row["sState"]);
			sZip = Convert.ToString(row["sZip"]);
			sCountry = Convert.ToString(row["sCountry"]);
			sPhone = Convert.ToString(row["sPhone"]);
			sMarEmail = Convert.ToString(row["sMarEmail"]);
			sProblemEmail = Convert.ToString(row["sProblemEmail"]);
			sHostName = Convert.ToString(row["sHostName"]);
			sConnectInfo = Convert.ToString(row["sConnectInfo"]);
			sxConfig = Convert.ToString(row["sxConfig"]);
			dtCreated = NullableDateTime.CreateInstance(row["dtCreated"]);
			dtDeactivated = NullableDateTime.CreateInstance(row["dtDeactivated"]);
			storeConcept = new StoreConcept(row,dsh);
			DataRow[] rows = row.GetChildRows("Store_Device");
			Devices = new Device[rows.Length];
			for(int i=0;i<rows.Length;i++)
				Devices[i] = new Device(rows[i], dsh);
		}

		public void Fill(DataRow row, DataSetHolder dsh, bool bNew,CSWContext ctx)
		{
			row["idStoreConcept"] = idStoreConcept;
			row["sName"] = sName;
			row["sAddress"] = sAddress;
			row["sCity"] = sCity;
			row["sState"] = sState;
			row["sZip"] = sZip;
			row["sCountry"] = sCountry;
			row["sPhone"] = sPhone;
			row["sMarEmail"] = sMarEmail;
			row["sProblemEmail"] = sProblemEmail;
			row["sHostName"] = sHostName;
			row["sConnectInfo"] = sConnectInfo;
			row["sxConfig"] = sxConfig;
			//row["sChangedBy"] = ctx.UserName;
			//row["dtDelta"] = DateTime.Now;
			//row["sDeltaAction"] = (bNew) ? 'A' : 'C';
			row["dtCreated"] = NullableDateTime.GetValue(dtCreated);
			row["dtDeactivated"] = NullableDateTime.GetValue(dtDeactivated);
			if(Devices != null)
			{
				foreach(Device device in Devices)
				{
					bool bNewRow = false;
					DataRow DeviceRow;
					if(0 == device.idDevice)
					{
						bNewRow = true;
						DeviceRow = dsh.Tables["Device"].NewRow();
					}
					else
					{
						DataRow[] DeviceRows = dsh.Tables["Device"].Select("idDevice = " + device.idDevice);
						bNewRow = DeviceRows.Length == 0;
						DeviceRow = (bNewRow) ? dsh.Tables["Device"].NewRow() : DeviceRows[0];
					}
					device.Fill(DeviceRow, dsh, bNewRow, ctx);
					DeviceRow.SetParentRow(row, dsh.Relations["Store_Device"]);
					if(bNewRow) dsh.Tables["Device"].Rows.Add(DeviceRow);
				}
			}
		}
	}

	public class LocationMode
	{
		public LocationMode() {}

		[XmlAttribute("dbid")]
		public int idLocationMode;

		[XmlAttribute("concept")]
		public int idStoreConcept;

		[XmlElement("Name",DataType="normalizedString")]
		public string sName;

		[XmlElement("HomePage",DataType="normalizedString")]
		public string sHomePage;

		[XmlElement("SingleMediaType")]
		public bool bSingleMediaType;

		[XmlElement("SingleGenre")]
		public bool bSingleGenre;

		[XmlElement("AddDefaultGenre")]
		public bool bAddDefaultGenre;

		[XmlElement("MediaTypeDefault")]
		public NullableInt idMediaTypeDefault;

		[XmlElement("MediaTypeDefault2")]
		public NullableInt idMediaTypeDefault2;

		[XmlElement("idGenreDefault")]
		public NullableInt idGenreDefault;

		[XmlElement("StoreConcept")]
		public StoreConcept storeConcept;

		public LocationMode(DataRow row, DataSetHolder dsh) : this(row, dsh, Location.Staging) {}

		public LocationMode(DataRow row, DataSetHolder dsh, Location location)
		{
			idLocationMode = Convert.ToInt32(row["idLocationMode"]);
			idStoreConcept = Convert.ToInt32(row["idStoreConcept"]);
			sName = Convert.ToString(row["sName"]);
			sHomePage = Convert.ToString(row["sHomePage"]);
			bSingleMediaType = Convert.ToBoolean(row["bSingleMediaType"]);
			bSingleGenre = Convert.ToBoolean(row["bSingleGenre"]);
			bAddDefaultGenre = Convert.ToBoolean(row["bAddDefaultGenre"]);
			idMediaTypeDefault = NullableInt.CreateInstance(row["idMediaTypeDefault"]);
			idMediaTypeDefault2 = NullableInt.CreateInstance(row["idMediaTypeDefault2"]);
			idGenreDefault = NullableInt.CreateInstance(row["idGenreDefault"]);
			storeConcept = new StoreConcept(row,dsh);
		}

		public void Fill(DataRow row, DataSetHolder dsh, bool bNew,CSWContext ctx, Location location)
		{
			row["idStoreConcept"] = idStoreConcept;
			row["sName"] = sName;
			row["sHomePage"] = sHomePage;
			row["bSingleMediaType"] = bSingleMediaType;
			row["bSingleGenre"] = bSingleGenre;
			row["bAddDefaultGenre"] = bAddDefaultGenre;
			row["idMediaTypeDefault"] = NullableInt.GetValue(idMediaTypeDefault);
			row["idMediaTypeDefault2"] = NullableInt.GetValue(idMediaTypeDefault2);
			row["idGenreDefault"] = NullableInt.GetValue(idGenreDefault);
			row["dtDelta"] = DateTime.Now;
			row["sDeltaAction"] = (bNew) ? 'A' : 'C';
		}
	}

	public class Device
	{
		public Device() {}

		[XmlAttribute("dbid")]
		public int idDevice;

		[XmlAttribute("dbidStore")]
		public int idStore;

		[XmlAttribute("dbidLocationMode")]
		public int idLocationMode;

/*
		[XmlElement("dbidFeatureList")]
		public int idFeatureList;
*/
		[XmlElement("dbidIdleLoop")]
		public int idIdleLoop;

		[XmlElement("LanguageDefault")]
		public string sLanguageDefault;

		[XmlElement("MacAddress")]
		public string sMacAddress;

		[XmlElement("Status")]
		public int iStatus;

		[XmlElement("SecsUntilIdle")]
		public int cSecsUntilIdle;

		[XmlElement("IsCalibrated")]
		public bool bIsCalibrated;

		[XmlElement("VolumeDefault")]
		public int shVolumeDefault;

		[XmlElement("BrightnessDefault")]
		public int shBrightnessDefault;

		[XmlElement("dbidScreenDefault")]
		public int idScreenDefault;

		public NullableInt DefaultMediaType;
		public NullableInt DefaultGenre;

		[XmlElement("LocationMode")]
		LocationMode locationmode;

		public Device(DataRow row, DataSetHolder dsh) : this(row, dsh, Location.Staging) {}

		public Device(DataRow row, DataSetHolder dsh, Location location)
		{
			idDevice = Convert.ToInt32(row["idDevice"]);
			idStore = Convert.ToInt32(row["idStore"]);
			idLocationMode = Convert.ToInt32(row["idLocationMode"]);
//			idFeatureList = Convert.ToInt32(row["idFeatureList"]);
			idIdleLoop = Convert.ToInt32(row["idIdleLoop"]);
			sLanguageDefault = Convert.ToString(row["sLanguageDefault"]);
			sMacAddress = Convert.ToString(row["sMacAddress"]);
			iStatus = Convert.ToInt32(row["iStatus"]);
			cSecsUntilIdle = Convert.ToInt32(row["cSecsUntilIdle"]);
			bIsCalibrated = Convert.ToBoolean(row["bIsCalibrated"]);
			shVolumeDefault = Convert.ToInt32(row["shVolumeDefault"]);
			shBrightnessDefault =Convert.ToInt32(row["shBrightnessDefault"]);
			idScreenDefault = Convert.ToInt32(row["idScreenDefault"]);
			DefaultMediaType = NullableInt.CreateInstance(row["idDefaultMediaType"]);
			DefaultGenre = NullableInt.CreateInstance(row["idDefaultGenre"]);

			DataRow[] rowMode = dsh.Tables["Mode"].Select("idLocationMode = " + idLocationMode);
			if(rowMode.Length > 0)
				locationmode = new LocationMode(rowMode[0],dsh, location);
			else
				locationmode = null;
		}

		public void Fill(DataRow row, DataSetHolder dsh, bool bNew,CSWContext ctx)
		{
			row["idStore"] = idStore;
			row["idLocationMode"] = idLocationMode;
//			row["idFeatureList"] = idFeatureList;
			row["idIdleLoop"] = idIdleLoop;
			row["sLanguageDefault"] = sLanguageDefault;
			row["sMacAddress"] = sMacAddress;
			row["iStatus"] = iStatus;
			row["cSecsUntilIdle"] = cSecsUntilIdle;
			row["bIsCalibrated"] = bIsCalibrated;
			row["shVolumeDefault"] = shVolumeDefault;
			row["shBrightnessDefault"] = shBrightnessDefault;
			row["idScreenDefault"] = idScreenDefault;
			row["idDefaultMediaType"] = NullableInt.GetValue(DefaultMediaType);
			row["idDefaultGenre"] = NullableInt.GetValue(DefaultGenre);
			row["dtDelta"] = DateTime.Now;
			row["sDeltaAction"] = (bNew) ? 'A' : 'C';
		}
	}

	public class StorePersister : PersisterBase
	{

		public StorePersister(CSWContext ctx, bool bUpdating, Location location) : base(ctx,bUpdating,location) {}

		public StorePersister(CSWContext ctx, bool bUpdating) : this(ctx,bUpdating,Location.Staging) {}

		protected override DataSetHolder GetDataSet()
		{
			DataSetHolder ds = new DataSetHolder();
			Data.Connection conn = Context.Config.MainDatabase;
			ds.AddTable(conn,"Store","select * from tblMMD_StoreConfiguration",_bUpdating);
			ds.AddTable(conn,"Device","select * from tblMMD_DeviceConfiguration",_bUpdating);
			ds.FillTable(conn,"Concept","select * from tblMMD_StoreConcept");
			ds.FillTable(conn,"Mode","select * from tblMMD_LocationMode");

			ds.Relations.Add("Store_Device",ds.Tables["Store"].Columns["idStore"],ds.Tables["Device"].Columns["idStore"]);
			return ds;		
		}

		protected override void UpdateDataSet()
		{
			_dsh.EnforceConstraints = false;
			_dsh.Update("Store","idStore");
			_dsh.Update("Device","idDevice");
			_dsh.EnforceConstraints = true;
		}

		protected void RemoveFromDataSet()
		{
			_dsh.EnforceConstraints = false;
			_dsh.Update("Device","idDevice");
			_dsh.Update("Store","idStore");
			_dsh.EnforceConstraints = true;
		}

		public Store[] GetStores()
		{
			DataRow[] rows = _dsh.Tables["Store"].Select();
			Store[] stores = new Store[rows.Length];
			for(int i=0;i < rows.Length;i++)
				stores[i] = new Store(rows[i], _dsh);
			return stores;
		}

		public Store GetStoreById(int idStore)
		{
			DataRow[] rows = _dsh.Tables["Store"].Select("idStore = " + idStore);
			Store store = new Store(rows[0],_dsh);
			return store;
		}

		public StoreConcept[] GetStoreConcepts()
		{
			DataRow[] rows = _dsh.Tables["Concept"].Select();
			StoreConcept[] scs = new StoreConcept[rows.Length];
			for(int i = 0;i < rows.Length;i++)
				scs[i] = new StoreConcept(rows[i], _dsh);
			return scs;
		}

		public void UpdateStore(Store store)
		{
			UpdateStoreHelper(store);
			UpdateDataSet();
		}

		public void UpdateStores(Store[] stores)
		{
			for(int i=0;i<stores.Length;i++)
				UpdateStoreHelper(stores[i]);
			UpdateDataSet();
		}

		protected void UpdateStoreHelper(Store store)
		{
			DataRow[] rows = _dsh.Tables["Store"].Select("idStore = " + store.idStore);
			DataRow row;
			bool bNew = rows.Length == 0;
			row = (bNew) ? _dsh.Tables["Store"].NewRow() : rows[0];
			store.Fill(row,_dsh,bNew,this.Context);
		}

		public void RemoveStore(int idStore)
		{
				
			DataRow row = _dsh.Tables["Store"].Rows.Find(idStore);
			DataRow[] rows = row.GetChildRows("Store_Device");
			for(int i = 0;i < rows.Length;i++)
			{
				rows[i].Delete();
			}
			row.Delete();
			RemoveFromDataSet();
		}

		public LocationMode[] GetLocationModes()
		{
			DataRow[] rows = _dsh.Tables["Mode"].Select();
			LocationMode[] modes = new LocationMode[rows.Length];
			for(int i = 0;i < rows.Length;i++)
				modes[i] = new LocationMode(rows[i], _dsh, _location);
			return modes;
		}

		public Device GetDeviceById(int idStore, int idDevice)
		{
			DataRow[] rows = _dsh.Tables["Device"].Select("idStore = " + idStore + " and idDevice = " + idDevice);
			if(rows.Length > 0 )
				return new Device(rows[0],_dsh);
			else
				return null;
		}

		public Device[] GetDevicesByStoreID(int idStore)
		{
			DataRow[] rows = _dsh.Tables["Device"].Select("idStore = " + idStore);
			Device[] devices = new Device[rows.Length];
			for(int i = 0;i < rows.Length;i++)
				devices[i] = new Device(rows[i], _dsh, _location);
			return devices;
		}

		public void RemoveDevices(Device[] arDevices)
		{
			foreach(Device device in arDevices)
			{
				DataRow[] rows = _dsh.Tables["Device"].Select("idStore = " + device.idStore + " and idDevice = " + device.idDevice);
				foreach(DataRow row in rows)
					row.Delete();
			}
			RemoveFromDataSet();
		}

		public void UpdateDevice(Device Device)
		{
			UpdateDeviceHelper(Device);
			UpdateDataSet();
		}

		public void UpdateDevices(Device[] Devices)
		{
			for(int i=0;i<Devices.Length;i++)
				UpdateDeviceHelper(Devices[i]);
			UpdateDataSet();
		}

		protected void UpdateDeviceHelper(Device Device)
		{
			DataRow[] rows = _dsh.Tables["Device"].Select("idDevice = " + Device.idDevice);
			DataRow row;
			bool bNew = rows.Length == 0;
			row = (bNew) ? _dsh.Tables["Device"].NewRow() : rows[0];
			Device.Fill(row,_dsh,bNew,this.Context);
		}

		
	}

}
