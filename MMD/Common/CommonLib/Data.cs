using System;
using System.Web;
using System.Data;
using System.Data.SqlClient;
using System.Data.SqlTypes;

namespace MMD.Common.CommonLib.Data
{
	/// <summary>
	/// Represents a connection to the database.  Pass the connection string in at time of instantiation.
	/// </summary>
	public class Connection
	{
		private const int _TimeoutMin = 120;
		private const int _TimeoutMax = 86400;
		public int CommandTimeout 
		{ 
			set 
			{ 
				if (value < _TimeoutMin)
					_iCommandTimeout = _TimeoutMin;
				else if (value > _TimeoutMax)
					_iCommandTimeout = _TimeoutMax; 
				else
					_iCommandTimeout = value;
			}
			get
			{
				return _iCommandTimeout;
			}
		}

		private int _iCommandTimeout = 120;
		private SqlConnection _oConn;
		private string _ConnectionString;

		//public Data() { }
		public Connection(string strConnectionString) 
		{ 
			_ConnectionString = strConnectionString; 
			_oConn = new SqlConnection( _ConnectionString);
		}

		public string ConnectionString { 
			get { return _ConnectionString; } 
			//set { _ConnectionString = value; } 
		}

		public SqlConnection SqlConnection { get { return _oConn; } }
		public void Close() { _oConn.Close(); }

		/// <summary>
		/// Opens the SqlConnection member.
		/// </summary>
		private void Open() 
		{
			try	
				{_oConn.Open();	}
			catch(InvalidOperationException) {}
		}

		/// <summary>
		/// Execute an INSERT, UPDATE, or DELETE SQL statement.  Returns number of rows affected.
		/// </summary>
		/// <param name="strSql"></param>
		/// <returns></returns>
		public int execSQL(string strSql)
		{
			//SqlConnection oConn = new SqlConnection(_ConnectionString);
			SqlCommand oCmd = new SqlCommand( strSql, _oConn );
			oCmd.CommandType = CommandType.Text;
			oCmd.CommandTimeout = _iCommandTimeout;
			
			Open(); //_oConn.Open();
			int ret;
			try 
			{
				ret = oCmd.ExecuteNonQuery();
			}
			catch (Exception e)
			{
				string msg = e.Message;
				if (e.InnerException != null)
					msg += "\n" + e.InnerException.Message;
				string copy = msg;
				ret = 0;
			}
			return ret;
		}

		/// <summary>
		/// Execute a SQL statement.  Returns a DataReader object.
		/// </summary>
		/// <param name="strSql"></param>
		/// <returns></returns>
		public SqlDataReader execSQL_DR(string strSql)
		{
			//SqlConnection oConn = new SqlConnection(_ConnectionString);
			SqlCommand oCmd = new SqlCommand( strSql, _oConn );
			oCmd.CommandType = CommandType.Text;
			oCmd.CommandTimeout = _iCommandTimeout;
			
			Open(); //_oConn.Open();
			SqlDataReader dr = oCmd.ExecuteReader();
			return dr;
		}

		public System.Collections.ArrayList DiscoverTableKeys(string sTableName)
		{
			//SqlConnection oConn = new SqlConnection(_ConnectionString);
			SqlCommand oCmd = new SqlCommand( "select top 1 * from " + sTableName, _oConn );
			oCmd.CommandType = CommandType.Text;
			oCmd.CommandTimeout = _iCommandTimeout;
			
			Open(); //_oConn.Open();
			SqlDataReader dr = oCmd.ExecuteReader(System.Data.CommandBehavior.KeyInfo | System.Data.CommandBehavior.SchemaOnly);
			DataTable dt = dr.GetSchemaTable();
			System.Collections.ArrayList saKeys = new System.Collections.ArrayList();
			for (int i = 0; i < dt.Rows.Count; ++i)
			{
				// index 6 is the IsKey column.
				if ((bool)dt.Rows[i].ItemArray.GetValue(6))
					// index 0 is the columnName column.
					saKeys.Add(dt.Rows[i].ItemArray.GetValue(0));
			}
			dr.Close();
			return saKeys;
		}

		/// <summary>
		/// Execute a stored procedure.  Returns number of rows affected.
		/// </summary>
		/// <param name="strStoredProcedure"></param>
		/// <param name="cParams"></param>
		/// <returns></returns>
		public int execSP(string strStoredProcedure, Data.Parameters cParams )
		{
			//SqlConnection oConn = new SqlConnection(_ConnectionString);
			SqlCommand oCmd = new SqlCommand( strStoredProcedure, _oConn);
			oCmd.CommandType = CommandType.StoredProcedure;
			oCmd.CommandTimeout = _iCommandTimeout;
		
			if(cParams != null)
				foreach( SqlParameter oParam in cParams )
					oCmd.Parameters.Add( oParam );

			Open(); //_oConn.Open();
			return oCmd.ExecuteNonQuery();
		}
		
		/// <summary>
		/// Execute a stored procedure.  Returns a DataReader object.
		/// </summary>
		/// <param name="strStoredProcedure"></param>
		/// <param name="cParams"></param>
		/// <returns></returns>
		public SqlDataReader execSP_DR(string strStoredProcedure, Data.Parameters cParams )
		{
			//SqlConnection oConn = new SqlConnection(_ConnectionString);
			SqlCommand oCmd = new SqlCommand( strStoredProcedure, _oConn);
			oCmd.CommandType = CommandType.StoredProcedure;
			oCmd.CommandTimeout = _iCommandTimeout;
			
			if(cParams != null)
				foreach( SqlParameter oParam in cParams )
					oCmd.Parameters.Add( oParam );

			Open(); //_oConn.Open();
			SqlDataReader dr = oCmd.ExecuteReader();
			return dr;
		}

		/// <summary>
		/// Execute a stored procedure, Returns a Data.NamedFieldReader object.
		/// </summary>
		/// <param name="sStoredProcedure"></param>
		/// <param name="cParams"></param>
		/// <returns></returns>
		public Data.NamedFieldReader execSP_NFR(string sStoredProcedure, Data.Parameters cParams)
		{
			return new Data.NamedFieldReader( execSP_DR(sStoredProcedure, cParams) );
		}
		/// <summary>
		/// Execute a stored procedure and returns a collection of input/output parameters.
		/// </summary>
		/// <param name="strStoredProcedure"></param>
		/// <param name="cParams"></param>
		/// <returns></returns>
		public Data.Parameters execSP_OutputParams(string strStoredProcedure, Data.Parameters cParams )
		{
			//SqlConnection oConn = new SqlConnection(_ConnectionString);
			SqlCommand oCmd = new SqlCommand( strStoredProcedure, _oConn);
			oCmd.CommandType = CommandType.StoredProcedure;
			oCmd.CommandTimeout = _iCommandTimeout;
			
			if(cParams != null)
				foreach( SqlParameter oParam in cParams )
					oCmd.Parameters.Add( oParam );

			Open(); //_oConn.Open();
			int res = oCmd.ExecuteNonQuery();			

			Data.Parameters outParams = new Data.Parameters();
			foreach( SqlParameter oParam in oCmd.Parameters )
				outParams.Add(oParam);
		
			return outParams;
		}

	}

	/// <summary>
	/// Collection of stored procedure parameters
	/// </summary>
	public class Parameters : System.Collections.CollectionBase 
	{
		public Parameters() {}

		public void Add(SqlParameter aSqlParameter) { List.Add(aSqlParameter); }

		/// <summary>
		/// Add Booleans, Ints, and Dates
		/// </summary>
		/// <param name="ParameterName"></param>
		/// <param name="Type"></param>
		/// <param name="Direction"></param>
		/// <param name="Value"></param>
		public void Add(string ParameterName, DbType Type, ParameterDirection Direction, object Value)
		{
			SqlParameter sp = new SqlParameter();
			sp.ParameterName = ParameterName;
			sp.DbType = Type;
			sp.Direction = Direction;
			sp.Value = Value;
			List.Add(sp);
		}

		/// <summary>
		/// Add Variable and Fixed Length Strings
		/// </summary>
		/// <param name="ParameterName"></param>
		/// <param name="Type"></param>
		/// <param name="Direction"></param>
		/// <param name="Value"></param>
		/// <param name="Size"></param>
		public void Add(string ParameterName, DbType Type, ParameterDirection Direction, object Value, int Size)
		{
			SqlParameter sp = new SqlParameter();
			sp.ParameterName = ParameterName;
			sp.DbType = Type;
			sp.Direction = Direction;
			sp.Size = Size;
			sp.Value = Value;
			List.Add(sp);
		}

		/// <summary>
		/// Add Numerics that require Precision or Scale specified (Decimal, floating point, and Currency)
		/// </summary>
		/// <param name="ParameterName"></param>
		/// <param name="Type"></param>
		/// <param name="Direction"></param>
		/// <param name="Value"></param>
		/// <param name="Precision"></param>
		/// <param name="Scale"></param>
		public void Add(string ParameterName, DbType Type, ParameterDirection Direction, object Value, byte Precision, byte Scale)
		{
			SqlParameter sp = new SqlParameter();
			sp.ParameterName = ParameterName;
			sp.DbType = Type;
			sp.Direction = Direction;
			sp.Precision = Precision;
			sp.Scale = Scale;
			sp.Value = Value;
			List.Add(sp);
		}
		
		/// <summary>
		/// Add Output Parameters or Return Values
		/// </summary>
		/// <param name="ParameterName"></param>
		/// <param name="Type"></param>
		/// <param name="Direction"></param>
		public void Add(string ParameterName, DbType Type, ParameterDirection Direction)
		{
			SqlParameter sp = new SqlParameter();
			sp.ParameterName = ParameterName;
			sp.DbType = Type;
			sp.Direction = Direction;
			List.Add(sp);
		}
			
		public SqlParameter this[int iIndex]
		{
			get {	return (SqlParameter) List[iIndex]; 	}
		}

		public SqlParameter this[string sParameterName]
		{
			get 
			{
				foreach(SqlParameter sp in List)
				{
					if(sp.ParameterName == sParameterName)
						return sp;
				}
				throw new ArgumentOutOfRangeException("sParameterName");
			}
		}
	}

	/// <summary>
	/// Container for SqlDataReader allowing callee to request data from the DataReader by field names.
	/// </summary>
	public class NamedFieldReader
	{
		private SqlDataReader _dr;

		public NamedFieldReader(SqlDataReader dr)
		{
			_dr = dr;
		}

		public void Close() { _dr.Close(); }
		public bool Read() { return _dr.Read(); }
		public bool NextResult() { return _dr.NextResult(); }

		public SqlDataReader DataReader { get { return _dr; } }


		public int GetInt32(string sField) { return _dr.GetInt32( _dr.GetOrdinal( sField ) ); }
		public short GetInt16(string sField) { return _dr.GetInt16( _dr.GetOrdinal( sField ) ); }

		public string GetString(string sField) { return _dr.GetString( _dr.GetOrdinal( sField ) ); }
		public bool GetBoolean(string sField) { return _dr.GetBoolean( _dr.GetOrdinal( sField ) ); }
		public float GetFloat(string sField) { return _dr.GetFloat( _dr.GetOrdinal( sField ) ) ; }
		public double GetDouble(string sField) { return _dr.GetDouble( _dr.GetOrdinal( sField ) ) ; }
		public DateTime GetDateTime(string sField) { return _dr.GetDateTime( _dr.GetOrdinal(sField) ); }

		public object GetValue(string sField) { return _dr.GetValue( _dr.GetOrdinal(sField) ); }



		/// <summary>
		/// Checks if the field is NULL or not.
		/// </summary>
		/// <param name="sField"></param>
		/// <returns></returns>
		public bool IsDBNull(string sField)
		{
			return _dr.IsDBNull( _dr.GetOrdinal(sField) ); 
		}


		/// <summary>
		/// Returns int if non null, else returns default int value provided by caller.
		/// </summary>
		/// <param name="sField"></param>
		/// <param name="iDefault"></param>
		/// <returns></returns>
		public int SafeGetInt32(string sField, int iDefault) 
		{ 
			try { return GetInt32(sField); 	} 
			catch ( SqlNullValueException  ) { return iDefault; }
		}

		public short SafeGetInt16(string sField, short iDefault)
		{
			try { return GetInt16(sField); }
			catch( SqlNullValueException ) { return iDefault; } 
		}
		/// <summary>
		/// If field is non-null, return field's string value, else returns default value provided by caller.
		/// </summary>
		/// <param name="sField"></param>
		/// <param name="sDefault"></param>
		/// <returns></returns>
		public string SafeGetString(string sField, string sDefault)
		{
			try { return GetString(sField); } 
			catch( SqlNullValueException ) { return sDefault; }
		}

		/// <summary>
		/// If field is non-null, return field's string value, else return ""
		/// </summary>
		/// <param name="sField"></param>
		/// <returns></returns>
		public string SafeGetString(string sField)
		{
			return SafeGetString(sField, "");
		}

		/// <summary>
		/// If field is non-null, return field's DateTime value, else returns defaule value provided by caller.
		/// </summary>
		/// <param name="sField"></param>
		/// <param name="dtDefault"></param>
		/// <returns></returns>
		public DateTime SafeGetDateTime(string sField, DateTime dtDefault)
		{
			try { return GetDateTime(sField); }
			catch( SqlNullValueException ) { return dtDefault; }
		}

		public int GetInt(string sField)
		{
			int i = _dr.GetOrdinal( sField );
			try	{ return (int) Convert.ToInt32(_dr.GetBoolean(i)); }	
			catch(InvalidCastException) {}
			
			try { return (int) _dr.GetByte(i); } 
			catch(InvalidCastException) { }

			try { return (int) _dr.GetInt16(i); } 
			catch(InvalidCastException) {}

			return _dr.GetInt32(i);
		}

		public int SafeGetInt(string sField, int iDefault)
		{
			try
			{
				return GetInt(sField);
			}
			catch(SqlNullValueException)
			{
				return iDefault;
			}
		}
	
	}

}
