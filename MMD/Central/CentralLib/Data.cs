using System;
using System.Collections;
using System.Data;
using System.Data.SqlClient;
using System.Data.SqlTypes;

namespace MMD.Central.CentralLib.Data
{
	public class DataAdapterHolder
	{
		public DataAdapterHolder() : this(null,null) {}
		public DataAdapterHolder(SqlCommandBuilder cb) : this(null,cb) {}
		public DataAdapterHolder(SqlDataAdapter da) : this (da,null) {}
		public DataAdapterHolder(SqlDataAdapter da, SqlCommandBuilder cb) 
		{
			DataAdapter = da;
			CommandBuilder = cb;
		}

		public SqlDataAdapter DataAdapter;
		public SqlCommandBuilder CommandBuilder;
	}


	public class DataSetHolder : DataSet
	{
		public DataSetHolder() : base()
		{ 
			//_ds = new DataSet();
			_hashAdapters = new Hashtable();
		}

		//private DataSet _ds;
		private Hashtable _hashAdapters;

		//public DataSet DataSet { get { return _ds; } set { _ds = value; } }
		//public DataSet ds { get { return _ds; } set { _ds = value; } }

		public DataAdapterHolder this [string sTableName]
		{
			get
			{
				return (DataAdapterHolder)_hashAdapters[sTableName];
			}

			set 
			{
				_hashAdapters[sTableName] = value;
			}
		}

		public void AddTable(Data.Connection conn, string sTableName, string sSelectSql, bool bBuildCommands)
		{
			if(bBuildCommands)
				BuildAdapter(conn,sTableName,sSelectSql);
			else
				FillTable(conn,sTableName,sSelectSql);
		}

		public void BuildAdapter(Data.Connection conn, string sTableName, string sSelectSql)
		{
			SqlCommandBuilder cb;
			SqlDataAdapter da = conn.BuildAdapter(sSelectSql, this, sTableName, out cb);
			this[sTableName] = new DataAdapterHolder(da, cb);


		}

		public void FillTable(Data.Connection conn, string sTableName, string sSelectSql)
		{
			this[sTableName] = new DataAdapterHolder( conn.FillTable(sSelectSql, this, sTableName) );
		}

		public void BuildCommands(string sTableName)
		{
			this[sTableName].CommandBuilder = new SqlCommandBuilder(this[sTableName].DataAdapter);
		}

		public ForeignKeyConstraint AddForeignKeyConstraint(string sConstraint, string sParentTable, string sParentKey, string sChildTable, string sChildKey)
		{
			ForeignKeyConstraint fkc = new ForeignKeyConstraint(sConstraint,
																Tables[sParentTable].Columns[sParentKey],
																Tables[sChildTable].Columns[sChildKey]);
			fkc.DeleteRule = Rule.Cascade;
			fkc.UpdateRule = Rule.Cascade;
			Tables[sParentTable].Constraints.Add(fkc);
			EnforceConstraints = true;
			return fkc;
		}

		public void SetPrimaryKey(string sTableName, string sPrimaryKeyColumn)
		{
			DataColumn[] key = new DataColumn[1];
			key[0] = Tables[sTableName].Columns[sPrimaryKeyColumn];
			Tables[sTableName].PrimaryKey = key;
		}

		private string _sIdentityColumn;

		public void Update(string sTableName) { Update(sTableName,null); }

		public void Update(string sTableName, string sIdentityColumn)
		{
			_sIdentityColumn = sIdentityColumn;
			SqlDataAdapter da = this[sTableName].DataAdapter;

			if(sIdentityColumn != null) 
				da.RowUpdated += new SqlRowUpdatedEventHandler( this.OnRowUpdated );
			
			da.Update(this,sTableName);
			
			if(sIdentityColumn != null) 
				da.RowUpdated -= new SqlRowUpdatedEventHandler ( this.OnRowUpdated );
		}

		private void OnRowUpdated(object sender, SqlRowUpdatedEventArgs args)
		{
			if (args.StatementType == StatementType.Insert)
			{
				int newID = 0;
				// Include a variable and a command to retrieve the identity value from the Access database.
				SqlCommand cmdIdentity = new SqlCommand("SELECT @@IDENTITY", 
					((SqlDataAdapter)sender).SelectCommand.Connection );

				newID = Convert.ToInt32(cmdIdentity.ExecuteScalar());

				//this.EnforceConstraints = false;
				// Retrieve the identity value and store it in the CategoryID column.
				args.Row.Table.Columns[_sIdentityColumn].ReadOnly = false;
				args.Row[_sIdentityColumn] = newID;
				args.Row.Table.Columns[_sIdentityColumn].ReadOnly = true;

				//we need to update the foreign keys of all related objects
				foreach(DataRelation relChild in args.Row.Table.ChildRelations)
				{
					//get child rows for that relation
					DataRow[] rowsChild = args.Row.GetChildRows(relChild);
					foreach(DataRow row in rowsChild)
					{
						//for each row set foreign key to new ID
						for(int i=0;i<relChild.ParentColumns.Length;i++)
						{
							if(relChild.ParentColumns[i].ColumnName == _sIdentityColumn)
								row[relChild.ChildColumns[i]] = newID;
						}
					}
				}

				//this.EnforceConstraints = true;
				//need to verify that children of object have the right parent id
				
			}
		}

	}


	/// <summary>
	/// Represents a connection to the database.  Pass the connection string in at time of instantiation.
	/// </summary>
	public class Connection
	{
		private const int c_iCommandTimeout = 120;
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
			SqlCommand oCmd = new SqlCommand( strSql, _oConn );
			oCmd.CommandTimeout = c_iCommandTimeout;
			
			Open(); //_oConn.Open();
			return oCmd.ExecuteNonQuery();
		}

		public object execSQL_Scalar(string sSql)
		{
			SqlCommand oCmd = new SqlCommand( sSql, _oConn);
			oCmd.CommandTimeout = c_iCommandTimeout;

			Open();
			return oCmd.ExecuteScalar();
		}

		/// <summary>
		/// Execute a SQL statement.  Returns a DataReader object.
		/// </summary>
		/// <param name="strSql"></param>
		/// <returns></returns>
		public SqlDataReader execSQL_DR(string strSql)
		{
			SqlCommand oCmd = new SqlCommand( strSql, _oConn );
			oCmd.CommandTimeout = c_iCommandTimeout;
			
			Open(); //_oConn.Open();
			SqlDataReader dr = oCmd.ExecuteReader();
			return dr;
		}

		public Data.NamedFieldReader execSQL_NFR(string sSQL)
		{
			return new Data.NamedFieldReader( execSQL_DR(sSQL) );
		}

		/// <summary>
		/// Execute a stored procedure.  Returns number of rows affected.
		/// </summary>
		/// <param name="strStoredProcedure"></param>
		/// <param name="cParams"></param>
		/// <returns></returns>
		public int execSP(string strStoredProcedure, Data.Parameters cParams )
		{
			SqlCommand oCmd = GetStoredProcCommand(strStoredProcedure, cParams);
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
			SqlCommand oCmd = GetStoredProcCommand( strStoredProcedure, cParams );
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
			SqlCommand oCmd = GetStoredProcCommand( strStoredProcedure, cParams );

			Open(); //_oConn.Open();
			int res = oCmd.ExecuteNonQuery();			

			Data.Parameters outParams = new Data.Parameters();
			foreach( SqlParameter oParam in oCmd.Parameters )
				outParams.Add(oParam);
		
			return outParams;
		}

		/// <summary>
		/// Execute stored procedure and return resultant recordset to a DataTable with specified name into specified DataSet.
		/// </summary>
		/// <param name="strStoredProcedure"></param>
		/// <param name="cParams"></param>
		/// <param name="sDataTableName"></param>
		/// <param name="oDS"></param>
		/// <returns></returns>
		public DataSet execSP_DS(string strStoredProcedure, Data.Parameters cParams, string sDataTableName, DataSet oDS)
		{
			return AddCommandToDataSet( GetStoredProcCommand(strStoredProcedure, cParams ) , oDS, sDataTableName );
		}
	
		/// <summary>
		/// Execute stored proc and return recordset to a DataTable with the specified name into a new DataSet object.
		/// </summary>
		/// <param name="strStoredProcedure"></param>
		/// <param name="cParams"></param>
		/// <param name="sDataTableName"></param>
		/// <returns></returns>
		public DataSet execSP_DS(string strStoredProcedure, Data.Parameters cParams, string sDataTableName)
		{
			return execSP_DS( strStoredProcedure, cParams, sDataTableName, null );
		}

		/// <summary>
		/// Execute SQL select statement and return recordset to a DataTable with the specified name into the specified DataSet object.
		/// </summary>
		/// <param name="sSelectStatement"></param>
		/// <param name="sDataTableName"></param>
		/// <param name="oDS"></param>
		/// <returns></returns>
		public DataSet execSQL_DS(string sSelectStatement, string sDataTableName, DataSet oDS)
		{
			SqlCommand oCmd = new SqlCommand(sSelectStatement, _oConn);
			oCmd.CommandTimeout = c_iCommandTimeout;
			return AddCommandToDataSet( oCmd, oDS, sDataTableName );
		}

		/// <summary>
		/// Execute SQL select statement and return recordset to a DataTable with the specified name into a new DataSet.
		/// </summary>
		/// <param name="sSelectStatement"></param>
		/// <param name="sDataTableName"></param>
		/// <returns></returns>
		public DataSet execSQL_DS(string sSelectStatement, string sDataTableName)
		{
			return execSQL_DS( sSelectStatement, sDataTableName, null );
		}

		public DataSet execSQL_DS(string sSelectStatement)
		{
			return execSQL_DS(sSelectStatement, "DS", null);
		}

		public int DoCountQuery(string sSQL)
		{
			DataSet ds = execSQL_DS("select count(*) from " + sSQL,"CountQuery");
			return int.Parse(ds.Tables[0].Rows[0][0].ToString());
		}	

		/// <summary>
		/// Fills a DataTable of the specified name in the specified DataSet from the given SqlCommand.
		/// </summary>
		/// <param name="oCmd"></param>
		/// <param name="oDS"></param>
		/// <param name="sDataTableName"></param>
		/// <returns></returns>
		private DataSet AddCommandToDataSet(SqlCommand oCmd, DataSet oDS, string sDataTableName)
		{
			SqlDataAdapter oDA = new SqlDataAdapter();
			oDA.SelectCommand = oCmd;
			Open();
			if(oDS == null)
				oDS = new DataSet();
			oDA.Fill( oDS, sDataTableName );
			Close();
			return oDS;
		}

		/// <summary>
		/// Creates a SqlCommand to execute a stored procedure with the given parameters.
		/// </summary>
		/// <param name="sStoredProcedure"></param>
		/// <param name="cParams"></param>
		/// <returns></returns>
		private SqlCommand GetStoredProcCommand(string sStoredProcedure, Data.Parameters cParams)
		{
			SqlCommand oCmd = new SqlCommand( sStoredProcedure, _oConn);
			oCmd.CommandType = CommandType.StoredProcedure;
			oCmd.CommandTimeout = c_iCommandTimeout;

			if(cParams != null)
				foreach( SqlParameter oParam in cParams )
					oCmd.Parameters.Add( oParam );

			return oCmd;
		}

		public SqlDataAdapter BuildAdapter(string sSelectSql, DataSet ds, string sTableName, out SqlCommandBuilder cb)
		{
			SqlDataAdapter da = new SqlDataAdapter();
			da.SelectCommand = new SqlCommand(sSelectSql, _oConn);
			cb = new SqlCommandBuilder(da);
			//da.FillSchema(ds,SchemaType.Source);
			//ds.Tables["Table"].TableName = sTableName;
			da.MissingSchemaAction = MissingSchemaAction.AddWithKey;
			da.Fill(ds, sTableName);
/*		
			foreach(DataColumn col in ds.Tables[sTableName].Columns)
			{
				if(col.AutoIncrement)
					col.AutoIncrementSeed = 1;
			}
*/			
			return da;
		}

		public SqlDataAdapter FillTable(string sSelectSql, DataSet ds, string sTableName)
		{
			SqlDataAdapter da = new SqlDataAdapter();
			da.SelectCommand = new SqlCommand(sSelectSql, _oConn);
			da.MissingSchemaAction = MissingSchemaAction.AddWithKey;
			da.Fill(ds, sTableName);
			return da;
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

		public void Add(string ParameterName, SqlDbType Type, ParameterDirection Direction, object Value)
		{
			SqlParameter sp = new SqlParameter();
			sp.ParameterName = ParameterName;
			sp.SqlDbType = Type;
			sp.Direction = Direction;
			sp.Value = Value;
			List.Add(sp);
		}

		public void AddInput(string ParameterName, SqlDbType Type, object Value)
		{
			Add(ParameterName, Type, ParameterDirection.Input, Value);	
		}

		public void AddInput(string ParameterName, DbType Type, object Value)
		{
			Add(ParameterName, Type, ParameterDirection.Input, Value);
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

		public void AddInput(string ParameterName, DbType Type, object Value, int Size)
		{
			Add(ParameterName, Type, ParameterDirection.Input, Value, Size);
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
