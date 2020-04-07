using System;
using System.Collections;
using System.IO;
using ICSharpCode.SharpZipLib.Zip;

namespace MMD.Common.CommonLib
{
	public class RecursiveZip
	{
		private string _Filename;
		private string _TreeRoot;

		public string Filename 
		{
			set 
			{ 
				if (value == null)
					throw new ArgumentNullException("Filename");
				if (!value.EndsWith(".zip"))
					throw new ArgumentException("expected full path ending in .zip",Filename);
				_Filename = value;
			}
			get 
			{
				return _Filename;
			}
		}

		public string TreeRoot
		{
			set
			{
				if (value != null && !value.EndsWith("\\"))
					_TreeRoot = value + "\\";
				else
					_TreeRoot = value;
			}
			get 
			{
				return _TreeRoot;
			}
		}
		/// <summary>
		/// Preferred form for zipping
		/// </summary>
		/// <param name="Filename">The zip file</param>
		/// <param name="TreeRoot">The root of the directory tree to zip.  Not required for unzipping.</param>
		public RecursiveZip(string Filename, string TreeRoot)
		{
			this.Filename = Filename;
			this.TreeRoot = TreeRoot;
		}
		/// <summary>
		/// Preferred form for unzipping
		/// </summary>
		/// <param name="Filename">The zip file</param>
		public RecursiveZip(string Filename)
		{
			this.Filename = Filename;
		}
		public RecursiveZip(){}

		public void ZipDir()
		{
			CheckMembersForZip();
			ZipOutputStream zos = null;
			try 
			{
				string sTreeParent = FilePath.ParentPathFromPath(_TreeRoot);
				string sTreeDirname = FilePath.NameFromPath(_TreeRoot) + "\\";

				zos = new ZipOutputStream(File.Create(_Filename));
				Queue dirs = new Queue();
				dirs.Enqueue(sTreeDirname);
				byte[] buffer = new byte[1024*8];
				int nBytes;
				while (dirs.Count != 0)
				{
					// yank off a todo directory rel-path
					string sDir = (string) dirs.Dequeue();
					// process the normal files in this dir
					string [] aFiles = Directory.GetFiles(sTreeParent + sDir);
					foreach (string sFile in aFiles)
					{
						string sFileName = FilePath.NameFromPath(sFile);
						ZipEntry zentry = new ZipEntry(sDir + sFileName);
						zos.PutNextEntry(zentry);
						// open the source file sharingly
						FileStream fs = new FileStream(sTreeParent + sDir + sFileName,FileMode.Open,FileAccess.Read,FileShare.Read);
						while (true)
						{
							nBytes = fs.Read(buffer,0,buffer.Length);
							if (nBytes == 0)
								break;
							zos.Write(buffer,0,nBytes);
						}
						fs.Close();
						fs = null;
					}
					aFiles = null;
					// process subdirs
					string [] aDirs = Directory.GetDirectories(sTreeParent+sDir);
					foreach (string sSubDir in aDirs)
					{
						// the subdir will have a trailing slash.
						string sFilename = FilePath.NameFromPath(sSubDir);
						dirs.Enqueue(sDir + sFilename + "\\");
					}
				}
			}
			catch (Exception e)
			{
				throw new RecursiveZipException(e.Message);
			}
			finally 
			{
				if (zos != null)
				{
					zos.Finish();
					zos.Close();
				}
			}
		}

		// unzips a zip file containing one top level directory into the parent directory of the source zip.
		// returns the path of the unzipped top level directory.
		public string UnzipToDir()
		{
			CheckMembersForUnzip();

			ZipInputStream zis = new ZipInputStream(File.OpenRead(_Filename));
			string sZipParentDir = FilePath.ParentPathFromPath(_Filename);
			ZipEntry zentry;
			FileStream stream;
			int bytes;
			byte[] buffer = new byte[2048];
			string sUnzippedRoot = null;
			try 
			{
				while ((zentry = zis.GetNextEntry()) != null)
				{
					// make sure all the current entry's directory pieces exist before trying to create it.
					FilePath.CreatePath(sZipParentDir + zentry.Name);
					// store the path to the unzipped root dir for return.
					if (sUnzippedRoot == null)
						sUnzippedRoot = sZipParentDir + zentry.Name.Substring(0,zentry.Name.IndexOf("\\"));
					// create the file
					stream = File.Create(sZipParentDir + zentry.Name);
					// stream the unzipped contents to the file.
					while (true)
					{
						bytes = zis.Read(buffer,0,buffer.Length);
						if (bytes == 0)
						{
							stream.Flush();
							stream.Close();
							break;
						}
						stream.Write(buffer,0,bytes);
					}
				}
			}
			finally 
			{
				if (zis != null)
					zis.Close();
			}
			return sUnzippedRoot;
		}

		// verify that critical members are non-null.
		private void CheckMembersForZip()
		{
			if (_Filename == null)
				throw new RecursiveZipException("Filename required");
			if (_TreeRoot == null)
				throw new RecursiveZipException("TreeRoot required");
		}
		private void CheckMembersForUnzip()
		{
			if (_Filename == null)
				throw new RecursiveZipException("Filename required");
		}

	}
	public class RecursiveZipException : Exception 
	{
		public RecursiveZipException(string description) : base(description) {}
		public override string ToString()
		{
			return "RecursiveZipException: " + Message + " " + StackTrace;
		}
	}

}
