using System;
using System.IO;

namespace MMD.Common.CommonLib
{
	/// <summary>
	/// using bcp, save data to and load data from, flat files.
	/// </summary>
	public abstract class Flatfile
	{
		private string _Filename;
		private string _Server;
		private string _Database;
		private string _User;
		private string _Password;
		private string _Table;

		public string Filename 
		{ 
			set 
			{ 
				if (value == null) throw new FlatfileArgumentException("Filename required","Filename");
				_Filename = value; 
			} 
			get { return _Filename; }
		}
		public string Server 
		{
			set { 
				if (value == null) throw new FlatfileArgumentException("Server required","Server");
				_Server = value; 
			}
			get { return _Server; }
		}
		public string Database 
		{
			set { 
				if (value == null) throw new FlatfileArgumentException("Database required","Database");
				_Database = value; 
			}
			get { return _Database; }
		}
		public string User
		{
			set { 
				if (value == null) throw new FlatfileArgumentException("User required","User");
				_User = value; 
			}
			get { return _User; }
		}
		public string Password
		{
			set { 
				if (value == null) throw new FlatfileArgumentException("Password required","Password");
				_Password = value; 
			}
			get { return _Password; }
		}
		public string Table
		{
			set { 
				if (value == null) throw new FlatfileArgumentException("Table required","Table");
				_Table = value; 
			}
			get { return _Table; }
		}

		public Flatfile () {}

		protected virtual void CheckMembers()
		{
			if (_Filename == null)
				throw new FlatfileException("filename null");
			if (_Server == null)
				throw new FlatfileException("server null");
			if (_Database == null)
				throw new FlatfileException("database null");
			if (_User == null)
				throw new FlatfileException("user null");
			if (_Password == null)
				throw new FlatfileException("password null");
			if (_Table == null)
				throw new FlatfileException("table null");
		}
	}
	public class FlatfileException : Exception 
	{
		public FlatfileException(string description) : base(description) {}
		public override string ToString()
		{
			return "FlatfileException: " + Message + " " + StackTrace;
		}
	}
	public class FlatfileArgumentException : ArgumentException
	{
		public FlatfileArgumentException(string param) : base(param) {}
		public FlatfileArgumentException(string param, Exception inner) : base(param,inner) {}
		public FlatfileArgumentException(string description, string param) : base(description,param) {}
		public FlatfileArgumentException(string description, string param, Exception inner) : base(description,param,inner) {}
		public override string ToString()
		{
			// TODO : tidy if ugly.
			return "FlatfileArgumentException: " + ParamName + " (" + Message + ") " + StackTrace;
		}
	}
}
