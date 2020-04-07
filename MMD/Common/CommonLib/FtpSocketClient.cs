using System;
using System.IO;
using System.Text;
using System.Text.RegularExpressions;
using System.Net;
using System.Net.Sockets;
using System.Collections;

namespace MMD.Common.CommonLib
{
	public struct FtpEndpoint 
	{
		public string Host;
		public int Port;
		public string User;
		public string Password;
	}

	public enum ResponseType
	{
		None=0,
		PositivePrelim=1,
		PositiveComplete=2,
		PositiveFurtherInfo=3,
		TransientNegative=4,
		PermanentNegative=5
	}

	public class FtpSocketClient
	{
		public FtpSocketClient()
		{
		}

		public void Connect()
		{
			_CommandSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
			_CommandSocket.Connect(new IPEndPoint(Dns.Resolve(Host).AddressList[0], Port));
			GetResponse();
			if (!LastResponseOk)
				throw new Exception("failed to connect");
		}
		public void Login()
		{
			SendCommand("USER " + _User);
			if (!LastResponseOk)
				throw new Exception("user rejected");
			SendCommand("PASS " + _Password);
			if (!LastResponseOk)
				throw new Exception("password rejected");
		}

		public string List()
		{
			StartListening();
			SendCommand("LIST");
			if (!LastResponseOk)
				throw new Exception("listing failed");
			byte[] received = ReceiveBufferData();
			// pick off the "transfer complete" response
			GetResponse();
			if (!LastResponseOk)
				throw new Exception("listing failed");
			StopListening();
			string listing = Encoding.ASCII.GetString(received);
			return listing;
		}

		public void CD(string dir)
		{	
			SendCommand("CWD " + dir);
			if (!LastResponseOk)
				throw new Exception("change dir failed");
		}

		public void MakeDir(string dir)
		{
			SendCommand("MKD " + dir);
			// TODO : make more certain, but for now a failure here probably means the dir was already there.
			//if (!LastResponseOk)
			//	throw new Exception("make dir failed");
		}

		public void MakeDirRecursive(string dir)
		{
			string [] legs = FilePath.Split(dir);
			int i = 0;
			foreach (string leg in legs)
			{
				if (leg != "")
				{
					MakeDir(leg);
					CD(leg);
					++i;
				}
			}
			for (int j = 0; j < i; ++j)
				CD("..");
		}
			
		private void GetResponse()
		{
			byte[] buffer = new byte[1023];
			int bytes;
			string response = String.Empty;
			try 
			{
				do 
				{
					bytes = _CommandSocket.Receive(buffer);
					response += Encoding.ASCII.GetString(buffer, 0, bytes);
				} while (!ResponseValid(response));
				if (response.Length >= 1) 
				{
					_LastResponse = response;
					Console.WriteLine("Recv: " + response + Environment.NewLine);
					_LastResponseType = (ResponseType) Enum.Parse(typeof(ResponseType),response.Substring(0, 1));
				} 
				else 
				{
					_LastResponse = String.Empty;
					_LastResponseType = ResponseType.PermanentNegative;
				}
			} 
			catch (Exception e)
			{
				throw new Exception("couldn't parse response", e);
			}

		}

		// Does similar to StartListening, but without the listening part, for sending files.
		// Can't find decent sample code that does this cleaner, can't think for self at moment.
		private void InitSendSock()
		{
			IPHostEntry he = Dns.Resolve(Host);
			IPEndPoint ep = new IPEndPoint(he.AddressList[0], 11000);

			_DataSocket = new Socket(ep.Address.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
			try 
			{ 
				_DataSocket.Connect(ep);
			}
			catch (Exception e)
			{
				throw new FtpClientException("data socket can't connect",e);
			}
		}

		private void ShutdownSendSock()
		{
			_DataSocket.Shutdown(SocketShutdown.Both);
			_DataSocket.Close();
			_DataSocket = null;
		}

		private bool _Listening = false;
		private void StartListening()
		{
			if (_Listening)
				throw new Exception("already listening on StartListening");

			_DataSocket = new Socket(AddressFamily.Unspecified, SocketType.Stream, ProtocolType.Tcp);
			_DataSocket.Bind(new IPEndPoint(Dns.Resolve(Dns.GetHostName()).AddressList[0], 0));
			
			_DataSocket.Listen(10);
			_Listening = true;
			IPEndPoint LocalEP = (IPEndPoint)(_DataSocket.LocalEndPoint);
			// Let the server know which port we're listening on
			SendCommand("PORT " + ((IPEndPoint)(LocalEP)).Address.ToString().Replace(".", ",") + "," + Math.Floor(LocalEP.Port / 256).ToString() + "," + (LocalEP.Port % 256).ToString());
			if (!LastResponseOk)
				throw new Exception("couldn't set up port for data transfer");
		}
		private void StopListening()
		{
			if (!_Listening)
				throw new Exception("not listening on stop");
			
			_DataSocket.Close();
			_DataSocket = null;
			
			_Listening = false;
		}

		private byte[] ReceiveBufferData()
		{
			byte[] buffer = new Byte[1024];
			byte[] data = new Byte[1024];
			int databytes = 0;
				
			Socket handler = null;

			try 
			{
				handler = _DataSocket.Accept();
				int bytes = handler.Receive(buffer);
				while (bytes != 0) 
				{
					if (bytes > data.Length - databytes)
					{
						// grow by necessary + 10% of previous size
						byte [] newdata = new Byte[databytes+bytes+(int)(data.Length/10)];
						Array.Copy(data,newdata,databytes);
						data = newdata;
					}
					Array.Copy(buffer,0,data,databytes,bytes);
					databytes += bytes;
				
					bytes = handler.Receive(buffer);
				}
			}
			finally 
			{
				if (handler != null)
				{
					handler.Shutdown(SocketShutdown.Both);
					handler.Close();
				}
			}

			return data;
		}

		private int SendFileData(string path)
		{
			Socket handler = null;
			byte[] buffer = new Byte[1024];
				
			FileStream fs = null;
			try 
			{
				fs = File.OpenRead(path);
			}
			catch 
			{
				throw new FtpClientException("couldn't open source file " + path);
			}


			int total = 0;
			try 
			{
				handler = _DataSocket.Accept();
				int bytes = fs.Read(buffer,0,buffer.Length);
				while (bytes != 0) 
				{
					int sent = handler.Send(buffer,0,bytes,SocketFlags.None);
					if (sent != bytes)
						throw new FtpClientException(String.Format("bytes transferred {0} != bytes attempted {1}{2}",sent,bytes,Environment.NewLine));
					total += sent;
					bytes = fs.Read(buffer,0,buffer.Length);
				}
			}
			finally 
			{
				if (fs != null)
					fs.Close();
				if (handler != null)
					handler.Close();
			}
			return total;

		}

		private int ReceiveFileData(string path)
		{
			byte[] buffer = new Byte[1024];
				
			Socket handler = null;

			FileStream fs = null;
			try {
				fs = File.OpenWrite(path);
			}
			catch {
				throw new FtpClientException("couldn't open dest file " + path);
			}

			int total = 0;
			try 
			{
				handler = _DataSocket.Accept();
				int bytes = handler.Receive(buffer);
				while (bytes != 0) 
				{
					fs.Write(buffer,0,bytes);
					total += bytes;
					bytes = handler.Receive(buffer);
				}
			}
			finally 
			{
				if (handler != null)
				{
					handler.Shutdown(SocketShutdown.Both);
					handler.Close();
				}
				fs.Flush();
				fs.Close();
			}
			return total;
		}

		private bool ResponseValid(string Input) 
		{
			string[] Lines = Input.Split('\n');
			if (Lines.Length > 1) 
			{
				try 
				{
					if (Lines[Lines.Length - 2].Replace("\r", "").Substring(3, 1).Equals(" ")) 
						return true;
				} 
				catch 
				{
					return false;
				}
			}
			return false;
		}


		public void Disconnect()
		{
			if (_DataSocket != null)
			{
				_DataSocket.Close();
			}
			if (_CommandSocket != null)
			{
				_CommandSocket.Shutdown(SocketShutdown.Both);
				_CommandSocket.Close();
			}
		}

		public int GetFile(string source, string path)
		{
			StartListening();
			int bytes = 0;
			try 
			{
				SendCommand("RETR " + source);
				if (!LastResponseOk)
					throw new Exception("getfile request failed");
				bytes = ReceiveFileData(path);
				// pick off the "transfer complete" response
				GetResponse();
				if (!LastResponseOk)
					throw new Exception("getfile transfer failed");
			} 
			finally 
			{
				StopListening();
			}
			return bytes;
		}

		public int PutFile(string source, string dest)
		{
			StartListening();
			int bytes = 0;
			try 
			{
				SendCommand("STOR " + dest);
				if (!LastResponseOk)
					throw new Exception("putfile request failed");
				bytes = SendFileData(source);
				GetResponse();
				if (!LastResponseOk)
					throw new Exception("putfile transfer failed");
			}
			finally 
			{
				StopListening();
			}
			return bytes;
		}

		public void SetBinary()
		{
			SendCommand("TYPE I");
			if (!LastResponseOk)
				throw new Exception("couldn't set mode to binary");
		}

		private void SendCommand(string command)
		{
			try 
			{
				Console.WriteLine("Send: " + command + Environment.NewLine);
				_CommandSocket.Send(Encoding.ASCII.GetBytes(command + "\r\n"));
				GetResponse();
			} 
			catch (Exception e)
			{
				throw new Exception("couldn't send command " + command, e);
			}
		}

		public ResponseType LastResponseType { get { return _LastResponseType; } }
		private ResponseType _LastResponseType = 0;

		public bool LastResponseOk 
		{
			get 
			{ 
				return (_LastResponseType == ResponseType.PositivePrelim) 
					|| (_LastResponseType == ResponseType.PositiveComplete) 
					|| (_LastResponseType == ResponseType.PositiveFurtherInfo); } 
		}

		public string LastResponse { get { return _LastResponse; } }
		private string _LastResponse;

		private Socket _CommandSocket;
		private Socket _DataSocket = null;
		public string Host;
		public int Port;
			
		public string User 
		{
			get { return _User; }
			set { _User = value; }
		}
		private string _User;
			
		public string Password 
		{
			get { return _Password; }
			set { _Password = value; }
		}
		private string _Password;
	}

	public class TweFtpWalker
	{
		static void WalkTweFtpAndGenerateCatalog()
		{
			FtpSocketClient ftp = null;
			try 
			{
				ftp = new FtpSocketClient();

				ftp.Host = "64.167.74.68";
				ftp.User = "twe";
				ftp.Password = "twe";
				ftp.Port = 21;
				ftp.Connect();
				ftp.Login();

				// uncomment to generate a games catalog instead of movies.
				//ftp.CD("games");

				string toplisting = ftp.List();
				ArrayList topdirs = DirNamesFromListing(toplisting);
				topdirs = DoubleDigitEntriesFilter(topdirs);

				FileStream catalog = File.OpenWrite(@"c:\temp\mmd\twecatalog.txt");
				foreach (string dir in topdirs)
					AppendDirContentsToCatalog(ftp, dir, catalog, 0);
				catalog.Flush();
				catalog.Close();
			}
			finally 
			{
				if (ftp != null)
					ftp.Disconnect();
			}
		}

		static ArrayList DoubleDigitEntriesFilter(ArrayList dirs)
		{
			ArrayList ret = new ArrayList();
			foreach(string dir in dirs)
			{
				if (dir.Length == 2)
				{
					try 
					{ 
						Int32.Parse(dir);
						ret.Add(dir);
					}
					catch (FormatException){}
				}
			}
			return ret;
		}

		// given a listing, return just the names of directories thereunder.
		static public ArrayList DirNamesFromListing(string listing)
		{
			ArrayList dirs = new ArrayList();
			string spc = @"[\s]+";
			string nspc = @"[^\s]+";
			string pattern = @"^d[rw\-]{2}" + nspc + spc + nspc + spc 
				+ nspc + spc + nspc + spc + nspc + spc + nspc 
				+ spc + nspc + spc + nspc + spc + @"([^.][ \w]*)[\s]*$";
			Regex regex = new Regex(pattern,RegexOptions.Multiline);
			MatchCollection matches = regex.Matches(listing);
			for (int i = 0; i < matches.Count; ++i)
			{
				dirs.Add(matches[i].Groups[1].Value);
			}
			return dirs;				
		}


		// assume starting at tweroot, and make sure we back out again to leave the ftp obj in a consistent state.
		static void AppendDirContentsToCatalog(FtpSocketClient ftp, string dir, FileStream catalog, int depth)
		{
			// limit recursion to depth of 10
			if (depth > 10)
				throw new Exception("unexpected recursion depth");

			// enter the dir
			ftp.CD(dir);
			string listing = ftp.List();

			// check for payload files
			ArrayList filelines = FileEntriesFromListing(listing);
			ArrayList wmvlines = EndingInWMVFilter(filelines);
			filelines = null;
			foreach (string wmvline in wmvlines)
			{
				string entry = ExtractNameAndSizeFromListingEntry(wmvline);
				byte[] bytes = Encoding.ASCII.GetBytes(entry + Environment.NewLine);
				catalog.Write(bytes,0,bytes.Length);
			}
			wmvlines = null;

			// recurse on any subdirs
			ArrayList dirs = DirNamesFromListing(listing);
			foreach (string subdir in dirs)
				AppendDirContentsToCatalog(ftp,subdir,catalog,depth+1);

			// back out of this dir.
			ftp.CD("..");
		}
		
		static ArrayList FileEntriesFromListing(string listing)
		{
			char [] split = {'\n'};
			string [] lines = listing.Split(split);
			ArrayList ret = new ArrayList();
			foreach (string line in lines)
			{
				if (line.StartsWith("-"))
					// strip out any carriage returns, and add it to the return list
					ret.Add(line.Replace("\r",String.Empty));
			}
			return ret;
		}

		static ArrayList EndingInWMVFilter(ArrayList filelines)
		{
			ArrayList ret = new ArrayList();
			foreach (string s in filelines)
			{
				if (s.EndsWith(".wmv"))
					ret.Add(s);
			}
			return ret;
		}

		static string ExtractNameAndSizeFromListingEntry(string wmvline)
		{
			string spc = @"[\s]+";
			string nspc = @"[^\s]+";
			string pattern = @"^" + nspc + spc + nspc + spc 
				+ nspc + spc + nspc + spc + "(" + nspc + ")" + spc + nspc 
				+ spc + nspc + spc + nspc + spc + @"(.*).wmv[\s]*$";
			Regex regex = new Regex(pattern,RegexOptions.IgnoreCase);
			Match match = regex.Match(wmvline);
			if (match.Success)
				return match.Groups[2] + "\t" + match.Groups[1];
			else
				throw new Exception("couldn't find name and size in wmvline");

		}
	}
	public class FtpClientException : Exception 
	{
		public FtpClientException(string description) : base(description) {}
		public FtpClientException(string description, Exception inner) : base(description,inner) {}
		public override string ToString()
		{
			return "FtpClientException: " + Message + " " + StackTrace;
		}
	}

	// provide parsing tools for a client of a particular ftp server.
	public interface FtpServerInterpreter
	{
		// the friendly description of what server an interpreter understands
		string ServerDescription();
		// if an interpreter recognizes a given ftp server's connect response, it will return true.
		bool CanHandleConnectResponse(string response);
		// return a list of file names from the given listing.
		string [] FileNamesFromListing(string listing);
		// return a list of directory names from the given listing.
		string [] DirNamesFromListing(string listing);
	}

	// TODO : impl
	public class FtpServerInterpreter_MSFTPV5 : FtpServerInterpreter
	{
		public string ServerDescription()
		{ 
			return  "Microsoft Ftp v5.0";
		}
		public FtpServerInterpreter_MSFTPV5(){}
		// if an interpreter recognizes a given ftp server's connect response, it will return true.
		public bool CanHandleConnectResponse(string response)
		{
			Regex r = new Regex(@".*Microsoft FTP Service \(Version 5\.0\).*",RegexOptions.Singleline | RegexOptions.Multiline | RegexOptions.IgnoreCase);
			Match m = r.Match(response);
			if (m.Success)
				return true;
			return false; 
		}
		// return a list of file names from the given listing.
		public string [] FileNamesFromListing(string listing)
		{ 
			string [] lines = listing.Split('\n');
			ArrayList files = new ArrayList();

			string spc = @"[\s]+";
			string nspc = @"[^\s]+";
			string pattern = @"^" + nspc + spc + nspc + spc + "[0-9]+" + spc + "(" + nspc + ")" + @"[\s]*$";
			
			Regex r = new Regex(pattern,RegexOptions.Singleline | RegexOptions.Multiline | RegexOptions.IgnoreCase);
			Match m;
			foreach (string line in lines)
			{
				m = r.Match(line);
				if (m.Success)
					files.Add(m.Groups[1].Value);
			}
			// TODO : de-cheese.
			string [] afiles = new string[files.Count];
			int i = 0;
			foreach (object file in files)
			{
				afiles[i] = (string) file;
				++i;
			}
			return afiles;
		}
		// return a list of directory names from the given listing.
		public string [] DirNamesFromListing(string listing){ 
			throw new Exception("FtpServerInterpreter_MSFTPV5.DirNamesFromListing Not Impl");
			return null;
		}
	}
}
