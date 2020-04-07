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
	/// Summary description for Task.
	/// </summary>
	public class Task
	{
		public Task()
		{
			//
			// TODO: Add constructor logic here
			//
		}
	}

	public class EventScheduleEdit : CommandBase
	{
		public EventScheduleEdit(CSWContext ctx) : base(ctx) {}

		int idEventSchedule;
		EventPersister ep;
		EventSchedule es;

		public override void InitParameters()
		{
			idEventSchedule = Context.Params.Int("idEventSchedule",0);
			ep = new EventPersister(Context,true);
			es = ep.GetEventScheduleById(idEventSchedule);
			es.sDesc = Context.Params.String("sDesc","");
			es.eType = (EventScheduleType)Context.Params.Int("byType",0);
			es.idSchedule = NullableInt.CreateInstance(Context.Params.Int("idSchedule",0));
			es.iPeriodMinutes = NullableInt.CreateInstance(Context.Params.Int("iPeriodMinutes",0));
			es.dtTimeOfDay = NullableDateTime.CreateInstance(Convert.ToDateTime("1/1/1900 " + Context.Params.String("sTimeOfDay","12:00:00 AM")));
			es.dtRunOnce = NullableDateTime.CreateInstance(Convert.ToDateTime(Context.Params.String("sRunOnceDateTime","1/1/1900 12:00:00 AM")));
			es.bRunNow = (Context.Params.Int("bRunNow",0) == 0);
			es.iPriority = Context.Params.Int("iPriority",0);
			es.eStatus = (EventRunStatus)Context.Params.Int("byStatus",0);
			es.dtLastRun = NullableDateTime.CreateInstance(Convert.ToDateTime(Context.Params.String("sLastRun","1/1/1900 12:00:00 AM")));

			byte bfDaysOfWeek = 0;
			for(int i=0;i<7;i++)
				if(0 != Context.Params.Int("dow_" + i,0))
					bfDaysOfWeek |= Convert.ToByte(1 << i);
			es.bfDaysOfWeek = bfDaysOfWeek;
			es.RunOnDayOfWeek.SetByByteValue(bfDaysOfWeek);
		}

		public override void Execute()
		{
			ep.Update(es);
			Context.SetStatusMessage(StatusMsg.EventScheduleEdited);
		}
	}

	public class EventScheduleRunNow : CommandBase
	{
		public EventScheduleRunNow(CSWContext ctx) : base(ctx) {}

		int idEventSchedule;
		EventPersister ep;
		EventSchedule es;

		public override void InitParameters()
		{
			idEventSchedule = Context.Params.Int("idEventSchedule",0);
			ep = new EventPersister(Context,true);
			es = ep.GetEventScheduleById(idEventSchedule);
			es.bRunNow = true;
		}

		public override void Execute()
		{
			ep.Update(es);
			Context.SetStatusMessage(StatusMsg.EventScheduleEdited);
		}
	}
}
