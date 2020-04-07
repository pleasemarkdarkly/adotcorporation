using System;
using System.Xml;
using MMD.Common.CommonLib.Data;
using System.Data.SqlClient;

namespace MMD.Common.CommonLib
{
	public struct tPair
	{
		public int Left;
		public int Right;
	}
	
	public enum EventType 
	{
		Periodic=0,
		Daily=1,
		RunOnce=2
	}
	
	public enum TaskStatus
	{
		Inactive=0,
		ReadyToRun=1,
		Running=2
	}

	public enum EventCommand
	{
		TransferLog,
		TransferUpdate,
		TransferConfig
	}

	public struct tEventCommand
	{
		public int idEventSchedule;
		public int idAction;
		public int iSequence;
		public EventCommand eCommand;
		public string sArgs;
	}

	public struct tEventSchedule
	{
		public int idEventSchedule;
		public EventType byType;
		public int idSchedule;
		public int iPeriodMinutes;
		public DateTime dtTimeOfDay;
		public char bfDaysOfWeek;
		public int iPriority;
		public char byStatus;
	}

	public class ScheduleTask
	{
		//private string _Conn;

		//private TableTransfer m_TableTransfer;
		//private LogExport m_StoreLogExport;
		private Connection _Conn;

		public ScheduleTask()
		{
			//XmlDocument xmld = new XmlDocument();
			//xmld.Load("http://localhost/mmd/xml/Config.xml");
			//_Conn = xmld.SelectSingleNode("//database[@id='store']").InnerText;
			//m_StoreLogExport = new LogExport();
		}

		// (specific to store in this context, will do central separately)
		public int PerformTaskAction(tEventCommand ta)
		{
			switch (ta.eCommand)
			{
					// reworking task actions to be slightly higher level, using context to fill in
					// the exact details of what to do, pushing more config into the database.
				case EventCommand.TransferLog:
				{
					//m_TableTransfer.TransferTables(ta.idAction);
					//m_StoreLogExport.BuildLogPackage();
					break;
				}
					/*case EventCommand.SendLogs:
					{
						m_StoreLogExport.TransferAllPackagesToCentral();
						break;
					}
					*/
				case EventCommand.TransferUpdate:
				{
					// TODO
					break;
				}
					/*
					case EventCommand.ImportUpdate:
					{
						// TODO
						break;
					}
					*/
				case EventCommand.TransferConfig:
				{
					// TODO
					break;
				}
			}
			return 0;
		}

		// given a datareader pointing at a tblMMD_EventSchedule row, return a struct containing that row.
		private tEventSchedule LoadEventSchedule(SqlDataReader dr)
		{
			NamedFieldReader nfr = new NamedFieldReader(dr);
			tEventSchedule task;
			task.bfDaysOfWeek = (char) nfr.SafeGetInt("bfDaysOfWeek",0);
			task.byStatus = (char) nfr.SafeGetInt("byStatus",0);
			task.byType = (EventType) nfr.SafeGetInt("byType",0);
			task.dtTimeOfDay = nfr.SafeGetDateTime("dtTimeOfDay",DateTime.Parse("12:00am"));
			task.idSchedule = nfr.SafeGetInt("idSchedule",0);
			task.idEventSchedule = nfr.SafeGetInt("idEventSchedule",0);
			task.iPeriodMinutes = nfr.SafeGetInt("iPeriodMinutes",0);
			task.iPriority = nfr.SafeGetInt("iPriority",0);
			return task;
		}

		private tEventCommand LoadEventCommand(SqlDataReader dr)
		{
			NamedFieldReader nfr = new NamedFieldReader(dr);
			tEventCommand action;
			action.idAction = nfr.GetInt("idAction");
			string sCommand = nfr.GetString("sCommand");
			
			try 
			{
				action.eCommand = (EventCommand) Enum.Parse(typeof(EventCommand) ,nfr.GetString("sCommand"),true);
			}
			catch (ArgumentException) 
			{
				throw new ScheduleTaskException (String.Format("Action {0} has invalid Command {1}",action.idAction,sCommand));
			}
			
			action.idEventSchedule = nfr.GetInt("idEventSchedule");
			action.iSequence = nfr.GetInt("iSequence");
			action.sArgs = nfr.GetString("sArgs");
			return action;
		}

		public object NextRunOnceTask()
		{
			string sql = String.Format("select top 1 * from tblMMD_EventSchedule where byType = {0} and byStatus = {1} order by iPriority",
				EventType.RunOnce.ToString("d"),TaskStatus.ReadyToRun.ToString("d"));
			SqlDataReader dr = _Conn.execSQL_DR(sql);
			if (!dr.Read())
			{
				dr.Close();
				return null;
			}
			tEventSchedule task = LoadEventSchedule(dr);
			dr.Close();
			return task;
		}

		public object NextPeriodicTask(DateTime dtEventStarted)
		{
			
			string sql = String.Format("select * from tblMMD_EventSchedule where byType = {0} and DATEADD( mi, iPeriodMinutes, dtLastRun) < '{1}' and byStatus = 1 ORDER BY iPriority",
				EventType.Periodic.ToString("d"), dtEventStarted.ToString("G"));
			
			SqlDataReader dr = _Conn.execSQL_DR(sql);
			if (!dr.Read())
			{
				dr.Close();
				return null;
			}
			NamedFieldReader nfr = new NamedFieldReader(dr);

			// read out the candidate tasks and their associated schedule ids so we can then run checks against the schedule to see if any are curently active.
			System.Collections.ArrayList taCandidates = new System.Collections.ArrayList();
			do 
			{
				tPair p;
				p.Left = nfr.GetInt("idEventSchedule");
				p.Right = nfr.GetInt("idSchedule");
				taCandidates.Add(p);
			} while (dr.Read());
			nfr.Close();

			int nActiveIndex = -1;
			for (int i = 0; i < taCandidates.Count; ++i)
			{
				tPair p = (tPair) taCandidates[i];
				int idEventSchedule = p.Left;
				int idSchedule = p.Right;
				sql = "exec spr_MMDScheduleCurrentlyActive " + idSchedule.ToString();
				nfr = new NamedFieldReader (_Conn.execSQL_DR(sql));
				if (!nfr.Read())
					throw new Exception("failed to run sched currently active");
				bool bActive = (nfr.GetInt("bScheduleActive") != 0);
				if (bActive)
				{
					nfr.Close();
					nActiveIndex = i;
					break;
				}
				nfr.Close();
			}

			// exit if no tasks are currently active.
			if (nActiveIndex == -1)
				return null;

			// the coveted active task.
			int idActiveEvent = ((tPair)taCandidates[nActiveIndex]).Left;
			sql = "select * from tblMMD_EventSchedule where idEventSchedule = " + idActiveEvent.ToString();
			dr = _Conn.execSQL_DR(sql);
			if (!dr.Read())
				throw new ScheduleTaskException("couldn't load task with id " + idActiveEvent.ToString());
			
			tEventSchedule task = LoadEventSchedule(dr);
			dr.Close();
			return task;
		}

		/// <summary>
		/// Given a DATETIME column name, return a sql fragment that converts it into just the time part.  Facilitates TimeOfDay comparisons.
		/// </summary>
		private string EmitTimePartSql(string sDateTimeColumnName)
		{
			return "CAST(CAST(DATEPART(hh," + sDateTimeColumnName + ") AS VARCHAR) + ':' + CAST(DATEPART(mi," + sDateTimeColumnName + ") AS VARCHAR) + ':' + CAST(DATEPART(ss," + sDateTimeColumnName + ") AS VARCHAR) AS DATETIME)";
		}

		public object NextDailyTask(DateTime dtEventStarted)
		{
			// set up a datetime that will tell us whether tasks have run today.
			string sThisMorningDateTime  = dtEventStarted.ToString("d") + " 12:00:00 AM";
			DateTime dtThisMorning = DateTime.Parse(sThisMorningDateTime);
			// set up a flags val to test if tasks should run today.
			byte bfToday;
			
			int iToday = (int) dtEventStarted.DayOfWeek; // returns 0 for sunday, 1 for monday, etc.
			bfToday = (byte) (1 << (iToday));

			// need to attempt to check the time part of the dtTimeOfDay without picking up unwanted date connotation.
			string sTimePart = EmitTimePartSql("dtTimeOfDay");
		
			string sql = String.Format("select top 1 * from tblMMD_EventSchedule where byType = {0} and dtLastRun < '{1}' and (bfDaysOfWeek & {2}) > 0 and {3} < CAST('{4}' as DateTime) order by iPriority",
				EventType.Daily.ToString("d"),dtThisMorning.ToString("G"),bfToday,sTimePart,dtEventStarted.ToString("T"));
		

			SqlDataReader dr = _Conn.execSQL_DR(sql);
			if (!dr.Read())
			{
				dr.Close();
				// no tasks made the cut.
				return null;
			}

			tEventSchedule task = LoadEventSchedule(dr);
			dr.Close();
			return task;
		}

		// check if there are any tasks that need to be run, and the highest priority one.  return true if a task was run.
		private bool RunNextTask()
		{
			// lock down the official run time for internal consistency
			DateTime dtEventStarted = General.DBNow();

			object oBestTask = null;
			// lower numbers imply higher priority, so start with a very low priority endpoint to work back from.
			int iHighestPriority = 10000;

			// look up the highest priority run-once task
			object oRunOnceTask = NextRunOnceTask();

			// look up the highest priority schedule-constrained periodic task
			object oPeriodicTask = NextPeriodicTask(dtEventStarted);

			// look up the highest priority daily task
			object oDailyTask = NextDailyTask(dtEventStarted);

			// sort out which object represents the highest priority task, if any.
			
			//  start with the run-once task
			oBestTask = oRunOnceTask;
			//  update the highest priority
			if (oBestTask != null)
				iHighestPriority = ((tEventSchedule)oBestTask).iPriority;
			//  check the periodic task for superiority
			if (oPeriodicTask != null && ((tEventSchedule)oPeriodicTask).iPriority < iHighestPriority)
				oBestTask = oPeriodicTask;
			//  update the highest priority
			if (oBestTask != null)
				iHighestPriority = ((tEventSchedule)oBestTask).iPriority;
			//  check the daily task
			if (oDailyTask != null && ((tEventSchedule)oDailyTask).iPriority < iHighestPriority)
				oBestTask = oDailyTask;

			// cut out if there weren't any tasks to do.
			if (oBestTask == null)
				return false;

			// the best task is our final decision at this point, and nonnull to boot.
			tEventSchedule activeEvent = (tEventSchedule) oBestTask;
			
			RunEventCommands(activeEvent,dtEventStarted);

			RunPackages(activeEvent);
			
			// update the task's last-run field
			string sql = "update tblMMD_EventSchedule set dtLastRun = '" + dtEventStarted.ToString("G") + "' where idEventSchedule = " + activeEvent.idEventSchedule.ToString();
			int iRowsAffected = _Conn.execSQL(sql);
			if (iRowsAffected != 1)
				throw new ScheduleTaskException("Failed to update ScheduleTask dtLastRun, expected 1 rows affected, got " + iRowsAffected.ToString());

			if (activeEvent.byType == EventType.RunOnce)
			{
				// mark the runonce as inactive.
				sql = "update tblMMD_EventSchedule set byStatus = 0 where idEventSchedule = " + activeEvent.idEventSchedule;
				iRowsAffected = _Conn.execSQL(sql);
				if (iRowsAffected != 1)
					throw new ScheduleTaskException(String.Format("Failed to mark ScheduleTask {0} inactive after runonce, expected 1 rows affected, got " + iRowsAffected.ToString(),activeEvent.idEventSchedule));
			}
			return true;
		}

		private void RunPackages(tEventSchedule activeEvent)
		{
			int idPackageLevel = (int) Config.GetInstance().TransferLevel;
			// first do package receiving.
			string sql = "select idPackage from tblMMD_Package where idRecvEvent = " + activeEvent.idEventSchedule;
			sql += " AND idDestLevel = " + idPackageLevel;
			NamedFieldReader nfr = new NamedFieldReader(_Conn.execSQL_DR(sql));
			while (nfr.Read())
			{
				Package package = new Package();
				int idPackage = nfr.SafeGetInt("idPackage",-1);
				if (idPackage != -1)
					package.Transfer(idPackage);
			}
			nfr.Close();

			// and then package sending
			sql = "select idPackage from tblMMD_Package where idSendEvent = " + activeEvent.idEventSchedule;
			sql += " AND idSourceLevel = " + idPackageLevel;
			nfr = new NamedFieldReader(_Conn.execSQL_DR(sql));
			while (nfr.Read())
			{
				Package package = new Package();
				int idPackage = nfr.SafeGetInt("idPackage",-1);
				if (idPackage != -1)
					package.Transfer(idPackage);
			}
			nfr.Close();
		}

		private void RunEventCommands(tEventSchedule activeEvent, DateTime dtEventStarted)
		{
			int idActiveEvent = activeEvent.idEventSchedule;

			// look up associated actions
			string sql = "select * from tblMMD_EventCommand where idEventSchedule = " + idActiveEvent + " ORDER BY iSequence";
			SqlDataReader dr = _Conn.execSQL_DR(sql);
			System.Collections.ArrayList aActions = new System.Collections.ArrayList();
			while (dr.Read())
			{
				tEventCommand ta = LoadEventCommand(dr);
				aActions.Add(ta);
			}
			dr.Close();

			// update schedule task to reflect that this task is running.
			sql = "update tblMMD_EventSchedule set byStatus = 2 where idEventSchedule = " + idActiveEvent ;
			int iRowsAffected = _Conn.execSQL(sql);
			if (iRowsAffected != 1)
				throw new ScheduleTaskException("Unable to mark ScheduleTask " + idActiveEvent + " as Running");

			iRowsAffected = 0;
			// perform each action in sequence.
			for (int i = 0; i < aActions.Count; ++i)
			{
				tEventCommand ta = (tEventCommand)aActions[i];
				DateTime dtActionStarted = DateTime.Now;
				
				// perform the action
				int iActionResult = PerformTaskAction(ta);

				// record action history.
				DateTime dtActionDone = DateTime.Now;
				sql = "insert into tblMMD_TaskActionHistory values (" + ta.idEventSchedule.ToString() + "," + ta.idAction.ToString() + "," + ta.iSequence.ToString() + ",'" + ta.eCommand + "','" + ta.sArgs + "','" + dtActionStarted.ToString("G") + "','" + dtActionDone.ToString("G") + "'," + iActionResult + ")";
				iRowsAffected = _Conn.execSQL(sql);
				if (iRowsAffected != 1)
					throw new ScheduleTaskException("Failed to insert into TaskActionHistory, expected 1 rows affected, got " + iRowsAffected.ToString());
			}
			DateTime dtTaskDone = DateTime.Now;

			// update schedule task to reflect that this task is no longer running.
			int iStatus;
			if (activeEvent.byType == EventType.RunOnce)
				iStatus = 0;
			else
				iStatus = 1;
			sql = "update tblMMD_EventSchedule set byStatus = " + iStatus + " where idEventSchedule = " + idActiveEvent;
			iRowsAffected = _Conn.execSQL(sql);
			if (iRowsAffected != 1)
				throw new ScheduleTaskException("Unable to mark ScheduleTask " + idActiveEvent + " as Running");

			Config cfg = Config.GetInstance();

			// record task history
			sql = "insert into tblMMD_EventCommandHistory";
			sql += " (idEventSchedule, idPackageLevel, idStore, dtStarted, dtEnded, iCompletionStatus, sDesc) values (";
			// idEventSchedule
			sql += idActiveEvent.ToString() + ",";
			// idTransferLevel
			sql += cfg.TransferLevel.ToString("d") + ",";
			// idStore
			if (cfg.TransferLevel == Package.Level.Store)
				sql += cfg.StoreNumber;
			else
				sql += "Null";
			sql += ",";
			// dtStarted
			sql += "'" + dtEventStarted.ToString("G") + "',";
			// dtEnded, completion status, description.
			sql += "'" + dtTaskDone.ToString("G") + "',0,'')";

			iRowsAffected = _Conn.execSQL(sql);
			if (iRowsAffected != 1)
				throw new ScheduleTaskException("Failed to insert into EventCommandHistory, expected 1 rows affected, got " + iRowsAffected.ToString());


		}

		public void RunAllTasks()
		{
			// store this for convenience during the run.
			//_Conn = new Connection(_Conn);
			_Conn = General.LocalConnection();
			bool bMoreTasks = true;
			while (bMoreTasks)
				bMoreTasks = RunNextTask();
			// clear out the data connection so we're not hogging database resources.
			_Conn.Close();
			_Conn = null;
		}
	}
	/// <summary>The exception that is thrown when a task scheduling error occurs.</summary>
	public class ScheduleTaskException: Exception
	{
		/// <summary>Constructs a new ScheduleTaskException.</summary>
		/// <param name="Description">Specifies the error message.<param>
		public ScheduleTaskException(string Description) : base(Description) {}
		/// <summary>Returns a string representation of this object.</summary>	
		/// <returns>A string representation of this ScheduleTaskException.</returns>
		public override string ToString() 
		{
			return "ScheduleTaskException: " + Message + " " + StackTrace;
		}
	}
}
