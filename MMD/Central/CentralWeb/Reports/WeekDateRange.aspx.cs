using System;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Web;
using System.Web.SessionState;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.HtmlControls;
/*
 * using Excel;
using MMD.Central.CentralLib.Utilities;

namespace MMD.Central.CentralWeb.Reports
{
	/// <summary>
	/// Summary description for WeekDateRange.
	/// </summary>
	public class WeekDateRange : System.Web.UI.Page
	{
		private void Page_Load(object sender, System.EventArgs e)
		{
		}


		public string GetReports()
		{
			string s = HTML.StartSelect("report");
			string[] sFiles = System.IO.Directory.GetFiles(Server.MapPath("/CentralWeb/Reports/Excel"));
			foreach(string sFile in sFiles)
			{
				string sPrettyName = sFile.Substring(sFile.LastIndexOf('\\') + 1);
				sPrettyName = sPrettyName.Substring(0,sPrettyName.LastIndexOf('.'));

				s += HTML.MakeOption(sFile,sPrettyName,sFiles[0]);
			}
			s += HTML.EndSelect();
			return s;
		}

		public void WriteResults()
		{
			if(Request["post"] != null)
			{
				Excel.Application xlApp = new Excel.ApplicationClass();
				Excel.Workbook xlWorkBook = xlApp.Workbooks.Open(Request["report"],System.Reflection.Missing.Value,System.Reflection.Missing.Value,System.Reflection.Missing.Value,System.Reflection.Missing.Value,System.Reflection.Missing.Value,System.Reflection.Missing.Value,System.Reflection.Missing.Value,System.Reflection.Missing.Value,System.Reflection.Missing.Value,System.Reflection.Missing.Value,System.Reflection.Missing.Value,System.Reflection.Missing.Value,System.Reflection.Missing.Value,System.Reflection.Missing.Value);
				Excel.Worksheet xlWorkSheet = (Excel.Worksheet)xlWorkBook.ActiveSheet;
				Excel.PivotTable xlPTable = (Excel.PivotTable)xlWorkSheet.PivotTables("PivotTable1");
				bool bRefreshed = xlPTable.RefreshTable();

				Excel.PivotFields xlPFields = (Excel.PivotFields)xlPTable.PivotFields(System.Reflection.Missing.Value);
				
				string sWeekField = "[Time].[Week].[Week]";
				Excel.PivotField pfWeek = null;

				for(int i=0;i<xlPFields.Count;i++)
				{
					Excel.PivotField xlPField = (Excel.PivotField)xlPFields.Item(i);
					if(xlPField.Name == sWeekField)
						pfWeek = xlPField;
				}
				xlPFields = null;

				object dtStart = null;
				object dtEnd = null;

				if(Request["date1"] != null) 
					dtStart = (object)Convert.ToDateTime(Request["date1"]);
				if(Request["date2"] != null)
					dtEnd = Convert.ToDateTime(Request["date2"]);

				Excel.PivotItems xlPItems = (Excel.PivotItems)pfWeek.PivotItems(System.Reflection.Missing.Value);
				

				ArrayList alHiddenItems = new ArrayList();
				Response.Write("<table>");
				
				for(int i=1;i<=xlPItems.Count;i++)
				{
					Excel.PivotItem xlPItem = (Excel.PivotItem)xlPItems.Item(i);
					Response.Write(String.Format("<tr><td>{0}</td><td>{1}</td><td>{2}</td><td>{3}</td></tr>",
										i, xlPItem.Name, xlPItem.Value, IsPivotItemVisible(xlPItem,dtStart,dtEnd) ));
					
					if(!IsPivotItemVisible(xlPItem,dtStart,dtEnd))
						alHiddenItems.Add(String.Copy(xlPItem.Name));

					xlPItem = null;

				}
				Response.Write("</table>");
				xlPItems = null;

				string[] arHiddenItems = new string[alHiddenItems.Count];
				for(int i=0;i<alHiddenItems.Count;i++)
					arHiddenItems[i] = (string)alHiddenItems[i];
				
				pfWeek.HiddenItemsList = arHiddenItems;
				pfWeek = null;

				//okay, let's try publishing
				string sFileName = Server.MapPath("/CentralWeb/Scratch/" + Guid.NewGuid() + ".htm");
				
				Excel.PublishObject xlPublish = xlWorkBook.PublishObjects.Add(
							Excel.XlSourceType.xlSourceSheet,sFileName,xlWorkSheet.Name,xlPTable.Name,Excel.XlHtmlType.xlHtmlStatic,System.Reflection.Missing.Value,"");

				//xlWorkBook.PublishObjects.Add(Excel.XlSourceType.xlSourcePivotTable,sFileName,System.Reflection.Missing.Value,System.Reflection.Missing.Value,System.Reflection.Missing.Value,System.Reflection.Missing.Value,System.Reflection.Missing.Value).Publish(true);
				xlPublish.Publish(true);
				xlPublish = null;

				xlWorkBook.Saved = true;
				xlWorkBook.Close(false,System.Reflection.Missing.Value,System.Reflection.Missing.Value);
				
				xlPTable = null;
				xlWorkSheet = null;
				xlWorkBook = null;
				xlApp.Quit();
				xlApp = null;
				//GC.Collect();

				System.IO.FileStream fs = System.IO.File.OpenRead(sFileName);
				byte[] arBytes = new byte[8*1024];
				int iBytesRead;
				while((iBytesRead = fs.Read(arBytes,0,8*1024)) > 0)
					Response.OutputStream.Write(arBytes,0,iBytesRead);
				
				Response.OutputStream.Flush();
				
				fs.Close();
				System.IO.File.Delete(sFileName);
			}

		}

		private bool IsPivotItemVisible(Excel.PivotItem pItem,object dtStart, object dtEnd)
		{
			string sWeek = pItem.Name;
			sWeek = sWeek.Substring(sWeek.LastIndexOf(' ') + 1);
			sWeek = sWeek.Substring(0,sWeek.Length-1);
			int iWeek = Convert.ToInt32(sWeek);
			
			if(dtStart != null)
			{
				if((DateTime)dtStart > GetWeekEndDate(iWeek,((DateTime)dtStart).Year ))
				{
					return false;
				}
			}

			if(dtEnd != null)
			{
				if((DateTime)dtEnd < GetWeekStartDate(iWeek, ((DateTime)dtEnd).Year ))
				{
					return false;
				}
			}

			return true;
		}

		private DateTime GetWeekEndDate(int iWeek, int iYear)
		{
			DateTime dt = new DateTime(iYear,1,1);
			while(dt.DayOfWeek != DayOfWeek.Saturday)
				dt = dt.AddDays(1);

			return dt.AddDays(7 * (iWeek - 1));
		}

		private DateTime GetWeekStartDate(int iWeek, int iYear)
		{
			DateTime dt = new DateTime(iYear,1,1);
			while(dt.DayOfWeek != DayOfWeek.Sunday)
				dt = dt.AddDays(-1);

			return dt.AddDays(7 * (iWeek - 1));
		}

		#region Web Form Designer generated code
		override protected void OnInit(EventArgs e)
		{
			//
			// CODEGEN: This call is required by the ASP.NET Web Form Designer.
			//
			InitializeComponent();
			base.OnInit(e);
		}
		
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{    
			this.Load += new System.EventHandler(this.Page_Load);
		}
		#endregion
	}
}
*/