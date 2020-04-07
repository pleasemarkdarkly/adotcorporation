using System;
using System.IO;
using System.Text;

namespace MMD.Common.CommonLib
{
	public class Log
	{
		public static string LogLocation = @"c:\mmd\log\DeltaPropagation.txt";
		private static FileStream _Log;
		public static void Close()
		{
			if (_Log != null)
				_Log.Close();
		}
		static Log()
		{
			FilePath.CreatePath(LogLocation);
			FilePath.GetFileOutOfWay(LogLocation);
			_Log = File.Create(LogLocation);
		}
		public static void Msg(string msg)
		{
			Console.WriteLine(msg);
			_Log.Write(Encoding.ASCII.GetBytes(msg + "\n"),0,msg.Length + 1);
			_Log.Flush();
		}
	}
}
