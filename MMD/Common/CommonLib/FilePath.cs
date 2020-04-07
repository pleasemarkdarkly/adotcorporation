using System;
using System.IO;

namespace MMD.Common.CommonLib
{
	// for simplicity, this class assumes that directories end in a slash, and
	// files don't.  ie, c:\temp\ is a directory, whereas c:\temp is a file.
	public class FilePath
	{
		// return the last leg of the path, either just the file name, or just the
		// directory name, depending on what is in path.
		public static string NameFromPath(string path)
		{
			if (IsDir(path))
			{
				int iStart = path.LastIndexOf("\\",path.Length-2,path.Length-2) + 1;
				return path.Substring(iStart,path.Length-iStart-1);
			}
			else
			{
				int iStart = path.LastIndexOf("\\",path.Length,path.Length) + 1;
				return path.Substring(iStart,path.Length-iStart);
			}
		}

		// return just the parent dir's name, not its full path.
		public static string ParentNameFromPath(string path)
		{
			try
			{
				int iEnd;
				if (IsDir(path))
				{
					iEnd = path.LastIndexOf("\\",path.Length-2,path.Length-1) - 1;
				}
				else
				{
					iEnd = path.LastIndexOf("\\",path.Length-1,path.Length) - 1;
				}
				int iStart = path.LastIndexOf("\\",iEnd,iEnd+1) + 1;
				return path.Substring(iStart,iEnd-iStart+1);
			}
			catch
			{
				throw new ArgumentException("no parent dir in path","path");
			}
		}

		// return the full path of the parent dirctory.
		public static string ParentPathFromPath(string path)
		{
			int iEnd;
			if (IsDir(path))
				iEnd = path.LastIndexOf("\\",path.Length-2,path.Length-1);
			else
				iEnd = path.LastIndexOf("\\",path.Length-1,path.Length);
			return path.Substring(0,iEnd+1);
		}

		public static bool IsUNC(string path)
		{  
			return path.StartsWith("\\\\");
		}

		public static bool IsLocal(string path)
		{
			return (path.IndexOf(":") != -1);
		}

		// simply, does it end in a slash.
		public static bool IsDir(string path)
		{
			return path.EndsWith("\\");
		}

		// does the path end in something other than a slash.
		public static bool IsFile(string path)
		{
			return !IsDir(path);
		}

		// create subdirs implied in sFilename.  expects filename of the form c:\aaa\bbb\ccc\name.ext
		public static void CreatePath(string path)
		{
			if (path == "")
				return;
			if (IsUNC(path))
				CreateNetworkPath(path);
			else if (IsLocal(path))
				CreateLocalPath(path);
			else
				throw new FilePathException("unknown path format '" + path + "'");
		}
		private static void CreateLocalPath(string path)
		{
			try 
			{
				int iSlash = path.LastIndexOf("\\");
				string sPath = "";
				char[] achSplitters = {'\\'};
				string[] saSubs = path.Substring(0,iSlash).Split(achSplitters);
				foreach (string sub in saSubs)
				{
					// build up the path, creating subdirs as needed.
					sPath += sub + "\\";
					// don't call createDirectory on drive designation
					if (sub.IndexOf(":") != -1)
						continue;
					Directory.CreateDirectory(sPath);
				}
			}
			catch (Exception)
			{
				throw new FilePathException("can't create path for " + path);
			}
		}
		private static void CreateNetworkPath(string path)
		{
			throw new FilePathException("CreateNetworkPath not implemented");
		}
		/// <summary>
		/// split the path on slash characters.
		/// </summary>
		/// <param name="path"></param>
		/// <returns></returns>
 
		public static string [] Split(string path)
		{
			char[] achSplit = {'\\','/'};
			return path.Split(achSplit);
		}
		/// <summary>
		/// Make sure the path follows the dir format, ie has a trailing slash.
		/// </summary>
		public static void EnsureDir(ref string path)
		{
			if (!path.EndsWith("\\"))
				path += "\\";
		}

		public static string NameExtension(string filename)
		{
			int slash = filename.LastIndexOf("\\");
			if (slash == -1)
				slash = 0;
			// only consider the last path leg.
			int i = filename.LastIndexOf(".",filename.Length-1,filename.Length-slash);
			if (i == -1)
				return "";
			return filename.Substring(i,filename.Length-i);
		}

		public static string StripExtension(string filename)
		{
			int slash = filename.LastIndexOf("\\");
			if (slash == -1)
				slash = 0;
			// only consider the last path leg.
			int i = filename.LastIndexOf(".",filename.Length-1,filename.Length-slash);
			if (i == -1)
				return filename;
			return filename.Substring(0,i);
		}
	}
	public class FilePathException : Exception 
	{
		public FilePathException(string description) : base(description) {}
		public override string ToString()
		{
			return "FilePathException: " + Message + " " + StackTrace;
		}
	}
}
