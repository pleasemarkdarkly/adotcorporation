using System;
using System.Collections;
using System.Data;
using System.Data.SqlClient;
using System.Data.SqlTypes;
using System.Xml.Serialization;
using MMD.Central.CentralLib.Data;
using MMD.Central.CentralLib;

namespace MMD.Central.CentralLib.XmlObjects
{

	public class ProductType
	{
		public ProductType() {}

		[XmlAttribute("dbid")]
		public int idProductType;

		[XmlElement("Name",DataType="normalizedString")]
		public string sName;

		public ProductType(DataRow row, DataSetHolder dsh)
		{
			idProductType = Convert.ToInt32(row["idProductType"]);
			sName = Convert.ToString(row["sName"]);
		}
	}

	public class MediaType
	{
		public MediaType() {}

		[XmlAttribute("dbid")]
		public int idMediaType;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		public MediaType(DataRow row, DataSetHolder dsh)
		{
			idMediaType = Convert.ToInt32(row["idMediaType"]);
			sName = Convert.ToString(row["sName"]);
		}
	}

	public class Genre
	{
		public Genre() {}
	
		[XmlAttribute("dbid")]
		public int idGenre;

		private int idMediaType;

		public MediaType MediaType;

		[XmlElement("Name",IsNullable=true)]
		public string sName;

		[XmlElement("DisplayName",IsNullable=true)]
		public string sDescription;

		//		[XmlAttribute("UseInDevice")]
		//		public bool bUseInDevice;

		public Genre(DataRow row, DataSetHolder dsh)
		{
			idGenre = Convert.ToInt32(row["idGenre"]);
			idMediaType = Convert.ToInt32(row["idMediaType"]);
			sName = Convert.ToString(row["sName"]);
			sDescription = Convert.ToString(row["sDescription"]);

			DataRow[] rows = dsh.Tables["MediaType"].Select("idMediaType = " + idMediaType);
			if(rows.Length > 0) MediaType = new MediaType(rows[0],dsh);
		}
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class Product
	{
		public Product() {}

		[XmlAttribute("dbid")]
		public int idProduct;

		[XmlElement("UPC")]
		public string sUPC;

		[XmlElement("Name",IsNullable=true)]
		public string sName;

		[XmlElement("Title",IsNullable=true)]
		public string sTitle;

		[XmlElement("DateCopyrighted",IsNullable=true)]
		public NullableDateTime dtCopyright;

		[XmlElement("DateReleased",IsNullable=true)]
		public NullableDateTime dtRelease;

		[XmlElement("DateCreated",IsNullable=true)]
		public NullableDateTime dtCreated;
		

		public Product(DataRow row)
		{
			idProduct = Convert.ToInt32(row["idProduct"]);
			sUPC = Convert.ToString(row["sUPC"]);
			sName = Convert.ToString(row["sName"]);
			sTitle = Convert.ToString(row["sTitle"]);
			dtCopyright = NullableDateTime.CreateInstance(row["dtCopyright"]);
			dtRelease = NullableDateTime.CreateInstance(row["dtRelease"]);
			dtCreated = NullableDateTime.CreateInstance(row["dtCreated"]);
		}
	}

	public class ProductValidator : CSWObjectBase
	{
		public ProductValidator(CSWContext ctx) : base(ctx)	{ }

		public bool IsValidItemId(int idItem)
		{
			DataSet ds = Context.Config.MainDatabase.execSQL_DS("select * from tblMMD_MetaDataItem where idItem = " + idItem);
			return ds.Tables[0].Rows.Count != 0;
		}
	}

	/*
	public class ProductPersister : PersisterBase
	{

	}
	*/
}
