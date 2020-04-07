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
	public enum EventScheduleType
	{
		PeriodicInMinutes = 0,
		Daily = 1,
		RunOnce = 2
	}

	public enum EventRunStatus
	{
		Disabled = 0,
		Ready = 1,
		Running = 2
	}

	public enum DaysOfTheWeek
	{
		Sunday = 1,
		Monday = 2,
		Tuesday = 4,
		Wednesday = 8, 
		Thursday = 16,
		Friday = 32,
		Saturday = 64
	}

	public enum MMDPackageLevel
	{
		Store = 1,
		Central = 2,
		Master = 3
	}

	public enum MMDPackageContentType
	{
		DatabaseTable = 1,
		DirectoryTree = 2
	}

	public enum MMDPackageDeltaStyle
	{
		Delta = 1,
		Replace = 2
	}

	public class PackageLevel
	{
		public PackageLevel() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idPackageLevel;

		[XmlElement(ElementName="Description",DataType="normalizedString",IsNullable=false)]
		public string sDesc;

		public PackageLevel(DataRow row)
		{
			idPackageLevel = Convert.ToInt32(row["idPackageLevel"]);
			sDesc = Convert.ToString(row["sDesc"]);
		}
	}

	public class PackageContentType
	{
		PackageContentType() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idDataType;

		[XmlElement(ElementName="Description",DataType="normalizedString",IsNullable=false)]
		public string sDescription;

		public PackageContentType(DataRow row)
		{
			idDataType = Convert.ToInt32(row["idDataType"]);
			sDescription = Convert.ToString(row["sDesc"]);
		}
	}

	public class PackageDeltaStyle
	{
		PackageDeltaStyle() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idDeltaStyle;

		[XmlElement(ElementName="Description",DataType="normalizedString",IsNullable=false)]
		public string sDescription;

		public PackageDeltaStyle(DataRow row)
		{
			idDeltaStyle = Convert.ToInt32(row["idDeltaStyle"]);
			sDescription = Convert.ToString(row["sDesc"]);
		}
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class Schedule
	{
		public Schedule() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idSchedule;

		[XmlElement(ElementName="BeginDate", IsNullable=true)]
		public NullableDateTime dtBegin;

		[XmlElement(ElementName="EndDate", IsNullable=true)]
		public NullableDateTime dtEnd;

		[XmlElement(ElementName="DayOfWeek",IsNullable=false)]
		public byte bfDayOfWeek;

		[XmlElement(ElementName="RunOnDayOfWeek", IsNullable=false)]
		public XmlObjects.RunOnDayOfWeek RunOnDayOfWeek;

		[XmlElement(ElementName="Priority", IsNullable=false)]
		public int iPriority;

		[XmlElement(ElementName="TimeStart",DataType="time", IsNullable=false)]
		public DateTime dtTimeStart;

		[XmlElement(ElementName="TimeEnd",DataType="time", IsNullable=false)]
		public DateTime dtTimeEnd;

//		[XmlElement("EventSchedules")]
//		public EventSchedule[] EventSchedules;

		public Schedule(DataRow row, DataSetHolder dsh) : this(row, dsh, Location.Staging) {}
				
		public Schedule(DataRow row, DataSetHolder dsh, Location location)
		{
			Initialize(row, dsh, location);
		}
		
		public void Initialize(DataRow row, DataSetHolder dsh, Location location)
		{
			idSchedule = Convert.ToInt32(row["idSchedule"]);
			dtBegin = NullableDateTime.CreateInstance(row["dtBegin"]);
			dtEnd = NullableDateTime.CreateInstance(row["dtEnd"]);
			if(row["bfDayOfWeek"] != System.DBNull.Value)
				bfDayOfWeek = Convert.ToByte(row["bfDayOfWeek"]);
			else
				bfDayOfWeek = 0;
			RunOnDayOfWeek = new XmlObjects.RunOnDayOfWeek(bfDayOfWeek);
			iPriority = Convert.ToInt32(row["iPriority"]);
			dtTimeStart = Convert.ToDateTime(row["dtTimeStart"]);
			dtTimeEnd = Convert.ToDateTime(row["dtTimeEnd"]);

//			DataRow[] rows = row.GetChildRows("S_ES");
//			EventSchedules = new EventSchedule[rows.Length];
//			for(int i = 0;i < rows.Length; i++)
//				EventSchedules[i] = new EventSchedule(rows[i], dsh, location);
		}

		public void Fill(DataRow row, DataSetHolder dsh, bool bNew ,CSWContext ctx, Location location)
		{
			if(!bNew)
				row["idSchedule"] = idSchedule;
			row["dtBegin"] = dtBegin;
			row["dtEnd"] = dtEnd;
			row["bfDayOfWeek"] = RunOnDayOfWeek.GetByteValue();
			row["iPriority"] = iPriority;
			row["dtTimeStart"] = dtTimeStart;
			row["dtTimeEnd"] = dtTimeEnd;
            
//			if(null != EventSchedules)
//			{
//				foreach(EventSchedule es in EventSchedules)
//				{
//					bool bNewRow = false;
//					DataRow esRow;
//					if(0 == es.idEventSchedule)
//					{
//						bNewRow = true;
//						esRow = dsh.Tables["EventSchedule"].NewRow();
//					}
//					else
//					{
//						DataRow[] esRows = dsh.Tables["EventSchedule"].Select("idEventSchedule = " + es.idEventSchedule);
//						bNewRow = esRows.Length == 0;
//						esRow = (bNewRow) ? dsh.Tables["EventSchedule"].NewRow() : esRows[0];
//					}
//					es.Fill(esRow, dsh, bNewRow, ctx, location);
//					esRow.SetParentRow(row, dsh.Relations["S_ES"]);
//					if(bNewRow) dsh.Tables["EventSchedule"].Rows.Add(esRow);
//				}
//				DataRow[] rows = dsh.Tables["EventSchedule"].Select("idSchedule = " + idSchedule);
//				for(int i = 0;i < rows.Length;i++)
//				{
//					bool bFound = false;
//					foreach(EventSchedule es in EventSchedules)
//					{
//						if(es.idEventSchedule == Convert.ToInt32(row["idEventSchedule"]))
//						{
//							bFound = true;
//						}
//					}
//					if(!bFound)
//						row.Delete();
//				}
//			}
		}	
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class EventSchedule
	{
		public EventSchedule() {}

		[XmlAttribute("dbid")]
		public int idEventSchedule;

		[XmlElement("Description")]
		public string sDesc;

		[XmlElement("Type")]
		public EventScheduleType eType;

		[XmlAttribute("dbidSchedule")]
		public NullableInt idSchedule;

		[XmlElement("PeriodMinutes",IsNullable=true)]
		public NullableInt iPeriodMinutes;

		[XmlElement("TimeOfDay",IsNullable=true)]
		public NullableDateTime dtTimeOfDay;

		[XmlElement("DaysOfWeek",IsNullable=true)]
		public byte bfDaysOfWeek;

		[XmlElement("RunOnDayOfWeek",IsNullable=true)]
		public XmlObjects.RunOnDayOfWeek RunOnDayOfWeek;

		[XmlElement("RunOnce",IsNullable=true)]
		public NullableDateTime dtRunOnce;

		[XmlElement("RunNow",IsNullable=false)]
		public bool bRunNow;

		[XmlElement("Priority",IsNullable=false)]
		public int iPriority;

		[XmlElement("Status",IsNullable=false)]
		public EventRunStatus eStatus;

		[XmlElement("DateLastRun",IsNullable=true)]
		public NullableDateTime dtLastRun;

		[XmlElement("Schedule",IsNullable=true)]
		public Schedule Schedule;
	
		[XmlElement("EventCommands")]
		public EventCommand[] EventCommands;

		public EventSchedule(DataRow row, DataSetHolder dsh) : this(row, dsh, Location.Staging) {}
				
		public EventSchedule(DataRow row, DataSetHolder dsh, Location location)
		{
			Initialize(row, dsh, location);
		}
		
		public void Initialize(DataRow row, DataSetHolder dsh, Location location)
		{
			idEventSchedule = Convert.ToInt32(row["idEventSchedule"]);
			sDesc = Convert.ToString(row["sDesc"]);
			eType = (EventScheduleType)Convert.ToInt32(row["byType"]);
			idSchedule = NullableInt.CreateInstance(row["idSchedule"]);
			iPeriodMinutes = NullableInt.CreateInstance(row["iPeriodMinutes"]);
			dtTimeOfDay = NullableDateTime.CreateInstance(row["dtTimeOfDay"]);
			if(row["bfDaysOfWeek"] != System.DBNull.Value)
				bfDaysOfWeek = Convert.ToByte(row["bfDaysOfWeek"]);
			else
				bfDaysOfWeek = 0;
			RunOnDayOfWeek = new XmlObjects.RunOnDayOfWeek(bfDaysOfWeek);
			dtRunOnce = NullableDateTime.CreateInstance(row["dtRunOnce"]);
			if(row["bRunNow"] != System.DBNull.Value)
				bRunNow = Convert.ToBoolean(row["bRunNow"]);
			else
				bRunNow = false;
			iPriority = Convert.ToInt32(row["iPriority"]);
			eStatus = (EventRunStatus)Convert.ToInt32(row["byStatus"]);
			dtLastRun = NullableDateTime.CreateInstance(row["dtLastRun"]);

			DataRow[] rows = row.GetChildRows("ES_EC");
			EventCommands = new EventCommand[rows.Length];
			for(int i = 0;i < rows.Length; i++)
				EventCommands[i] = new EventCommand(rows[i], dsh, location);
//			if(null != idSchedule)
//			{
//				rows = dsh.Tables["Schedule"].Select("idSchedule = " + idSchedule);
//				if(rows.Length > 0)
//					Schedule = new Schedule(rows[0], dsh, location);
//			}
		}

		public void Fill(DataRow row, DataSetHolder dsh, bool bNew ,CSWContext ctx, Location location)
		{
//			if(!bNew && null != Schedule)
//				row["idSchedule"] = Schedule.idSchedule;
			row["idSchedule"] = NullableInt.GetValue(idSchedule); // ?

			row["sDesc"] = sDesc;
			row["byType"] = eType;
			row["iPeriodMinutes"] = NullableInt.GetValue(iPeriodMinutes);
			row["dtTimeOfDay"] = NullableDateTime.GetValue(dtTimeOfDay);
			row["bfDaysOfWeek"] = RunOnDayOfWeek.GetByteValue();
			row["dtRunOnce"] = NullableDateTime.GetValue(dtRunOnce);
			row["bRunNow"] = bRunNow;
			row["iPriority"] = iPriority;
			row["byStatus"] = eStatus;
			row["dtLastRun"] = NullableDateTime.GetValue(dtLastRun);
            
			if(null != EventCommands)
			{
				foreach(EventCommand ec in EventCommands)
				{
					bool bNewRow = false;
					DataRow ecRow;
					if(0 == ec.idEventSchedule)
					{
						bNewRow = true;
						ecRow = dsh.Tables["EventCommand"].NewRow();
					}
					else
					{
						DataRow[] ecRows = dsh.Tables["EventCommand"].Select("idEventSchedule = " + ec.idEventSchedule + " and sNamespace = " + ec.sNamespace + " and sObject = " + ec.sObject + " and sMethod = " + ec.sMethod);
						bNewRow = ecRows.Length == 0;
						ecRow = (bNewRow) ? dsh.Tables["EventCommand"].NewRow() : ecRows[0];
					}
					ec.Fill(ecRow, dsh, bNewRow, ctx, location);
					ecRow.SetParentRow(row, dsh.Relations["ES_EC"]);
					if(bNewRow) dsh.Tables["EventCommand"].Rows.Add(ecRow);
				}
				DataRow[] rows = dsh.Tables["EventCommand"].Select("idEventSchedule = " + idEventSchedule);
				for(int i = 0;i < rows.Length;i++)
				{
					bool bFound = false;
					foreach(EventCommand ec in EventCommands)
					{
						if(ec.idEventSchedule == Convert.ToInt32(row["idEventSchedule"]))
						{
							bFound = true;
						}
					}
					if(!bFound)
						row.Delete();
				}
			}
		}
	}

	public class EventCommand
	{
		public EventCommand() {}

		[XmlAttribute("dbidEvent")]
		public int idEventSchedule;

		[XmlElement("dbidPackageLevel")]
		public int idPackageLevel;

		[XmlElement("Namespace",IsNullable=false)]
		public string sNamespace;

		[XmlElement("Object",IsNullable=false)]
		public string sObject;

		[XmlElement("Method",IsNullable=false)]
		public string sMethod;

		[XmlElement("Sequence",IsNullable=true)]
		public NullableInt iSequence;

		[XmlElement("Static",IsNullable=false)]
		public bool bStatic;

		[XmlElement("Singleton",IsNullable=false)]
		public bool bSingleton;

		[XmlElement("PackageLevel",IsNullable=false)]
		public PackageLevel PackageLevel;

		public EventCommand(DataRow row, DataSetHolder dsh) : this(row, dsh, Location.Staging) {}
				
		public EventCommand(DataRow row, DataSetHolder dsh, Location location)
		{
			Initialize(row, dsh, location);
		}
		
		public void Initialize(DataRow row, DataSetHolder dsh, Location location)
		{
			idEventSchedule = Convert.ToInt32(row["idEventSchedule"]);
			idPackageLevel = Convert.ToInt32(row["idPackageLevel"]);
			sNamespace = Convert.ToString(row["sNamespace"]);
			sObject = Convert.ToString(row["sObject"]);
			sMethod = Convert.ToString(row["sMethod"]);
			iSequence = NullableInt.CreateInstance(row["iSequence"]);
			bStatic = Convert.ToBoolean(row["bStatic"]);
			bSingleton = Convert.ToBoolean(row["bSingleton"]);

			DataRow[] rows = dsh.Tables["PackageLevel"].Select("idPackageLevel = " + idPackageLevel);
			if(rows.Length > 0)
				PackageLevel = new PackageLevel(rows[0]);
		}

		public void Fill(DataRow row, DataSetHolder dsh, bool bNew ,CSWContext ctx, Location location)
		{
			row["idEventSchedule"] = idEventSchedule;
			row["idPackageLevel"] = idPackageLevel;
			row["sNamespace"] = sNamespace;
			row["sObject"] = sObject;
			row["sMethod"] = sMethod;
			row["iSequence"] = NullableInt.GetValue(iSequence);
			row["bStatic"] = bStatic;
			row["bSingleton"] = bSingleton;
		}
	}

	public class EventCommandHistory
	{
		[XmlAttribute("dbidEventSchedule")]
		public int idEventSchedule;

		[XmlElement("PackageLevel",IsNullable=false)]
		public PackageLevel idPackageLevel;

		[XmlElement("Store",IsNullable=true)]
		public int idStore;

		[XmlElement("Namespace",IsNullable=false)]
		public string sNamespace;

		[XmlElement("Object",IsNullable=false)]
		public string sObject;

		[XmlElement("Method",IsNullable=false)]
		public string sMethod;

		[XmlElement("DateStarted",IsNullable=false)]
		public DateTime dtStarted;

		[XmlElement("DateEnded",IsNullable=false)]
		public DateTime dtEnded;

		[XmlElement("CompletionStatus",IsNullable=false)]
		public int iCompletionStatus;
	}

	public class Package
	{
		public Package() {}

		[XmlAttribute("dbid")]
		public int idPackage;

		[XmlAttribute("dbidSourceLevel")]
		public int idSourceLevel;

		[XmlAttribute("dbidDestLevel")]
		public int idDestLevel;

		[XmlElement("dbidSendEvent", IsNullable=true)]
		public int idSendEvent;

		[XmlElement("dbidRecvEvent", IsNullable=true)]
		public int idRecvEvent;

		[XmlElement("Description", IsNullable=true)]
		public string sDesc;

		[XmlElement("TransportSubdir", IsNullable=false)]
		public string sTransportSubdir;

		[XmlElement(ElementName="DateCreated", IsNullable=false)]
		public DateTime dtCreated;
	}

	public class RunOnDayOfWeek
	{
		public RunOnDayOfWeek() {}

		[XmlAttribute(AttributeName="Su")]
		public bool bSunday;
		
		[XmlAttribute(AttributeName="M")]
		public bool bMonday;

		[XmlAttribute(AttributeName="Tu")]
		public bool bTuesday;

		[XmlAttribute(AttributeName="W")]
		public bool bWednesday;

		[XmlAttribute(AttributeName="Th")]
		public bool bThursday;

		[XmlAttribute(AttributeName="F")]
		public bool bFriday;

		[XmlAttribute(AttributeName="Sa")]
		public bool bSaturday;

		public RunOnDayOfWeek(byte byDOW)
		{
			SetByByteValue(byDOW);
		}

		public void SetByByteValue(byte byDOW)
		{
			bSunday = Convert.ToBoolean(byDOW & (1 << 0));
			bMonday = Convert.ToBoolean(byDOW & (1 << 1));
			bTuesday = Convert.ToBoolean(byDOW & (1 << 2));
			bWednesday = Convert.ToBoolean(byDOW & (1 << 3));
			bThursday = Convert.ToBoolean(byDOW & (1 << 4));
			bFriday = Convert.ToBoolean(byDOW & (1 << 5));
			bSaturday = Convert.ToBoolean(byDOW & (1 << 6));
		}

		public byte GetByteValue()
		{
			byte byDOW = 0;
			if(bSunday) byDOW |= (1 << 0);
			if(bMonday) byDOW |= (1 << 1);
			if(bTuesday) byDOW |= (1 << 2);
			if(bWednesday) byDOW |= (1 << 3);
			if(bThursday) byDOW |= (1 << 4);
			if(bFriday) byDOW |= (1 << 5);
			if(bSaturday) byDOW |= (1 << 6);
			return byDOW;
		}
	}

	public class EventPersister : CSWObjectBase
	{
		protected bool _bUpdating;
		protected Location _location;
		protected DataSetHolder _dsh;

		public EventPersister(CSWContext ctx, bool bUpdating) : this(ctx,bUpdating,Location.Staging) {}

		public EventPersister(CSWContext ctx, bool bUpdating, Location location) : base(ctx)
		{
			_bUpdating = bUpdating;
			_location = location;
			_dsh = GetDataSet(bUpdating,location);
		}

		public DataSetHolder DataSet { get { return _dsh; } }

		protected DataSetHolder GetDataSet(bool bUpdating, Location location)
		{
			Data.Connection conn = Context.Config.MainDatabase;
			DataSetHolder dsh = new DataSetHolder();

			dsh.AddTable(conn,"Schedule","select * from tblMMD_Schedule",bUpdating);
			dsh.AddTable(conn,"EventSchedule","select * from tblMMD_EventSchedule",bUpdating);
			dsh.AddTable(conn,"EventCommand","select * from tblMMD_EventCommand",bUpdating);
			dsh.FillTable(conn,"PackageLevel","select * from tblMMD_PackageLevel");

			//dsh.Relations.Add("S_ES",dsh.Tables["Schedule"].Columns["idSchedule"],dsh.Tables["EventSchedule"].Columns["idSchedule"]);
			dsh.Relations.Add("ES_EC",dsh.Tables["EventSchedule"].Columns["idEventSchedule"],dsh.Tables["EventCommand"].Columns["idEventSchedule"]);
			//dsh.Relations.Add("PL_EC",dsh.Tables["PackageLevel"].Columns["idPackageLevel"],dsh.Tables["EventCommand"].Columns["idPackageLevel"]);

			return dsh;
		}

		protected void UpdateDataSet()
		{
			_dsh.EnforceConstraints = false;
			_dsh.Update("Schedule","idSchedule");
			_dsh.Update("EventSchedule","idEventSchedule");
			_dsh.Update("EventCommand","idEventCommand");
			_dsh.EnforceConstraints = true;
		}

		protected void RemoveFromDataSet()
		{
			_dsh.EnforceConstraints = false;
			_dsh.Update("EventCommand","idEventCommand");
			_dsh.Update("EventSchedule","idEventSchedule");
			_dsh.Update("Schedule","idSchedule");
			_dsh.EnforceConstraints = true;
		}

		public void Update(EventSchedule es)
		{
			UpdateHelper(es);
			UpdateDataSet();
		}

		public void Update(EventSchedule[] ess)
		{
			for(int i=0;i<ess.Length;i++)
				UpdateHelper(ess[i]);
			UpdateDataSet();
		}

		protected void UpdateHelper(EventSchedule es)
		{
			// TODO: are there any rules about modifying an EventSchedule?
			if(false)
				throw new CSWException("Cannot update or modify an EventSchedule because ...");

			DataRow[] rows = _dsh.Tables["EventSchedule"].Select("idEventSchedule = " + es.idEventSchedule);
			DataRow row;
			bool bNew = rows.Length == 0;
			row = (bNew) ? _dsh.Tables["EventSchedule"].NewRow() : rows[0];
			es.Fill(row,_dsh,bNew,this.Context,_location);
		}

		public void RemoveEventSchedules(int[] idEventSchedules)
		{
			foreach(int i in idEventSchedules)
			{
				// TODO: are there any rules about modifying an EventSchedule?
				if(false)
					throw new CSWException("Cannot update or modify an EventSchedule because ...");

				DataRow r = _dsh.Tables["EventSchedule"].Rows.Find(idEventSchedules[i]);
				DataRow[] ecs = r.GetChildRows("ES_EC");
				foreach(DataRow row in ecs)
				{
					row.Delete();
				}
				r.Delete();
			}
			RemoveFromDataSet();
		}

		public EventSchedule GetEventScheduleById(int idEventSchedule)
		{
			DataRow[] rows = _dsh.Tables["EventSchedule"].Select("idEventSchedule = " + idEventSchedule);
			EventSchedule es = new EventSchedule(rows[0],_dsh,_location);
			return es;
		}

		public EventSchedule[] GetEventSchedules()
		{
			DataRow[] rows = _dsh.Tables["EventSchedule"].Select();
			EventSchedule[] ess = new EventSchedule[rows.Length];
			for(int i=0;i<rows.Length;i++)
				ess[i] = new EventSchedule(rows[i],_dsh,_location);
			return ess;
		}

	}

}
