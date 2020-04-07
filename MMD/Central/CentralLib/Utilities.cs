using System;
using System.Xml;
using System.Data;
using System.Data.SqlClient;
using System.Collections;
using System.Collections.Specialized;


namespace MMD.Central.CentralLib
{
	/// <summary>
	/// Utility classes for MMD application.  This namespace includes classes that expose
	/// static methods that extend existing objects in MMDLib and System class libraries.
	/// </summary>
	namespace Utilities
	{
		/// <summary>
		/// A collection of static methods to facilitate working with System.Xml objects
		/// </summary>
		public class Xml
		{
			/// <summary>
			/// Appends a child element to the given Parent <c>XmlElement</c> with the specified value.
			/// <seealso cref="AppendChildElement"/>
			/// </summary>
			/// <param name="xmld"><c>XmlDocument</c> containing xmleParent</param>
			/// <param name="xmleParent">The element to append new child element to</param>
			/// <param name="sElement">The name of the new child element</param>
			/// <param name="oValue">The value of the new child element</param>
			/// <returns>The new child <c>XmlElement</c></returns>
			static public XmlElement AppendChildElementWithValue(XmlDocument xmld, XmlElement xmleParent, string sElement, object oValue)
			{
				XmlElement xmle = (XmlElement) xmleParent.AppendChild( xmld.CreateElement(sElement) );
				if(oValue != null)
					xmle.InnerText = oValue.ToString();
				return xmle;
			}

			/// <summary>
			/// Appends a child <c>System.Xml.XmlElement</c> to the given Parent <c>XmlElement</c>.
			/// </summary>
			/// <param name="xmld">The <c>XmlDocument</c> to add the new element to.</param>
			/// <param name="xmleParent">The parent of the new child element.</param>
			/// <param name="sElement">The name of the new child element.</param>
			/// <returns>The new child <c>XmlElement</c>.</returns>
			static public XmlElement AppendChildElement(XmlDocument xmld, XmlElement xmleParent, string sElement)
			{
				return AppendChildElementWithValue(xmld, xmleParent, sElement, null);
			}

			/// <summary>
			/// Returns the value of a given <c>XmlAttribute</c> on a particular XmlNode.  
			/// If the particular attribute doesn't exist, <c>GetAttribute</c> returns the provided default string.
			/// </summary>
			/// <param name="xmln">The <c>XmlNode</c> to query for the attribute.</param>
			/// <param name="sAttribute">The name of the attribute.</param>
			/// <param name="sDefault">Default value to return in case of no existing attribute.</param>
			/// <returns>A string containing either the attribute value, or the default.</returns>
			static public string GetAttribute(XmlNode xmln, string sAttribute, string sDefault)
			{
				try
				{
					return xmln.Attributes[sAttribute].Value;
				}
				catch
				{
					return sDefault;
				}
			}
		}

		/// <summary>
		/// A collection of static methods to deal with boxed primitives that guarantee non-null values.
		/// </summary>
		public class SafeGet
		{
			/// <summary>
			/// Attempts to coerce <c>oValue</c> to an integer.  If an exception occurs, return the <c>iDefaultValue</c>.
			/// </summary>
			/// <param name="oValue">The value to be coerced to an integer.</param>
			/// <param name="iDefaultValue">The default value used if an exception occurs.</param>
			/// <returns>The coerced or default integer.</returns>
			static public int Int(object oValue, int iDefaultValue)
			{
				try 
				{
					return System.Convert.ToInt32(oValue);
				}
				catch
				{
					return iDefaultValue;
				}
			}

			/// <summary>
			/// Attempts to coerce <c>oString</c> to a non-null string.  If null, return <c>sDefaultValue</c>
			/// </summary>
			/// <param name="oValue">The value to be coerced to a string.</param>
			/// <param name="sDefaultValue">The default value used if <c>oString</c> is null.</param>
			/// <returns>The coerced or default string.</returns>
			static public string String(object oValue, string sDefaultValue)
			{
				return (oValue == null) ? sDefaultValue : (string) oValue;
			}

			/// <summary>
			/// Attempts to coerce <c>oValue</c> to a non-null string.  If null, return an empty string.
			/// </summary>
			/// <param name="oValue">The value to be coerced to a string.</param>
			/// <returns></returns>
			static public string String(object oValue)
			{
				return String(oValue, "");
			}
		}
/*
		namespace Collections
		{
			/// <summary>
			/// A collection with string keys, whose values can be any object.  
			/// </summary>
			/// <remarks>There's probably already an implementation of this in C#, but the Lord knows I couldn't find it.</remarks>
			public class NameObjectCollection : NameObjectCollectionBase
			{
				/// <summary>
				/// Default Constructor.
				/// </summary>
				public NameObjectCollection() : base() {}

				/// <summary>
				/// Clear all members from the collection.
				/// </summary>
				public void Clear() { BaseClear(); }

				/// <summary>
				/// Indexer to get/set objects in the collection based on the string <c>sKey</c>. 
				/// </summary>
				/// <value>An object.</value>
				public object this[string sKey]
				{
					get
					{
						return BaseGet(sKey);
					}
					set
					{
						BaseAdd(sKey,value);
					}
				}
			}

		}
*/
		/// <summary>
		/// A collection of static methods to facilitate HTML rendering.
		/// </summary>
		public class HTML
		{
			/// <summary>
			/// Creates an beginning HTML SELECT element.  Used for generating standard combo boxes in a web page.
			/// </summary>
			/// <param name="sSelectName">The name of the HTML input element.</param>
			/// <returns>Rendered HTML.</returns>
			static public string StartSelect(string sSelectName)
			{
				return "<SELECT NAME=\"" + sSelectName + "\" >";
			}

			/// <summary>
			/// Creates a beginning HTML SELECT statement that supports multiple select.
			/// </summary>
			/// <param name="sSelectName">The name of the HTML input element.</param>
			/// <param name="iSize">The number of rows in the listbox.</param>
			/// <returns>Rendered HTML.</returns>
			static public string StartSelect(string sSelectName, int iSize)
			{
				return "<SELECT NAME=\"" + sSelectName +"\" MULTIPLE SIZE=\"" + iSize + "\" >";
			}

			/// <summary>
			/// Renders an end tag for an HTML SELECT control.
			/// </summary>
			/// <returns></returns>
			static public string EndSelect() { return "</SELECT>"; }

			/// <summary>
			/// Renders an HTML OPTION tag to populate a HTML SELECT CONTROL.
			/// </summary>
			/// <param name="sID">The OPTION value.</param>
			/// <param name="sValue">The OPTION display text.</param>
			/// <param name="sSelected">If equal to <c>sID</c>, then mark this OPTION as SELECTED.</param>
			/// <returns></returns>
			static public string MakeOption(string sID, string sValue, string sSelected)
			{
				return "<OPTION VALUE=\"" + sID + "\" " + ((sID == sSelected) ? "SELECTED" : "") + " >" + sValue + "</OPTION>";
			}

			/// <summary>
			/// Creates a HIDDEN HTML INPUT statement with the NAME and VALUE fields.
			/// </summary>
			/// <param name="sName">The name of the HTML input element.</param>
			/// <param name="sValue">The value of the HTML input element.</param>
			/// <returns>HIDDEN HTML INPUT statement.</returns>
			static public string MakeHiddenInput(string sName, string sValue)
			{
				return "<input type=\"hidden\" name=\"" + sName + "\" value=\"" + sValue + "\" >";
			}

			/// <summary>
			/// Escapes characters in a string that are NON-ASCII.
			/// </summary>
			/// <param name="sHTML">The string to HTML escape.</param>
			/// <returns>HTML escaped string.</returns>
			static public string Latin1ToHTML(string sHTML)
			{
				string s = "";
				for(int i=0;i<sHTML.Length;i++)
				{
					if(sHTML[i] > 127)
						s += "&#" + sHTML[i] + ";";
					else
						s += sHTML[i];
				}
				return s;
			}
		}

		/// <summary>
		/// A collection of static methods to facilitate working with SQL programmatically.
		/// </summary>
		public class SQL
		{
			/// <summary>
			/// Encodes a string for use in a SQL WHERE clause.  
			/// It contains the string in single quotes (' '), and escapes single quotes in the string.
			/// </summary>
			/// <param name="sValue">The string to encode.</param>
			/// <returns></returns>
			static public string EncodeFull(string sValue)
			{
				return "'" + sValue.Replace("'","''") + "'";
			}

			/// <summary>
			/// Encodes a <c>DateTime</c> for use in a SQL WHERE clause.
			/// </summary>
			/// <param name="dt">The <c>DateTime</c> value to encode.</param>
			/// <returns></returns>
			static public string EncodeDate(DateTime dt)
			{
				return EncodeFull(dt.ToString("G"));
			}

			static public object SafeGetDate(object o)
			{
				return (o is System.DBNull) ? null : (object)Convert.ToDateTime(o);
			}

			static public string SafeGetString(object o)
			{
				return (o is System.DBNull) ? null : Convert.ToString(o);
			}
		}

		public class File
		{
			static public string GetFileExtension(string sFileName)
			{
				int iPos = sFileName.LastIndexOf('.');
				if(iPos < 0)
					return null;
				return sFileName.Substring(iPos);
			}

			static public string GetFileName(string sFileName)
			{
				int iPos = sFileName.LastIndexOf('.');
				int iPosSlash = sFileName.LastIndexOf('\\');
				if(iPos < 0)
					return sFileName.Substring(iPosSlash + 1);
				else
					return sFileName.Substring(iPosSlash + 1,iPos - iPosSlash - 1);
			}
		}	

		namespace Excel
		{
			public class ExcelWriter : CSWObjectBase
			{
				public ExcelWriter(CSWContext ctx) : base(ctx) {}


				public void Write(string s) { Context.Response.Write(s); }
				public void Write(string sFormat, params object[] args) { Context.Response.Write(String.Format(sFormat, args)); }

				public string IntTwo(int i) { return (i < 10) ? "0" + i : i.ToString(); }

				public string DateTimeToExcelTime(DateTime dt)
				{
					return "" + dt.Year + "-" + IntTwo(dt.Month) + "-" + IntTwo(dt.Day) + "T" + IntTwo(dt.Hour) + ":" + IntTwo(dt.Minute) + ":" + dt.Second + "." + dt.Millisecond;
				}

				//    <Cell ss:StyleID="s21"><Data ss:Type="String">a</Data></Cell>
				public void EmitCell(string sValue, string sDataType, string sStyleID)
				{
					Write("\t\t<ss:Cell");
					if(sStyleID != null && sStyleID != "")
						Write(" ss:StyleID=\"{0}\"",sStyleID);
					Write(">");

					if(sValue != null && sValue.Length > 0)
					{
						Write("<ss:Data ss:Type=\"{0}\">",sDataType);
						Write(HTML.Latin1ToHTML(System.Security.SecurityElement.Escape(sValue)));
						Write("</ss:Data>");
					}

					Write("</ss:Cell>\r\n");
				}

				public void EmitInteger(int i, string sStyleID) { EmitNumber(i.ToString(),sStyleID); }
				public void EmitDouble(double d, string sStyleID) { EmitNumber(d.ToString(),sStyleID); }
				public void EmitNumber(string sValue, string sStyleID) { EmitCell(sValue,"Number",sStyleID); }
				public void EmitString(string sValue, string sStyleID) { EmitCell(sValue,"String",sStyleID); }
				public void EmitDateTime(string sValue, string sStyleID) { EmitCell(sValue,"DateTime",sStyleID); }
				public void EmitShortDateTime(object o) 
				{ 
					EmitCell((o != null) ? o.ToString() : null,"DateTime","s22"); 
				}
				
				public void EmitLongDateTime(object o) 
				{ 
					EmitCell((o != null) ? o.ToString() : null,"DateTime", "s23"); 
				}

				public void EmitColumn() { Write("\t<Column ss:AutoFitWidth=\"1\" />\r\n"); }
				public void StartRow() { Write("\t<ss:Row>\r\n"); }
				public void StartRowCaption(object o) { Write("\t<ss:Row c:Caption=\"{0}\">\r\n",o); }
				public void EndRow() { Write("\t</ss:Row>\r\n"); }

				public void StartWorkSheet(string sName)
				{
					Write("<ss:Worksheet ss:Name=\"{0}\">\r\n",sName);
					Write("<ss:Table x:FullColumns=\"1\" x:FullRows=\"1\">\r\n");
				}

				public void EndWorkSheet()
				{
					Write("</ss:Table>\r\n");
					Write("</ss:Worksheet>\r\n");
				}

				public void StartWorkBook(string sTitle, string sAuthor, string sCompany)
				{
					Context.Response.ContentType = "application/vnd.ms-excel";
					Context.Response.AddHeader("Content-Disposition",String.Format("attachment; filename=\"{0}.xls\"",sTitle));
					Context.Response.Clear();

					Write("<?xml version=\"1.0\"?>\r\n");
					Write("<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\"");
					Write(" xmlns:o=\"urn:schemas-microsoft-com:office:office\"");
					Write(" xmlns:x=\"urn:schemas-microsoft-com:office:excel\"");
					Write(" xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\"");
					Write(" xmlns:html=\"http://www.w3.org/TR/REC-html40\">\r\n");
					Write("<DocumentProperties xmlns=\"urn:schemas-microsoft-com:office:office\">\r\n");
					Write("\t<Author>{0}</Author>\r\n",sAuthor);
					Write("\t<Company>{0}</Company>\r\n",sCompany);
					Write("\t<Version>10.2625</Version>\r\n");
					Write("</DocumentProperties>\r\n");
					Write("<Styles>\r\n");
					Write("\t<Style ss:ID=\"Default\" ss:Name=\"Normal\">\r\n");
					Write("\t\t<Alignment ss:Vertical=\"Bottom\"/>\r\n");
					Write("\t\t<Borders/>\r\n");
					Write("\t\t<Font/>\r\n");
					Write("\t\t<Interior/>\r\n");
					Write("\t\t<NumberFormat/>\r\n");
					Write("\t\t<Protection/>\r\n");
					Write("\t</Style>\r\n");
					Write("\t<Style ss:ID=\"s21\"><Font x:Family=\"Swiss\" ss:Bold=\"1\"/></Style>\r\n");
					Write("\t<Style ss:ID=\"s22\"><NumberFormat ss:Format=\"Short Date\"/></Style>\r\n");
					Write("\t<Style ss:ID=\"s23\"><NumberFormat ss:Format=\"General Date\"/></Style>\r\n");
					Write("\t<Style ss:ID=\"s24\"><Alignment ss:Vertical=\"Bottom\" ss:ShrinkToFit=\"1\"/></Style>\r\n");
					Write("</Styles>\r\n");
				}

				public void EndWorkBook()
				{
					Write("</Workbook>\r\n");
				}

			}
		}
	}
}
