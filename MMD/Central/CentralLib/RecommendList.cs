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
	public class RecListImpl : CSWObjectBase 
	{
		public RecListImpl(CSWContext ctx) : base(ctx)
		{
		}

		public RecListImpl(CSWContext ctx, string User) : base(ctx)
		{
			Context.UserName = User;
		}

		public RecListType[] GetAllRecListTypes()
		{
			RecListTypePersister _rltp = new RecListTypePersister(this.Context,false,Location.Deploy);

			return _rltp.GetRecListTypes();
		}

		public RecListType GetRecListTypeById(int idRecListType)
		{
			RecListTypePersister _rltp = new RecListTypePersister(this.Context,false,Location.Deploy);

			return _rltp.GetRecListTypeById(idRecListType);
		}

		/*public void AddRecListTypes(RecListType[] recListTypes)
		{
			RecListTypePersister _rltp = new RecListTypePersister(this.Context,true,Location.Deploy);

			_rltp.Update(recListTypes);
		}*/
/*
		private DataSetHolder RecListTypeDataSet(bool bUpdating)
		{
			Data.Connection conn = Context.Config.MainDatabase;
			DataSetHolder dsh = new DataSetHolder();

			dsh.BuildAdapter(conn,"RecListType","select * from tblMMD_RecommendListType");
			return dsh;
		}

*/
		public void UpdateRecListTypes(RecListType[] recListTypes)
		{
			RecListTypePersister _rltp = new RecListTypePersister(this.Context,true,Location.Deploy);

			_rltp.Update(recListTypes);
		}

		public void RemoveRecListTypes(int[] idRecListTypes)
		{
			RecListTypePersister _rltp = new RecListTypePersister(this.Context,true,Location.Deploy);

			_rltp.RemoveTypes(idRecListTypes);
		}

		//rec list
		public RecList[] GetRecListsByRecListType(int idRecListType)
		{
			RecListPersister _rlp = new RecListPersister(this.Context,false,Location.Deploy);

			return _rlp.GetRecListsByType(idRecListType);
		}

		public RecList[] GetAllRecLists()
		{
			RecListPersister _rlp = new RecListPersister(this.Context, false, Location.Deploy);

			return _rlp.GetRecLists();
		}

		public RecList GetRecListById(int idRecList)
		{
			RecListPersister _rlp = new RecListPersister(this.Context, false, Location.Deploy);

			return _rlp.GetRecListById(idRecList);
		}

		public void UpdateRecLists(RecList[] recLists)
		{
			RecListPersister _rlp = new RecListPersister(this.Context, true, Location.Deploy);

			_rlp.Update(recLists);
		}		

		public void RemoveRecLists(int[] idRecLists)
		{
			RecListPersister _rlp = new RecListPersister(this.Context, true, Location.Deploy);

			_rlp.RemoveLists(idRecLists);
		}

		public void RemoveRecLists(int idRecListType, int idProductType, int idStoreConcept)
		{
			RecListPersister _rlp = new RecListPersister(this.Context, true, Location.Deploy);

			_rlp.RemoveLists(idRecListType, idProductType, idStoreConcept);
		}

		/*private RecList[] GetRecListsBySelect(string sSelect)
		{
			DataSetHolder ds = RecListDataSet(false);

			DataRow[] rows = (sSelect == null) ? ds.Tables["RecList"].Select() 
											   : ds.Tables["RecList"].Select(sSelect);
			RecList[] arRecList = new RecList[rows.Length];

			for(int i=0;i<rows.Length;i++)
				arRecList[i] = new RecList(rows[i],ds);

			return arRecList;
		}

		public RecList[] GetRecListsByMediaType(int idMediaType)
		{
			return GetRecListsBySelect("idMediaType = " + idMediaType);
		}

		public RecList[] GetRecListsByUPC(string sUPC)
		{
			DataSetHolder ds = RecListDataSet(false);
			
			DataRow[] rows = ds.Tables["ItemUPC"].Select("sUPC = " + SQL.EncodeFull(sUPC));
			if(rows.Length == 0)
				return null;

			rows = ds.Tables["RecListItem"].Select("idItem = " + rows[0]["idItem"]);

			RecList[] arRecList = new RecList[rows.Length];

			for(int i=0;i<rows.Length;i++)
				arRecList[i] = new RecList(rows[i].GetParentRow("RL_RLI"),ds);

			return arRecList;
		}*/
	}
}
