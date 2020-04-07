using System;
using System.Collections;
using System.Data.SqlClient;
using MMD.Common.CommonLib.Data;

namespace MMD.Common.CommonLib
{
	// given a table and a delta table, consume the delta to update the active table.
	public class DeltaTable
	{
		Connection _Conn = General.LocalConnection();
		private static readonly string _DelTable = "tblMMD_DeltaDeleteHistory";
		public DeltaTable()
		{
		}

		ArrayList TableKeys(string sTable, string sCatalog)
		{
			string sql;

			sql = "SELECT column_name FROM information_schema.key_column_usage";
			sql += String.Format(" WHERE table_name = '{0}' AND table_catalog = '{1}' ORDER BY Ordinal_Position",sTable,sCatalog);

			SqlDataReader dr = _Conn.execSQL_DR(sql);
			ArrayList pkeys = new ArrayList();
			while (dr.Read())
				pkeys.Add(dr.GetString(0));
			dr.Close();

			if (pkeys.Count == 0)
				throw new Exception("No primary keys found for table " + sTable + " during delta merge");

			return pkeys;
		}

		public void MergeDelta(string sTable, string sDeltaTable, string sCatalog)
		{
			ArrayList pkeys = TableKeys(sTable, sCatalog);

			// get column names
			string sql = String.Format("select column_name from information_schema.columns"
				+ " where table_name = '{0}' ORDER BY Ordinal_Position", sTable);
			SqlDataReader dr = _Conn.execSQL_DR(sql);
			ArrayList columns = new ArrayList();
			while (dr.Read())
			{
				string col = dr.GetString(0);
				if (!pkeys.Contains(col))
					columns.Add(col);
			}
			dr.Close();

			string sDeltaCol;
			if (columns.Contains("dtDelta") || pkeys.Contains("dtDelta"))
				sDeltaCol = "dtDelta";
			else if (columns.Contains("dtLogged") || pkeys.Contains("dtDelta"))
				sDeltaCol = "dtLogged";
			else
				throw new Exception(String.Format("table {0} must have either a dtDelta or dtLogged column to transfer via delta propagation",sTable));

			bool bDeltaActionCol = columns.Contains("sDeltaAction") || pkeys.Contains("sDeltaAction");

			// do a sophisticated add/change/delete based on the delta action.
			if (bDeltaActionCol)
			{
				// handle add rows
				// add the rows into the table
				sql = String.Format("INSERT INTO {0} SELECT * FROM {1} WHERE sDeltaAction = 'A'",sTable,sDeltaTable);
				_Conn.execSQL(sql);
				sql = String.Format("DELETE FROM {0} WHERE sDeltaAction = 'A'", sDeltaTable);
				_Conn.execSQL(sql);
				
				// remove any delete history entries that have been superceded by newer adds.
				sql = string.Format("DELETE FROM {0} FROM {0} INNER JOIN {1} ON ",_DelTable,sTable);
				int iKey = 0;
				foreach (string key in pkeys)
				{
					++iKey;
					if (iKey > 1)
						sql += " AND ";
					sql += String.Format("{0}.{1} = {2}.sKey{3}",sTable,key,_DelTable,iKey);
				}
				sql += String.Format(" WHERE {0}.sTableName = {1} AND {0}.dtDeleted < {3}.{4}",_DelTable,sTable,sDeltaCol,sDeltaTable,sDeltaCol);
				_Conn.execSQL(sql);

				bool bFirst;
				// short circuit the update process for immutable tables, if they happen to exist
				// (ie, tables where all columns are in the primkey)
				if (columns.Count != 0)
				{
					sql = "UPDATE " + sTable + " SET";
					bFirst = true;
					foreach (string col in columns)
					{
						if (!bFirst)
							sql += ",";
						bFirst = false;
						sql += String.Format(" {0}.{2} = {1}.{2} ",sTable,sDeltaTable,col);
					}
					sql += " FROM " + sTable + " INNER JOIN " + sDeltaTable
						+ " ON ";
					bFirst = true;
					foreach (string pkey in pkeys)
					{
						if (!bFirst)
							sql += " AND ";
						bFirst = false;
						sql += String.Format(" {0}.{2} = {1}.{2} ",sTable,sDeltaTable,pkey);
					}

					sql += " WHERE " + sDeltaTable + ".sDeltaAction = 'C'";

					_Conn.execSQL(sql);
				}

				// handle deletion rows
			
				// first, record the deletions in the delete history table		
				if (pkeys.Count > 4)
					throw new Exception("Unexpected : delta table " + sTable + " has more than 4 key columns");
				// example : insert into delhist select 'tblName' as sTableName, sKey1 as sKey1, NULL as sKey2
				//  from table join delta on (keys match)
				sql = String.Format("INSERT INTO tblMMD_DeltaDeleteHistory SELECT '{0}' AS sTableName",sTable);

				iKey = 1;
				foreach (string pkey in pkeys)
				{
					sql += String.Format(", {0}.{1} AS sKey{2}",sTable,pkey,iKey);
					++iKey;
				}
				for ( ; iKey <=4; ++iKey)
				{
					sql += String.Format(", Null AS sKey{0}",iKey);
				}
				sql += String.Format(", {0}.{1} AS dtDeleted", sDeltaTable, sDeltaCol);

				sql += " FROM " + sTable + " INNER JOIN " + sDeltaTable + " ON ";
				bFirst = true;
				foreach (string pkey in pkeys)
				{
					if (!bFirst)
						sql += " AND ";
					bFirst = false;
					sql += String.Format(" {0}.{2} = {1}.{2} ",sTable,sDeltaTable,pkey);
				}
				sql += " WHERE " + sDeltaTable + ".sDeltaAction = 'D'";
				_Conn.execSQL(sql);

				// do the actual delete from the target table.
				sql = "DELETE FROM " + sTable + " FROM " + sTable + " INNER JOIN " + sDeltaTable + " ON ";
				bFirst = true;
				foreach (string pkey in pkeys)
				{
					if (!bFirst)
						sql += " AND ";
					bFirst = false;
					sql += String.Format(" {0}.{2} = {1}.{2} ",sTable,sDeltaTable,pkey);
				}
				sql += " WHERE " + sDeltaTable + ".sDeltaAction = 'D'";
			
				_Conn.execSQL(sql);

			}
			// with no delta action field, we just add the rows in and that's that.
			else
			{
				sql = String.Format("INSERT INTO {0} SELECT * FROM {1}",sTable,sDeltaTable);
				_Conn.execSQL(sql);
			}
		}

		private string ThrowAwayValForDatatype(string datatype)
		{
			switch (datatype)
			{
				case "bit":
				case "int":
					return "0";
				case "datetime":
					return "'January 1, 1990'";
				case "text":
				case "varchar":
				case "char":
					return "''";
				case "uniqueidentifier":
					return "Null";
				default:
					throw new Exception("unhandled column datatype " + datatype);
			}
		}

		// for a delta style export, copy the desired range into the delta table
		public void GenDelta(string sTable, string sDeltaTable, string sSelectClause, DateTime dtFrom, DateTime dtTo, string sCatalog, bool bDeleteSent)
		{
			string sql;

			// figure out the table's key information.	
			sql = String.Format("SELECT column_name FROM information_schema.key_column_usage"
				+ " WHERE table_name = '{0}' AND table_catalog = '{1}' ORDER BY Ordinal_Position",sTable,sCatalog);

			SqlDataReader dr = _Conn.execSQL_DR(sql);
			ArrayList pkeys = new ArrayList();
			while (dr.Read())
				pkeys.Add(dr.GetString(0));
			dr.Close();

			sql = String.Format("SELECT column_name, data_type FROM information_schema.columns WHERE table_name = '{0}' and table_catalog = '{1}' ORDER BY ordinal_position",sTable,sCatalog);
			dr = _Conn.execSQL_DR(sql);
			ArrayList columns = new ArrayList();
			ArrayList types = new ArrayList();
			while (dr.Read())
			{
				columns.Add(dr.GetString(0));
				types.Add(dr.GetString(1));
			}
			dr.Close();

			string sDeltaCol;
			if (columns.Contains("dtDelta"))
				sDeltaCol = "dtDelta";
			else if (columns.Contains("dtLogged"))
				sDeltaCol = "dtLogged";
			else
				throw new Exception(String.Format("table {0} must have either a dtDelta or dtLogged column to transfer via delta propagation",sTable));


            // copy across rows that have been added or changed.
			sql = String.Format("INSERT INTO dbo.{0} (",sDeltaTable);
			bool bFirst = true;
			foreach (string column in columns)
			{
				if (!bFirst)
					sql += ", ";
				bFirst = false;
				sql += column;
			}
			sql += String.Format(") SELECT * FROM {0}",sTable);
			string sWhere = String.Format(" WHERE {0} > '{1}' AND {0} <= '{2}'",
				sDeltaCol,dtFrom.ToString("G"),dtTo.ToString("G"));
			// allow arbitrary parts of the table to get delta'd.
			if (sSelectClause != "")
				sWhere += " AND " + sSelectClause;
			sql += sWhere;
			_Conn.execSQL(sql);

			// recreate the delete delta rows.

			// inherent to the handling of this history table, there shouldn't be both an update
			// recorded as well as a delete entry.  delete history is only maintained until it
			// becomes obsolete through a subsequent ADD.

			sql = String.Format("INSERT INTO {0} SELECT",sDeltaTable);
			for (int i = 0; i < columns.Count; ++i)
			{
				if (i != 0)
					sql += ",";
				if (pkeys.Contains(columns[i]))
				{
					int iKey = pkeys.IndexOf(columns[i]);
					sql += String.Format(" {0}.sKey{1} AS {2}",_DelTable,i+1,columns[i]);
				}
				else
					sql += String.Format(" {0} AS {1}", ThrowAwayValForDatatype((string)types[i]), columns[i]);
			}
			sql += String.Format(" FROM {0} WHERE sTableName = '{1}' AND dtDeleted > '{2}' AND dtDeleted <= '{3}'",_DelTable,sTable,dtFrom.ToString("G"),dtTo.ToString("G"));
			_Conn.execSQL(sql);

			if (bDeleteSent)
			{
				sql = String.Format("DELETE FROM {0} {1}",sTable,sWhere);
				_Conn.execSQL(sql);
			}
		}
	}
}
