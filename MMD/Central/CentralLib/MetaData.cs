using System;
using System.Data;
using System.Data.SqlClient;
using System.Xml;
using System.Xml.Serialization;
using MMD.Central.CentralLib;
using MMD.Central.CentralLib.Data;
using MMD.Central.CentralLib.Utilities;
using MMD.Central.CentralLib.XmlObjects;

namespace  MMD.Central.CentralLib.ServiceImpl
{
	/// <summary>
	/// Summary description for RecListService.
	/// </summary>
	public class MetadataImpl : CSWObjectBase 
	{
		public MetadataImpl(CSWContext ctx) : base(ctx)
		{
		}

		private DataSetHolder GetGenreDataSet()
		{
			Data.Connection conn = Context.Config.MainDatabase;
			DataSetHolder ds = new DataSetHolder();

			ds.FillTable(conn,"MediaType","select * from tblMMD_MetaDataMediaType");
			ds.FillTable(conn,"Genre","select * from tblMMD_MetaDataGenre");

			ds.Relations.Add("MT_GENRE",ds.Tables["MediaType"].Columns["idMediaType"],ds.Tables["Genre"].Columns["idMediaType"]);
			return ds;
		}

		public MediaType[] GetAllMediaTypes()
		{
			DataSetHolder ds = GetGenreDataSet();
			DataRow[] rows = ds.Tables["MediaType"].Select();
			MediaType[] arTypes = new MediaType[rows.Length];
			for(int i=0;i<rows.Length;i++)
				arTypes[i] = new MediaType(rows[i],ds);
			return arTypes;
		}

		public MediaType GetMediaTypeById(int idMediaType)
		{
			DataSetHolder ds = GetGenreDataSet();
			DataRow row = ds.Tables["MediaType"].Rows.Find(idMediaType);
			if(row != null)
				return new MediaType(row, ds);
			else
				return null;
		}

		public Genre[] GetAllGenres()
		{
			DataSetHolder ds = GetGenreDataSet();
			DataRow[] rows = ds.Tables["Genre"].Select();
			Genre[] arGenres = new Genre[rows.Length];
			for(int i=0;i<rows.Length;i++)
				arGenres[i] = new Genre(rows[i],ds);

			return arGenres;
		}

		public Genre[] GetGenresByMediaType(int idMediaType)
		{
			DataSetHolder ds = GetGenreDataSet();
			DataRow pRow = ds.Tables["MediaType"].Rows.Find(idMediaType);
			if(pRow != null)
			{
				DataRow[] rows = pRow.GetChildRows("MT_GENRE");
				Genre[] arGenres = new Genre[rows.Length];
				for(int i=0;i<rows.Length;i++)
					arGenres[i] = new Genre(rows[i],ds);
				return arGenres;
			}
			else
				return null;
		}



		public Genre GetGenreById(int idGenre)
		{
			DataSetHolder ds = GetGenreDataSet();
			DataRow row = ds.Tables["Genre"].Rows.Find(idGenre);
			if(row != null)
				return new Genre(row,ds);
			else
				return null;
		}

		public Product[] GetProductsByCreateDate(DateTime dtCreated,
			NullableInt idMediaType,
			NullableInt idGenre)
		{
			return GetProductsAfterDate("dtCreated",dtCreated,idMediaType,idGenre);
		}

		public Product[] GetProductsByReleaseDate(DateTime dtReleased,
			NullableInt idMediaType,
			NullableInt idGenre)
		{
			return GetProductsAfterDate("dtRelease",dtReleased,idMediaType,idGenre);
		}

		private Product[] GetProductsAfterDate(string sDateField, DateTime dt, NullableInt idMediaType, NullableInt idGenre)
		{
			string sSQL = "vwCSW_MetadataServiceProduct where " + sDateField + " > " + SQL.EncodeDate(dt);
			if(idMediaType != null)	sSQL += " and idMediaType = " + idMediaType.value;
			if(idGenre != null) sSQL += " and idGenre = " + idGenre.value;

			int iCount = Context.Config.MainDatabase.DoCountQuery(sSQL); 
			if(10000 < iCount)
				throw new ApplicationException("Too many products matched query date (" + iCount + ")");
			
			DataSetHolder ds = new DataSetHolder();
			ds.FillTable(Context.Config.MainDatabase,"Products","select * from " + sSQL);

			Product[] arProducts = new Product[ds.Tables["Products"].Rows.Count];
			for(int i=0;i<arProducts.Length;i++)
				arProducts[i] = new Product(ds.Tables["Products"].Rows[i]);

			return arProducts;
		}
	
	
	}
}
