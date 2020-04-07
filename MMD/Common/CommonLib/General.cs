using System;
using System.Text.RegularExpressions;
using System.Xml;
using System.Data;
using System.Data.SqlClient;
using MMD.Common.CommonLib.Data;

namespace MMD.Common.CommonLib
{

	public enum ProductType 
	{
		Unspecified = 0,
		Audio = 1,
		Video = 2,
		Both = 3
		// considering only audio vs video for now, that's all the distinction that is required at the moment,
		// and further the only thing that's easy to accomplish given the provider divisions.
		//Game = 3
	}

	public struct TransferDirSet 
	{
		// channel interface
		public string Transport;
		// packing / exploding
		public string Packaging;
		// import / export with database
		public string Serialize;
		// keepsake storage
		public string Archive;
	}

	public class Config
	{
		private static Config _Instance = null;

		public static Config GetInstance()
		{
			if (_Instance == null)
				_Instance = new Config();
			return _Instance;
		}

		public string ConnString 
		{
			get 
			{
				return _xmld.SelectSingleNode("//database[@id='local_principle']").InnerText;
			}
		}

		private Config()
		{
			_xmld = new XmlDocument();
			_xmld.Load("http://localhost/mmd/xml/Config.xml");
			General.ParseConnString(ConnString,out _DBUser,out _DBPassword,out _DBServer,out _DBDatabase);
			FtpClient.Host = _xmld.SelectSingleNode("//ftpClient/host").InnerText;
			FtpClient.Password = _xmld.SelectSingleNode("//ftpClient/password").InnerText;
			FtpClient.Port = Int32.Parse(_xmld.SelectSingleNode("//ftpClient/port").InnerText);
			FtpClient.User = _xmld.SelectSingleNode("//ftpClient/user").InnerText;
		}

		public int StoreNumber
		{
			get 
			{
				return int.Parse(_xmld.SelectSingleNode("//config/storeId").InnerText);
			}
		}

		public Package.Level TransferLevel 
		{
			get 
			{
				XmlNode n = _xmld.SelectSingleNode("//transferLevel");
				return (Package.Level)Enum.Parse(typeof (Package.Level), n.InnerText);
			}
		}

		// Hmmm, I sort of wanted to provide string-level access to the members of these structs,
		// but I'll probably have to just deliver the whole struct on access, since there's no way
		// to define the accesors to be nested inside, as it were.

		// TODO : impl get from config xml

		public string DBServer { get { return _DBServer; }}
		public string DBDatabase { get { return _DBDatabase; }}
		public string DBUser { get { return _DBUser; }}
		public string DBPassword { get { return _DBPassword; }}

		public FtpEndpoint FtpClient;
	
		private string _DBServer;
		private string _DBDatabase;
		private string _DBUser;
		private string _DBPassword;
		private XmlDocument _xmld;
	}

	public class General
	{
		public General()
		{
			XmlDocument xmld = new XmlDocument();
			xmld.Load("http://localhost/mmd/xml/Config.xml");
			// todo : redo config keying to allow unified but categorized keying like "local_principle" and "local_log"
			// (for now, this won't find a proper node).
			char [] chars = xmld.SelectSingleNode("TransferLevelDesignation").InnerText.ToCharArray();
			_TransferLevelDesignation = (string)chars.GetValue(0);
			
		}

		string _TransferLevelDesignation;

		// parse the local configuration files as necessary and return a connection object.
		public static Connection LocalConnection()
		{
			XmlDocument xmld = new XmlDocument();
			xmld.Load("http://localhost/mmd/xml/Config.xml");
			// todo : redo config keying to allow unified but categorized keying like "local_principle" and "local_log"
			// (for now, this won't find a proper node).
			string cs = xmld.SelectSingleNode("//databases/database[@id='local_principle']").InnerText;
			Connection conn = new Connection(cs);
			conn.CommandTimeout = int.Parse(xmld.SelectSingleNode("//databases").Attributes["timeout"].Value);
			return conn;
		}

		public static void ParseConnString(string sConn, out string sUserID, out string sPassword, out string sServer, out string sDatabase)
		{
			// separate out the server, user id, and password from the conn string, as dts wants them separated.
			Regex regex = new Regex(@"[uU]ser[\s]+[iI][dD][\s]*=[\s]*([\w]+)");
			Match match = regex.Match(sConn);
			sUserID = match.Groups[1].Value;

			regex = new Regex(@"[pP]assword[\s]*=[\s]*([\w]+)");
			match = regex.Match(sConn);
			sPassword = match.Groups[1].Value;

			regex = new Regex(@"[nN]etwork[\s]+[aA]ddress[\s]*=[\s]*([\w]+)");
			match = regex.Match(sConn);
			sServer = match.Groups[1].Value;

			regex = new Regex(@"[iI]nitial[\s]+[cC]atalog[\s]*=[\s]*([\w]+)");
			match = regex.Match(sConn);
			sDatabase = match.Groups[1].Value;
		}

		public string TransferLevel()
		{
			return _TransferLevelDesignation;
		}


		public bool IsServer()
		{
			if (_TransferLevelDesignation == "S")
				return false;
			return true;
		}

		public static DateTime DBNow()
		{
			// TODO : more efficient.
			Connection conn = LocalConnection();
			NamedFieldReader nfr = new NamedFieldReader(conn.execSQL_DR("SELECT GetDate() as TheDate"));
			if (!nfr.Read())
				throw new Exception("couldn't get DBNow");
			DateTime ret = nfr.GetDateTime("TheDate");
			return ret;
		}
	}
}
