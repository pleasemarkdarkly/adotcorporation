using System;
using System.Diagnostics;
using System.Text.RegularExpressions;

namespace MMD.Common.CommonLib
{
	public class FlatfileWriter : Flatfile
	{
		public FlatfileWriter() {}

		public void ToFile()
		{
			CheckMembers();
			FilePath.CreatePath(Filename);
			string sOutput = RunBCPOut();
			string sMessage;
			bool bSuccess = ExamineBCPOutOutput(sOutput,out sMessage);
			if (!bSuccess)
				throw new FlatfileException(sMessage + " " + sOutput);
		}
		private string RunBCPOut()
		{
			string sSqlSelect = "select * from [" + Database + "].[dbo].[" + Table + "]";
			// -S specifies server to run on
			// -c uses character encoding
			// -U specifies login user name
			// -P specifies login pw
			// -k specifies don't insert defaults when null encountered
			string sBcpArgs= "";
			sBcpArgs += " \"" + sSqlSelect + "\"";
			sBcpArgs += " queryout";
			sBcpArgs += " " + Filename;
			sBcpArgs += " -S" + Server;
			sBcpArgs += " -c";
			sBcpArgs += " -U" + User;
			sBcpArgs += " -P" + Password;
			sBcpArgs += " -k";

			Process process = new Process();
			process.StartInfo.FileName = "bcp";
			process.StartInfo.Arguments = sBcpArgs;
			process.StartInfo.UseShellExecute = false;
			process.StartInfo.RedirectStandardOutput = true;
			process.Start();
			string sBcpOutput = process.StandardOutput.ReadToEnd();
			process.WaitForExit();
			return sBcpOutput;
		}
		
		protected override void CheckMembers()
		{
			base.CheckMembers();
		}

		private bool ExamineBCPOutOutput(string sOutput, out string sMessage)
		{
			// look for text like:
			// "Starting copy..."
			// "XXXX rows copied"
			// "Clock Time (ms.): total      XX
			Regex regex = new Regex(@"^Starting Copy.*^([0-9]+) rows copied.*^Clock Time",RegexOptions.IgnoreCase | RegexOptions.Multiline | RegexOptions.Singleline);
			Match match = regex.Match(sOutput);
			int nRowsCopied;
			if (match.Groups.Count > 1)
				nRowsCopied = int.Parse(match.Groups[1].Value);
			else
			{
				sMessage = "BCP Out Failed.  Copying " + Table + " to " + Filename + " unsuccessful.";
				return false;
			}
			sMessage = "BCP Out Succeeded.  " + nRowsCopied + " rows copied.";
			return true;
		}
	}
}
