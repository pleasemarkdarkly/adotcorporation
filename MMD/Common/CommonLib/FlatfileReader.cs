using System;
using System.IO;
using System.Diagnostics;
using System.Data.SqlClient;
using System.Text.RegularExpressions;

namespace MMD.Common.CommonLib
{
	public class FlatfileReader : Flatfile
	{
		public FlatfileReader()
		{}
		private string RunBCPIn()
		{
			string sErrorFile = FilePath.StripExtension(Filename) + "_BcpErrors.txt";
			// Run BCP with the following args:
			// -S specifies server to run on
			// -c uses character encoding
			// -U specifies login user name
			// -P specifies login pw
			// -k specifies don't insert defaults when null encountered
			// -e specified error file path
			string sBcpArgs= " ";
			sBcpArgs += " " + Database + ".dbo." + Table;
			sBcpArgs +=  " in ";
			sBcpArgs += Filename;
			sBcpArgs += " -S" + Server;
			sBcpArgs += " -c";
			sBcpArgs += " -U" + User;
			sBcpArgs += " -P" + Password;
			sBcpArgs += " -k";
			sBcpArgs += " -e" + sErrorFile;

			Process process = new Process();
			process.StartInfo.FileName = "bcp";
			process.StartInfo.Arguments = sBcpArgs;
			process.StartInfo.UseShellExecute = false;
			process.StartInfo.RedirectStandardOutput = true;
			process.Start();
			string sBcpOutput = process.StandardOutput.ReadToEnd();
			process.WaitForExit();

			if (File.Exists(sErrorFile))
			{
				FileInfo fi = new FileInfo(sErrorFile);
				if (fi.Length == 0)
					File.Delete(sErrorFile);
			}

			return sBcpOutput;
		}
		private void ExamineBCPInOutput(string sOutput)
		{
			Regex regex = new Regex(@"^Starting Copy.*^([0-9]+) rows copied.*^Clock Time",RegexOptions.IgnoreCase | RegexOptions.Multiline | RegexOptions.Singleline);
			Match match = regex.Match(sOutput);
			int nRowsCopied;
			if (match.Groups.Count > 1)
				nRowsCopied = int.Parse(match.Groups[1].Value);

			else
				throw new FlatfileException("BCP In Failed.  Copying " + Filename + " to " + Table + " unsuccessful.");
		}
		public void ToTable()
		{
			CheckMembers();
			//FilePath.CreatePath(Filename);
			string sOutput = RunBCPIn();
			ExamineBCPInOutput(sOutput);
		}
	}
}
