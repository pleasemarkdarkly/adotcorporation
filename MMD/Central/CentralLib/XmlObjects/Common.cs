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
	public enum Location
	{
		Staging,
		Deploy,
	}

	public enum MMDPostedFileType
	{
		IdleLoopElement = 1,
		RecListAttribute = 2,
		FeatureArticle = 3
	}

	public enum MMDEventScheduleType
	{
		PeriodicInMinutes = 0,
		Daily = 1,
		RunOnce = 2
	}

	public enum MMDEventScheduleStatus
	{
		Disabled = 0,
		Ready = 1,
		Running = 2
	}

	public enum MMDDaysOfTheWeek
	{
		Sunday = 1,
		Monday = 2,
		Tuesday = 4,
		Wednesday = 8, 
		Thursday = 16,
		Friday = 32,
		Saturday = 64
	}

	public enum SortOrder
	{
		ASC = 0,
		DESC = 1
	}

	public enum FrameType
	{
		Image = 1,
		Video = 2,
		ScreenML = 3,
		RecList = 4,
		Promotion = 5,
		Product = 6
	}

	public enum FileMediaType
	{
		Image = 1,
		Video = 2,
		ScreenML = 3
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd",IsNullable=true)]
	public class NullableDateTime
	{
		public NullableDateTime() {}
		public NullableDateTime(DateTime dt) { value = dt; }

		[XmlAttribute(AttributeName="value")]
		public DateTime value;

		static public NullableDateTime Now	{ get {	return new NullableDateTime(DateTime.Now);	} }

		static public object GetValue(NullableDateTime ndt)
		{
			if(ndt == null)
				return DBNull.Value; //null;
			else
				return ndt.value;
		}

		static public object GetValue(NullableDateTime ndt,object ndtDefault)
		{
			if(ndt == null)
				return ndtDefault;
			else
				return ndt.value;
		}

		static public NullableDateTime CreateInstance(object o)
		{
			if(o == null) return null;
			if(o == DBNull.Value) return null;

			if(o is SqlDateTime)
			{
				SqlDateTime dt = (SqlDateTime)o;
				if(dt.IsNull) return null;
				
				return new NullableDateTime(dt.Value);
			}

			if(o is DateTime)
				return new NullableDateTime((DateTime)o);

			return new NullableDateTime(Convert.ToDateTime(o));
		}
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd",IsNullable=true)]
	public class NullableInt
	{
		public NullableInt() {}
		public NullableInt(int i) { value = i; }

		[XmlAttribute(AttributeName="value")]
		public int value;

		static public NullableInt CreateInstance(object o)
		{
			if(o == null) return null;
			if(o == DBNull.Value) return null;

			if(o is System.Int32)
				return new NullableInt((int)o);

			return new NullableInt(Convert.ToInt32(o));
		}

		static public object GetValue(NullableInt o)
		{
			if(o == null) return DBNull.Value; //null;
			else return (object)o.value;
		}
	}

	public abstract class PersisterBase : CSWObjectBase
	{
		protected bool _bUpdating;
		protected Location _location;
		protected DataSetHolder _dsh;

		public DataSetHolder DataSet { get { return _dsh; } }

		public PersisterBase(CSWContext ctx, bool bUpdating) : base(ctx)
		{
			_bUpdating = bUpdating;
			_dsh = GetDataSet();
		}

		public PersisterBase(CSWContext ctx, bool bUpdating, Location location) : base(ctx)
		{
			_bUpdating = bUpdating;
			_location = location;
			_dsh = GetDataSet();
		}

		protected abstract DataSetHolder GetDataSet();
		protected abstract void UpdateDataSet();

	}



}
