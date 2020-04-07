using System;
using System.Collections;
using System.Data;
using System.Data.SqlClient;
using System.Data.SqlTypes;
using System.Xml.Serialization;
using MMD.Central.CentralLib.Data;
using MMD.Central.CentralLib;
using System.Diagnostics;

namespace MMD.Central.CentralLib.XmlObjects
{
	public class RecListBaseType
	{
		public RecListBaseType() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idRecListBaseType;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		[XmlElement(ElementName="System",IsNullable=false)]
		public bool bSystem;

		public RecListBaseType(DataRow row)
		{
			idRecListBaseType = Convert.ToInt32(row["idRecListBaseType"]);
			sName = Convert.ToString(row["sName"]);
			bSystem = 1 == Convert.ToInt32(row["bSystem"]);
		}
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class RecListType
	{
		public RecListType() {}
		
		[XmlAttribute("dbid")]
		public int idRecListType;

		[XmlAttribute(AttributeName="dbidRecListBaseType")]
		public int idRecListBaseType;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		[XmlElement("baseType", IsNullable=false)]
		public RecListBaseType BaseType;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

		[XmlElement(ElementName="Status",IsNullable=false)]
		public StagingStatus byStatus;

		[XmlArray("RecListTypeAttributes", IsNullable=true)]
		public RecListTypeAttribute[] RecListAttributes; // TODO: rename this to RecListTypeAttributes

		[XmlArray("RecListTypeItemAttributes", IsNullable=true)]
		public RecListTypeItemAttribute[] RecListItemAttributes; // TODO: rename this to RecListTypeItemAttributes

		public RecListType(DataRow row, DataSetHolder dsh) : this(row, dsh, Location.Staging) {}
				
		public RecListType(DataRow row, DataSetHolder dsh, Location location)
		{
			Initialize(row, dsh, location);
		}
		
		public void Initialize(DataRow row, DataSetHolder dsh, Location location)
		{
			idRecListType = Convert.ToInt32(row["idRecListType"]);
			idRecListBaseType = Convert.ToInt32(row["idRecListBaseType"]);
			sName = Convert.ToString(row["sName"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
			if(location == Location.Staging)
			{
				byStatus = (StagingStatus)Convert.ToInt32(row["byStatus"]);
			}
			DataRow[] rows = row.GetChildRows("RLT_RLTA");
			RecListAttributes = new RecListTypeAttribute[rows.Length];
			for(int i = 0;i < rows.Length; i++)
				RecListAttributes[i] = new RecListTypeAttribute(rows[i], dsh, location);
			rows = row.GetChildRows("RLT_RLTIA");
			RecListItemAttributes = new RecListTypeItemAttribute[rows.Length];
			for(int i = 0;i < rows.Length; i++)
				RecListItemAttributes[i] = new RecListTypeItemAttribute(rows[i], dsh, location);
			rows = dsh.Tables["BaseType"].Select("idRecListBaseType = " + idRecListBaseType);
			if(rows.Length > 0)
				BaseType = new RecListBaseType(rows[0]);
		}

		public void Fill(DataRow row, DataSetHolder dsh, bool bNew ,CSWContext ctx, Location location)
		{
			if(null != BaseType)
				row["idRecListBaseType"] = BaseType.idRecListBaseType;
			row["sName"] = sName;
			row["dtChanged"] = DateTime.Now;
			row["sChangedBy"] = ctx.UserName;
			if(Location.Staging == location)
			{
				row["byStatus"] = byStatus;
			}
			if(null != RecListAttributes)
			{
				foreach(RecListTypeAttribute rlta in RecListAttributes)
				{
					bool bNewRow = false;
					DataRow attrRow;
					if(0 == rlta.idRecListTypeAttribute)
					{
						bNewRow = true;
						attrRow = dsh.Tables["RLTypeAttr"].NewRow();
					}
					else
					{
						DataRow[] AttrRows = dsh.Tables["RLTypeAttr"].Select("idRecListTypeAttribute = " + rlta.idRecListTypeAttribute);
						bNewRow = AttrRows.Length == 0;
						attrRow = (bNewRow) ? dsh.Tables["RLTypeAttr"].NewRow() : AttrRows[0];
					}
					rlta.Fill(attrRow, dsh, bNewRow, ctx, location);
					attrRow.SetParentRow(row, dsh.Relations["RLT_RLTA"]);
					if(bNewRow) dsh.Tables["RLTypeAttr"].Rows.Add(attrRow);
				}
				DataRow[] rows = dsh.Tables["RLTypeAttr"].Select("idRecListType = " + idRecListType);
				for(int i = 0;i < rows.Length;i++)
				{
					bool bFound = false;
					foreach(RecListTypeAttribute rlta in RecListAttributes)
					{
						if(rlta.idRecListTypeAttribute == Convert.ToInt32(row["idRecListTypeAttribute"]))
						{
							bFound = true;
						}
					}
					if(!bFound)
						row.Delete();
				}
			}
			if(null != RecListItemAttributes)
			{
				foreach(RecListTypeItemAttribute rltia in RecListItemAttributes)
				{
					bool bNewRow = false;
					DataRow attrRow;
					if(0 == rltia.idRecListTypeItemAttribute)
					{
						bNewRow = true;
						attrRow = dsh.Tables["RLTypeItemAttr"].NewRow();
					}
					else
					{
						DataRow[] AttrRows = dsh.Tables["RLTypeItemAttr"].Select("idRecListTypeItemAttribute = " + rltia.idRecListTypeItemAttribute);
						bNewRow = AttrRows.Length == 0;
						attrRow = (bNewRow) ? dsh.Tables["RLTypeItemAttr"].NewRow() : AttrRows[0];
					}
					rltia.Fill(attrRow, dsh, bNewRow, ctx, location);
					attrRow.SetParentRow(row, dsh.Relations["RLT_RLTIA"]);
					if(bNewRow) dsh.Tables["RLTypeItemAttr"].Rows.Add(attrRow);
				}
				DataRow[] rows = dsh.Tables["RLTypeItemAttr"].Select("idRecListType = " + idRecListType);
				for(int i = 0;i < rows.Length;i++)
				{
					bool bFound = false;
					foreach(RecListTypeItemAttribute rltia in RecListItemAttributes)
					{
						if(rltia.idRecListTypeItemAttribute == Convert.ToInt32(row["idRecListTypeItemAttribute"]))
						{
							bFound = true;
						}
					}
					if(!bFound)
						row.Delete();
				}
			}
		}
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class RecList
	{
		public RecList() {}

		[XmlAttribute("dbid")]
		public int idRecList;

		[XmlAttribute("dbidRecListType")]
		public int idRecListType;

		[XmlAttribute("dbidStoreConcept")]
		public int idStoreConcept;

		[XmlAttribute("dbidProductType")]
		public int idProductType;

		[XmlElement("Type",IsNullable=false)]
		public RecListType Type;

		[XmlElement("ProductType",IsNullable=true)]
		public ProductType PType;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

		[XmlElement(ElementName="Status",IsNullable=false)]
		public StagingStatus byStatus;

		[XmlArray("RecListAttributes",IsNullable=true)]
		public RecListAttribute[] RecListAttributes;

		[XmlArray("RecListItems",IsNullable=true)]
		public RecListItem[] RecListItems;

		public RecList(DataRow row, DataSetHolder dsh) : this(row, dsh, Location.Staging) {}

		public RecList(DataRow row, DataSetHolder dsh, Location location)
		{
			idRecList = Convert.ToInt32(row["idRecList"]);
			idRecListType = Convert.ToInt32(row["idRecListType"]);
			idStoreConcept = Convert.ToInt32(row["idStoreConcept"]);
			idProductType = Convert.ToInt32(row["idProductType"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
			if(Location.Staging == location)
			{
				byStatus = (StagingStatus)Convert.ToInt32(row["byStatus"]);
			}

			DataRow[] rows = row.GetChildRows("RL_RLI");
			RecListItems = new RecListItem[rows.Length];
			for(int i=0;i<rows.Length;i++)
				RecListItems[i] = new RecListItem(rows[i], dsh, location);
			rows = row.GetChildRows("RL_RLA");
			RecListAttributes = new RecListAttribute[rows.Length];
			for(int i = 0;i < rows.Length;i++)
				RecListAttributes[i] = new RecListAttribute(rows[i], dsh, location);
			rows = dsh.Tables["RLType"].Select("idRecListType = " + idRecListType);
			if(rows.Length > 0)
				Type = new RecListType(rows[0], dsh, location);
			else
				Type = null;
			rows = dsh.Tables["ProductType"].Select("idProductType = " + idProductType);
			if(rows.Length > 0)
				PType = new ProductType(rows[0], dsh);
			else
				PType = null;
		}

		public void Fill(DataRow row, DataSetHolder dsh, bool bNew,CSWContext ctx, Location location)
		{
			row["idRecListType"] = idRecListType;
			row["idStoreConcept"] = idStoreConcept;
			row["idProductType"] = idProductType;
			row["dtChanged"] = DateTime.Now;
			row["sChangedBy"] = ctx.UserName;
			row["dtDelta"] = DateTime.Now;
			row["sDeltaAction"] = (bNew) ? 'A' : 'C';
			if(Location.Staging == location)
			{
				row["byStatus"] = byStatus;
			}
			if(bNew) dsh.Tables["RecList"].Rows.Add(row);

			if(RecListItems != null)
			{
				foreach(RecListItem rli in RecListItems)
				{
					bool bNewRow = false;
					DataRow ItemRow;
					if(0 == rli.idRecListItem)
					{
						bNewRow = true;
						ItemRow = dsh.Tables["RLItem"].NewRow();
					}
					else
					{
						DataRow[] ItemRows = dsh.Tables["RLItem"].Select("idRecListItem = " + rli.idRecListItem);
						bNewRow = ItemRows.Length == 0;
						ItemRow = (bNewRow) ? dsh.Tables["RLItem"].NewRow() : ItemRows[0];
					}
					rli.Fill(row, ItemRow, dsh, bNewRow, ctx, location);
					//ItemRow.SetParentRow(row, dsh.Relations["RL_RLI"]);
					//if(bNewRow) dsh.Tables["RLItem"].Rows.Add(ItemRow);
				}
			}
			if(RecListAttributes != null)
			{
				foreach(RecListAttribute rla in RecListAttributes)
				{
					bool bNewRow = false;
					DataRow attrRow;
					if(0 == rla.idRecListAttribute)
					{
						bNewRow = true;
						attrRow = dsh.Tables["RLAttr"].NewRow();
					}
					else
					{
						DataRow[] AttrRows = dsh.Tables["RLAttr"].Select("idRecListAttribute = " + rla.idRecListAttribute);
						bNewRow = AttrRows.Length == 0;
						attrRow = (bNewRow) ? dsh.Tables["RLAttr"].NewRow() : AttrRows[0];
					}
					rla.Fill(attrRow, dsh, bNewRow, ctx, location);
					attrRow.SetParentRow(row, dsh.Relations["RL_RLA"]);
					if(bNewRow) dsh.Tables["RLAttr"].Rows.Add(attrRow);
				}
			}
		}
	}

	public class RecListPersister : CSWObjectBase
	{
		protected bool _bUpdating;
		protected Location _location;
		protected DataSetHolder _dsh;

		public DataSetHolder DataSet { get { return _dsh; } }

		public RecListPersister(CSWContext ctx, bool bUpdating, Location location) : base(ctx)
		{
			_bUpdating = bUpdating;
			_location = location;
			_dsh = GetDataSet(bUpdating,location);
		}

		protected void UpdateDataSet()
		{
			_dsh.EnforceConstraints = false;
			_dsh.Update("RecList","idRecList");
			_dsh.Update("RLItem","idRecListItem");
			_dsh.Update("RLAttr","idRecListAttribute");
			_dsh.Update("RLItemAttr","idRecListItemAttribute");
			_dsh.EnforceConstraints = true;
			// synch the staging and deployment databases
			Context.Config.MainDatabase.execSP("sprCSW_SyncStagingRecLists", null);

		}

		protected void RemoveFromDataSet()
		{
			_dsh.EnforceConstraints = false;
			_dsh.Update("RLItemAttr","idRecListItemAttribute");
			_dsh.Update("RLAttr","idRecListAttribute");
			_dsh.Update("RLItem","idRecListItem");
			_dsh.Update("RecList","idRecList");
			_dsh.EnforceConstraints = true;
			// synch the staging and deployment databases
			Context.Config.MainDatabase.execSP("sprCSW_SyncStagingRecLists", null);
		}


		protected DataSetHolder GetDataSet(bool bUpdating, Location location)
		{
			Data.Connection conn = Context.Config.MainDatabase;
			DataSetHolder dsh = new DataSetHolder();

			dsh.FillTable(conn,"BaseType","select * from tblMMD_RecommendListBaseType");
			dsh.FillTable(conn,"AttrType", "select * from tblMMD_RecommendListAttributeType");
			dsh.FillTable(conn,"ProductType","select * from tblMMD_MetaDataProductType");

			if(location == Location.Staging)
			{
				dsh.FillTable(conn,"RLType","select * from tblMMD_RecommendListTypeStaging");
				dsh.FillTable(conn,"RLTypeAttr","select * from tblMMD_RecommendListTypeAttributeStaging");
				dsh.FillTable(conn,"RLItemTypeAttr","select * from tblMMD_RecommendListTypeItemAttributeStaging");
				dsh.AddTable(conn,"RecList","select * from tblMMD_RecommendListStaging",bUpdating);
				dsh.AddTable(conn,"RLAttr","select * from tblMMD_RecommendListAttributeStaging",bUpdating);
				dsh.AddTable(conn,"RLItem","select * from tblMMD_RecommendListItemStaging",bUpdating);
				dsh.AddTable(conn,"RLItemAttr","select * from tblMMD_RecommendListItemAttributeStaging",bUpdating);
			}
			else
			{
				dsh.FillTable(conn,"RLType","select * from tblMMD_RecommendListType");
				dsh.FillTable(conn,"RLTypeAttr","select * from tblMMD_RecommendListTypeAttribute");
				dsh.FillTable(conn,"RLItemTypeAttr","select * from tblMMD_RecommendListTypeItemAttribute");
				dsh.AddTable(conn,"RecList","select * from tblMMD_RecommendList",bUpdating);
				dsh.AddTable(conn,"RLAttr","select * from tblMMD_RecommendListAttribute",bUpdating);
				dsh.AddTable(conn,"RLItem","select * from tblMMD_RecommendListItem",bUpdating);
				dsh.AddTable(conn,"RLItemAttr","select * from tblMMD_RecommendListItemAttribute",bUpdating);
			}

			dsh.Relations.Add("RLT_RLTIA",dsh.Tables["RLType"].Columns["idRecListType"],dsh.Tables["RLItemTypeAttr"].Columns["idRecListType"]);
			dsh.Relations.Add("RLT_RLTA",dsh.Tables["RLType"].Columns["idRecListType"],dsh.Tables["RLTypeAttr"].Columns["idRecListType"]);
			dsh.Relations.Add("RL_RLA",dsh.Tables["RecList"].Columns["idRecList"],dsh.Tables["RLAttr"].Columns["idRecList"]);
			dsh.Relations.Add("RL_RLI",dsh.Tables["RecList"].Columns["idRecList"],dsh.Tables["RLItem"].Columns["idRecList"]);
			dsh.Relations.Add("RLI_RLIA",dsh.Tables["RLItem"].Columns["idRecListItem"],dsh.Tables["RLItemAttr"].Columns["idRecListItem"]);

			return dsh;
		}

		public RecListType GetRecListTypeById(int idRecListType)
		{
			DataRow[] rows = _dsh.Tables["RLType"].Select("idRecListType = " + idRecListType);
			RecListType rlt = new RecListType(rows[0],_dsh,_location);
			return rlt;
		}
		
		public RecList GetRecListById(int idRecList)
		{
			DataRow[] rows = _dsh.Tables["RecList"].Select("idRecList = " + idRecList);
			RecList rl = new RecList(rows[0],_dsh, _location);
			return rl;
		}

		public RecList[] GetRecLists()
		{
			DataRow[] rows = _dsh.Tables["RecList"].Select();
			RecList[] rls = new RecList[rows.Length];
			for(int i=0;i < rows.Length;i++)
				rls[i] = new RecList(rows[i], _dsh, _location);
			return rls;
		}

		public RecList[] GetRecListsByType(int idRecListType)
		{
			DataRow[] rows = _dsh.Tables["RecList"].Select("idRecListType = " + idRecListType);
			RecList[] rls = new RecList[rows.Length];
			for(int i = 0;i < rows.Length;i++)
				rls[i] = new RecList(rows[i], _dsh, _location);
			return rls;
		}

		public void Update(RecList rl)
		{
			UpdateHelper(rl);
			UpdateDataSet();
		}

		public void Update(RecList[] rls)
		{
			for(int i=0;i<rls.Length;i++)
				UpdateHelper(rls[i]);
			UpdateDataSet();
		}

		protected void UpdateHelper(RecList rl)
		{
			DataRow[] rows = _dsh.Tables["RecList"].Select("idRecList = " + rl.idRecList);
			DataRow row;
			bool bNew = rows.Length == 0;

			// Make sure that this is a new and/or unique combo of RecListType, StoreConcept, and ProductType
			DataRow[] rs = _dsh.Tables["RecList"].Select("idRecListType = " + rl.idRecListType + " and idStoreConcept = " + rl.idStoreConcept + " and idProductType = " + rl.idProductType);
			if(rs.Length > 0)
			{
				if(bNew || Convert.ToInt32(rs[0]["idRecList"]) != rl.idRecList)
					throw new CSWException("Reclist of type already exists");
			}

			// Make sure that each RecListTypeAttribute is present in this RecList according to it's RecListType
			RecListAttribute[] rlas;
			RecListType rlt = GetRecListTypeById(rl.idRecListType);
			//DataRow[] TypeAttrRows  = _dsh.Tables["RLTypeAttr"].Select("idRecListType = " + rl.idRecListType);
			if(rlt.RecListAttributes.Length > 0)
			{

				rlas = rl.RecListAttributes;
				RecListAttribute[] new_rlas = new RecListAttribute[rlt.RecListAttributes.Length];
				
				for(int i = 0; i < rlt.RecListAttributes.Length; i++)
				{
					bool bFound = false;
					new_rlas[i] = new RecListAttribute();
					if(null != rlas)
					{
						foreach(RecListAttribute rla in rlas)
						{
							if(rlt.RecListAttributes[i].idRecListTypeAttribute == rla.idRecListTypeAttribute ||
								(rla.TypeAttribute != null && 
								rlt.RecListAttributes[i].idRecListTypeAttribute == rla.TypeAttribute.idRecListTypeAttribute))
							{
								//if(rla.TypeAttribute == null)
								rla.idRecList = rl.idRecList;
									rla.TypeAttribute = rlt.RecListAttributes[i];
								new_rlas[i] = rla;
								bFound = true;
								break;
							}
						}
					}
					if(!bFound)
					{
						new_rlas[i].idRecList = rl.idRecList;
						new_rlas[i].TypeAttribute = rlt.RecListAttributes[i];
					}
				}
				rl.RecListAttributes = new_rlas;
			}

			rlas = rl.RecListAttributes;
			// Make sure that required attributes for this rec list are filled
			if(null != rlas)
			{
				foreach(RecListAttribute rla in rlas)
				{
					// TODO:  fix this
					string sField = rla.TypeAttribute.AttributeType.sField;
					switch(sField)
					{
						case "sValue":
							if(rla.sValue.Length > 255)
								throw new CSWException("sValue cannot exceed 255 characters");
							if(rla.TypeAttribute.bRequired && rla.sValue == null)
								throw new CSWException("sValue, a required attribute, cannot be null");
							break;
						case "txValue":
							if(rla.TypeAttribute.bRequired && rla.txValue == null)
								throw new CSWException("txValue, a required attribute, cannot be null");
							break;
						case "iValue":
							if(rla.TypeAttribute.bRequired && rla.iValue == null)
								throw new CSWException("iValue, a required attribute, cannot be null");
							break;
						default:
							break;
					}
				}
			}

			// Make sure that required attributes for this rec list's items are filled
			RecListItem[] rlis = rl.RecListItems;
			if(null != rlis)
			{
				foreach(RecListItem rli in rlis)
				{
					// Make sure that each RecListTypeItemAttribute is present in this RecListItem according to it's RecListType
					rli.idRecList = rl.idRecList;
					RecListItemAttribute[] rlias;
					//DataRow[] TypeItemAttrRows  = _dsh.Tables["RLItemTypeAttr"].Select("idRecListType = " + rl.idRecListType);
					if(rlt.RecListItemAttributes.Length > 0)
					{
						rlias = rli.RecListItemAttributes;
						RecListItemAttribute[] new_rlias = new RecListItemAttribute[rlt.RecListItemAttributes.Length];
				
						for(int i = 0; i < rlt.RecListItemAttributes.Length; i++)
						{
							bool bFound = false;
							new_rlias[i] = new RecListItemAttribute();
							if(null != rlias)
							{
								foreach(RecListItemAttribute rlia in rlias)
								{
									if(rlt.RecListItemAttributes[i].idRecListTypeItemAttribute == rlia.idRecListTypeItemAttribute ||
										(rlia.TypeItemAttribute != null && 
										rlt.RecListItemAttributes[i].idRecListTypeItemAttribute == rlia.TypeItemAttribute.idRecListTypeItemAttribute))
									{
										//if(rlia.TypeItemAttribute == null)
											rlia.TypeItemAttribute = rlt.RecListItemAttributes[i];
										rlia.idRecListItem = rli.idRecListItem;
										new_rlias[i] = rlia;

										bFound = true;
										break;
									}
								}
							}
							if(!bFound)
							{
								new_rlias[i].idRecListItem = rli.idRecListItem;
								new_rlias[i].TypeItemAttribute = rlt.RecListItemAttributes[i];
							}
						}
						rli.RecListItemAttributes = new_rlias;
					}

					rlias = rli.RecListItemAttributes;
					if(null != rlias)
					{
						foreach(RecListItemAttribute rlia in rlias)
						{
							// TODO:  fix this
							string sField = rlia.TypeItemAttribute.AttributeType.sField;
							switch(sField)
							{
								case "sValue":
									if(rlia.sValue.Length > 255)
										throw new CSWException("sValue cannot exceed 255 characters");
									if(rlia.TypeItemAttribute.bRequired && rlia.sValue == null)
										throw new CSWException("sValue, a required attribute, cannot be null");
									break;
								case "txValue":
									if(rlia.TypeItemAttribute.bRequired && rlia.txValue == null)
										throw new CSWException("txValue, a required attribute, cannot be null");
									break;
								case "iValue":
									if(rlia.TypeItemAttribute.bRequired && rlia.iValue == null)
										throw new CSWException("iValue, a required attribute, cannot be null");
									break;
								default:
									break;
							}
						}
					}
				}
			}

			row = (bNew) ? _dsh.Tables["RecList"].NewRow() : rows[0];
			rl.Fill(row,_dsh,bNew,this.Context, _location);
		}

		public void RemoveLists(int[] idRecLists)
		{
			foreach(int i in idRecLists)
			{
				DataRow r = _dsh.Tables["RecList"].Rows.Find(idRecLists[i]);
				RemoveHelper(r);
			}
			RemoveFromDataSet();
		}

		public void RemoveLists(int idRecListType, int idProductType, int idStoreConcept)
		{
			DataRow[] rows = _dsh.Tables["RecList"].Select("idRecListType = " + idRecListType + " AND idProductType = " + idProductType + " AND idStoreConcept = " + idStoreConcept);
			foreach(DataRow row in rows)
			{
				RemoveHelper(row);
			}
			RemoveFromDataSet();
		}

		protected void RemoveHelper(DataRow RecListRow)
		{
			DataRow[] ChildRows = RecListRow.GetChildRows("RL_RLA");
			foreach(DataRow row in ChildRows)
			{
				row.Delete();
			}
			ChildRows = RecListRow.GetChildRows("RL_RLI");
			foreach(DataRow row in ChildRows)
			{
				DataRow[] ItemChildRows = row.GetChildRows("RLI_RLIA");
				foreach(DataRow icrow in ItemChildRows)
				{
					icrow.Delete();
				}
				row.Delete();
			}
			RecListRow.Delete();
		}

	}

	public class RecListTypePersister : CSWObjectBase
	{
		protected bool _bUpdating;
		protected Location _location;
		protected DataSetHolder _dsh;

		public DataSetHolder DataSet { get { return _dsh; } }

		public RecListTypePersister(CSWContext ctx, bool bUpdating, Location location) : base(ctx)
		{
			_bUpdating = bUpdating;
			_location = location;
			_dsh = GetDataSet(bUpdating,location);
		}

		protected void UpdateDataSet()
		{
			_dsh.EnforceConstraints = false;
			_dsh.Update("RLType","idRecListType");
			_dsh.Update("RLTypeAttr","idRecListTypeAttribute");
			_dsh.Update("RLItemTypeAttr","idRecListTypeItemAttribute");
			_dsh.EnforceConstraints = true;
			// synch the staging and deployment databases
			Context.Config.MainDatabase.execSP("sprCSW_SyncStagingRecListTypes", null);
		}

		protected void RemoveFromDataSet()
		{
			_dsh.EnforceConstraints = false;
			_dsh.Update("RLItemTypeAttr","idRecListTypeItemAttribute");
			_dsh.Update("RLTypeAttr","idRecListTypeAttribute");
			_dsh.Update("RLType","idRecListType");
			_dsh.EnforceConstraints = true;
			// synch the staging and deployment databases
			Context.Config.MainDatabase.execSP("sprCSW_SyncStagingRecListTypes", null);
		}

		protected DataSetHolder GetDataSet(bool bUpdating, Location location)
		{
			Data.Connection conn = Context.Config.MainDatabase;
			DataSetHolder dsh = new DataSetHolder();

			dsh.FillTable(conn,"BaseType","select * from tblMMD_RecommendListBaseType");
			dsh.FillTable(conn,"AttrType", "select * from tblMMD_RecommendListAttributeType");
		
			if(location == Location.Staging)
			{
				dsh.AddTable(conn,"RLType","select * from tblMMD_RecommendListTypeStaging",bUpdating);
				dsh.AddTable(conn,"RLTypeAttr","select * from tblMMD_RecommendListTypeAttributeStaging",bUpdating);
				dsh.AddTable(conn,"RLItemTypeAttr","select * from tblMMD_RecommendListTypeItemAttributeStaging",bUpdating);
			}
			else
			{
				dsh.AddTable(conn,"RLType","select * from tblMMD_RecommendListType",bUpdating);
				dsh.AddTable(conn,"RLTypeAttr","select * from tblMMD_RecommendListTypeAttribute",bUpdating);
				dsh.AddTable(conn,"RLItemTypeAttr","select * from tblMMD_RecommendListTypeItemAttribute",bUpdating);
			}

			dsh.Relations.Add("RLT_RLTIA",dsh.Tables["RLType"].Columns["idRecListType"],dsh.Tables["RLItemTypeAttr"].Columns["idRecListType"]);
			dsh.Relations.Add("RLT_RLTA",dsh.Tables["RLType"].Columns["idRecListType"],dsh.Tables["RLTypeAttr"].Columns["idRecListType"]);

			return dsh;
		}

		public RecListType GetRecListTypeById(int idRecListType)
		{
			DataRow[] rows = _dsh.Tables["RLType"].Select("idRecListType = " + idRecListType);
			RecListType rlt = new RecListType(rows[0],_dsh,_location);
			return rlt;
		}

		public RecListType[] GetRecListTypes()
		{
			DataRow[] rows = _dsh.Tables["RLType"].Select();
			RecListType[] rlts = new RecListType[rows.Length];
			for(int i=0;i<rows.Length;i++)
				rlts[i] = new RecListType(rows[i],_dsh,_location);
			return rlts;
		}

		public void Update(RecListType rlt)
		{
			UpdateHelper(rlt);
			UpdateDataSet();
		}

		public void Update(RecListType[] rlts)
		{
			for(int i=0;i<rlts.Length;i++)
				UpdateHelper(rlts[i]);
			UpdateDataSet();
		}

		protected void UpdateHelper(RecListType rlt)
		{
			// Check that there are no RecLists of this RecListType before allowing an update/edit/etc
			if(RecListTypeInUse(rlt.idRecListType))
				throw new CSWException("Cannot update a RecListType that has RecLists of that type still active.");

			DataRow[] rows = _dsh.Tables["RLType"].Select("idRecListType = " + rlt.idRecListType);
			DataRow row;
			bool bNew = rows.Length == 0;
			row = (bNew) ? _dsh.Tables["RLType"].NewRow() : rows[0];
			rlt.Fill(row,_dsh,bNew,this.Context,_location);
		}

		public void RemoveTypes(int[] idRecListTypes)
		{
			foreach(int i in idRecListTypes)
			{
				// Check that there are no RecLists of this RecListType before deleting
				if(RecListTypeInUse(idRecListTypes[i]))
					throw new CSWException("Cannot Remove a RecListType that has RecLists of that type still active.");

				DataRow r = _dsh.Tables["RLType"].Rows.Find(idRecListTypes[i]);
				DataRow[] Attributes = r.GetChildRows("RLT_RLTA");
				foreach(DataRow row in Attributes)
				{
					row.Delete();
				}
				Attributes = r.GetChildRows("RLT_RLTIA");
				foreach(DataRow row in Attributes)
				{
					row.Delete();
				}
				r.Delete();
			}
			RemoveFromDataSet();
		}

		public bool RecListTypeInUse(int idRecListType)
		{
			DataRow[] rs = _dsh.Tables["RecList"].Select("idRecListType = " + idRecListType);
			return (rs.Length > 0);
		}
	}

	public class RecListAttributeType
	{
		public RecListAttributeType() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idRecListAttributeType;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		public string sField;

		public RecListAttributeType(DataRow row, DataSetHolder dsh, Location location)
		{
			idRecListAttributeType = Convert.ToInt32(row["idRecListAttributeType"]);
			sName = Convert.ToString(row["sName"]);
			sField = Convert.ToString(row["sField"]);
		}
	}

	public class RecListTypeItemAttribute
	{
		public RecListTypeItemAttribute() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idRecListTypeItemAttribute;

		[XmlAttribute(AttributeName="dbidRecListType")]
		public int idRecListType;

		[XmlAttribute(AttributeName="dbidRecListAttributeType")]
		public int idRecListAttributeType;

		[XmlAttribute(AttributeName="required")]
		public bool bRequired;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		[XmlElement("AttributeType",IsNullable=false)]
		public RecListAttributeType AttributeType;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;
/*
		[XmlElement(ElementName="Status",IsNullable=false)]
		public StagingStatus byStatus;
*/
		public RecListTypeItemAttribute(DataRow row, DataSetHolder dsh, Location location)
		{
			Initialize(row, dsh, location);
		}
		
		public void Initialize(DataRow row, DataSetHolder dsh, Location location)
		{
			idRecListTypeItemAttribute = Convert.ToInt32(row["idRecListTypeItemAttribute"]);
			idRecListType = Convert.ToInt32(row["idRecListType"]);
			idRecListAttributeType = Convert.ToInt32(row["idRecListAttributeType"]);
			bRequired = Convert.ToBoolean(row["bRequired"]);
			sName = Convert.ToString(row["sName"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
/*			if(Location.Staging == location)
			{
				byStatus = (StagingStatus)Convert.ToInt32(row["byStatus"]);
			}
*/			DataRow[] rows = dsh.Tables["AttrType"].Select("idRecListAttributeType = " + idRecListAttributeType);
			if(rows.Length > 0)
				AttributeType = new RecListAttributeType(rows[0], dsh, location);
		}

		public void Fill(DataRow row, DataSetHolder ds, bool bNew ,CSWContext ctx, Location location)
		{
			if(!bNew)
				row["idRecListType"] = idRecListType;
			if(null != AttributeType)
				row["idRecListAttributeType"] = AttributeType.idRecListAttributeType;
			row["bRequired"] = bRequired;
			row["sName"] = sName;
			row["dtChanged"] = DateTime.Now;
			row["sChangedBy"] = ctx.UserName;
/*			if(Location.Staging == location)
			{
				row["byStatus"] = byStatus;
			}
*/			row["dtDelta"] = DateTime.Now;
			row["sDeltaAction"] = (bNew) ? 'A' : 'C';
		}
	}

	public class RecListTypeAttribute
	{
		public RecListTypeAttribute() {}

		[XmlAttribute(AttributeName="dbid")]
		public int idRecListTypeAttribute;

		[XmlAttribute(AttributeName="dbidRecListType")]
		public int idRecListType;

		[XmlAttribute(AttributeName="dbidRecListAttributeType")]
		public int idRecListAttributeType;

		[XmlAttribute(AttributeName="required")]
		public bool bRequired;

		[XmlElement(ElementName="Name",DataType="normalizedString",IsNullable=false)]
		public string sName;

		[XmlElement("AttributeType",IsNullable=false)]
		public RecListAttributeType AttributeType;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

/*		[XmlElement(ElementName="Status",IsNullable=false)]
		public StagingStatus byStatus;
*/
		public RecListTypeAttribute(DataRow row, DataSetHolder dsh, Location location)
		{
			Initialize(row, dsh, location);
		}
		
		public void Initialize(DataRow row, DataSetHolder dsh, Location location)
		{
			idRecListTypeAttribute = Convert.ToInt32(row["idRecListTypeAttribute"]);
			idRecListType = Convert.ToInt32(row["idRecListType"]);
			idRecListAttributeType = Convert.ToInt32(row["idRecListAttributeType"]);
			bRequired = Convert.ToBoolean(row["bRequired"]);
			sName = Convert.ToString(row["sName"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
/*			if(Location.Staging == location)
			{
				byStatus = (StagingStatus)Convert.ToInt32(row["byStatus"]);
			}
*/			DataRow[] rows = dsh.Tables["AttrType"].Select("idRecListAttributeType = " + idRecListAttributeType);
			if(rows.Length > 0)
				AttributeType = new RecListAttributeType(rows[0], dsh, location);
		}

		public void Fill(DataRow row, DataSetHolder ds, bool bNew ,CSWContext ctx, Location location)
		{
			if(!bNew)
				row["idRecListType"] = idRecListType;
			if(null != AttributeType)
				row["idRecListAttributeType"] = AttributeType.idRecListAttributeType;
			row["bRequired"] = bRequired;
			row["sName"] = sName;
			row["dtChanged"] = DateTime.Now;
			row["sChangedBy"] = ctx.UserName;
/*			if(Location.Staging == location)
			{
				row["byStatus"] = byStatus;
			}
*/			row["dtDelta"] = DateTime.Now;
			row["sDeltaAction"] = (bNew) ? 'A' : 'C';
		}
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class RecListAttribute
	{
		public RecListAttribute() {}
		
		[XmlAttribute("dbid")]
		public int idRecListAttribute;

		[XmlAttribute(AttributeName="dbidRecList")]
		public int idRecList;

		[XmlAttribute("dbidRecListTypeAttribute")]
		public int idRecListTypeAttribute;

		[XmlElement("sValue",IsNullable=true)]
		public string sValue;

		[XmlElement("txValue",IsNullable=true)]
		public string txValue;

		[XmlElement("iValue",IsNullable=true)]
		public NullableInt iValue;

		[XmlElement("TypeAttribute",IsNullable=false)]
		public RecListTypeAttribute TypeAttribute;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

/*		[XmlElement(ElementName="Status",IsNullable=false)]
		public StagingStatus byStatus;
*/
		public RecListAttribute(DataRow row, DataSetHolder dsh, Location location)
		{
			Initialize(row, dsh, location);
		}
		
		public void Initialize(DataRow row, DataSetHolder dsh, Location location)
		{
			idRecListAttribute = Convert.ToInt32(row["idRecListAttribute"]);
			idRecList = Convert.ToInt32(row["idRecList"]);
			idRecListTypeAttribute = Convert.ToInt32(row["idRecListTypeAttribute"]);
			sValue = Convert.ToString(row["sValue"]);
			txValue = Convert.ToString(row["txValue"]);
			iValue = NullableInt.CreateInstance(row["iValue"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
/*			if(Location.Staging == location)
			{
				byStatus = (StagingStatus)Convert.ToInt32(row["byStatus"]);
			}
*/			DataRow[] rows = dsh.Tables["RLTypeAttr"].Select("idRecListTypeAttribute = " + idRecListTypeAttribute);
			if(rows.Length > 0)
				TypeAttribute = new RecListTypeAttribute(rows[0], dsh, location);
		}

		public void Fill(DataRow row, DataSetHolder ds, bool bNew ,CSWContext ctx, Location location)
		{
			if(!bNew)
				row["idRecList"] = idRecList;
			if(null != TypeAttribute)
				row["idRecListTypeAttribute"] = TypeAttribute.idRecListTypeAttribute;
			row["sValue"] = sValue;
			row["txValue"] = txValue;
			row["iValue"] = NullableInt.GetValue(iValue);
			row["dtChanged"] = DateTime.Now;
			row["sChangedBy"] = ctx.UserName;
/*			if(Location.Staging == location)
			{
				row["byStatus"] = byStatus;
			}
*/			row["dtDelta"] = DateTime.Now;
			row["sDeltaAction"] = (bNew) ? 'A' : 'C';
		}
	}

	[XmlRootAttribute(Namespace="urn:schemas-fullplaymedia-com:mmd", IsNullable=true)]
	public class RecListItemAttribute
	{
		public RecListItemAttribute() {}
		
		[XmlAttribute("dbid")]
		public int idRecListItemAttribute;

		[XmlAttribute(AttributeName="dbidRecListItem")]
		public int idRecListItem;

		[XmlAttribute("dbidRecListTypeItemAttribute")]
		public int idRecListTypeItemAttribute;

		[XmlElement("sValue",IsNullable=true)]
		public string sValue;

		[XmlElement("txValue",IsNullable=true)]
		public string txValue;

		[XmlElement("iValue", IsNullable=true)]
		public NullableInt iValue;

		[XmlElement("TypeItemAttribute",IsNullable=false)]
		public RecListTypeItemAttribute TypeItemAttribute;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

/*		[XmlElement(ElementName="Status",IsNullable=false)]
		public StagingStatus byStatus;
*/
		public RecListItemAttribute(DataRow row, DataSetHolder dsh, Location location)
		{
			Initialize(row, dsh, location);
		}
		
		public void Initialize(DataRow row, DataSetHolder dsh, Location location)
		{
			idRecListItemAttribute = Convert.ToInt32(row["idRecListItemAttribute"]);
			idRecListItem = Convert.ToInt32(row["idRecListItem"]);
			idRecListTypeItemAttribute = Convert.ToInt32(row["idRecListTypeItemAttribute"]);
			sValue = Convert.ToString(row["sValue"]);
			txValue = Convert.ToString(row["txValue"]);
			iValue = NullableInt.CreateInstance(row["iValue"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
/*			if(Location.Staging == location)
			{
				byStatus = (StagingStatus)Convert.ToInt32(row["byStatus"]);
			}
*/			DataRow[] rows = dsh.Tables["RLItemTypeAttr"].Select("idRecListTypeItemAttribute = " + idRecListTypeItemAttribute);
			if(rows.Length > 0)
				TypeItemAttribute = new RecListTypeItemAttribute(rows[0],dsh,location);
		}

		public void Fill(DataRow rowParent,DataRow row, DataSetHolder ds, bool bNew ,CSWContext ctx, Location location)
		{
			//if(!bNew)
				row["idRecListItem"] = rowParent["idRecListItem"];
			if(null != TypeItemAttribute)
				row["idRecListTypeItemAttribute"] = TypeItemAttribute.idRecListTypeItemAttribute;
			row["sValue"] = sValue;
			row["txValue"] = txValue;
			row["iValue"] = NullableInt.GetValue(iValue);
			row["dtChanged"] = DateTime.Now;
			row["sChangedBy"] = ctx.UserName;
/*			if(Location.Staging == location)
			{
				row["byStatus"] = byStatus;
			}
*/			row["dtDelta"] = DateTime.Now;
			row["sDeltaAction"] = (bNew) ? 'A' : 'C';
		}
	}
	
	public class RecListItem
	{
		public RecListItem() {}

		[XmlAttribute("dbid")]
		public int idRecListItem;

		[XmlAttribute("dbidRecList")]
		public int idRecList;

		[XmlElement("item",IsNullable=false)]
		public int idItem;

		[XmlElement("sequence",IsNullable=false)]
		public int iSeq;

		[XmlElement(ElementName="DateChanged",IsNullable=true)]
		public NullableDateTime dtChanged;

		[XmlElement(ElementName="ChangedBy",DataType="normalizedString",IsNullable=true)]
		public string sChangedBy;

/*		[XmlElement(ElementName="Status",IsNullable=false)]
		public StagingStatus byStatus;
*/
		[XmlArray("RecListItemAttributes",IsNullable=true)]
		public RecListItemAttribute[] RecListItemAttributes;

		public RecListItem(DataRow row, DataSetHolder dsh, Location location)
		{
			idRecListItem = Convert.ToInt32(row["idRecListItem"]);
			idRecList = Convert.ToInt32(row["idRecList"]);
			idItem = Convert.ToInt32(row["idItem"]);
			iSeq = Convert.ToInt32(row["iSeq"]);
			dtChanged = NullableDateTime.CreateInstance(row["dtChanged"]);
			sChangedBy = Convert.ToString(row["sChangedBy"]);
/*
			if(Location.Staging == location)
			{
				byStatus = (StagingStatus)Convert.ToInt32(row["byStatus"]);
			}
*/
			DataRow[] rows = row.GetChildRows("RLI_RLIA");
			RecListItemAttributes = new RecListItemAttribute[rows.Length];
			for(int i = 0;i < rows.Length;i++)
				RecListItemAttributes[i] = new RecListItemAttribute(rows[i], dsh, location);
		}

		public void Fill(DataRow rowParent, DataRow row, DataSetHolder dsh, bool bNew,CSWContext ctx, Location location)
		{
			//if(!bNew)
			row["idRecList"] = rowParent["idRecList"];
			row["idItem"] = idItem;
			row["iSeq"] = iSeq;
			row["dtChanged"] = DateTime.Now;
			row["sChangedBy"] = ctx.UserName;
/*
 * 			if(Location.Staging == location)
			{
				row["byStatus"] = byStatus;
			}
*/
			row["dtDelta"] = DateTime.Now;
			row["sDeltaAction"] = (bNew) ? 'A' : 'C';

			if(bNew) 
			{
				dsh.Tables["RLItem"].Rows.Add(row);
				row.SetParentRow(rowParent, dsh.Relations["RL_RLI"]);
			}

			if(RecListItemAttributes != null)
			{
				foreach(RecListItemAttribute rlia in RecListItemAttributes)
				{
					bool bNewRow;
					DataRow attrRow;
					if(0 == rlia.idRecListItemAttribute)
					{
						bNewRow = true;
						attrRow = dsh.Tables["RLItemAttr"].NewRow();
					}
					else
					{
						DataRow[] AttrRows = dsh.Tables["RLItemAttr"].Select("idRecListItemAttribute = " + rlia.idRecListItemAttribute);
						bNewRow = AttrRows.Length == 0;
						attrRow = (bNewRow) ? dsh.Tables["RLItemAttr"].NewRow() : AttrRows[0];
					}
					rlia.Fill(row,attrRow, dsh, bNewRow, ctx, location);
					if(bNewRow) dsh.Tables["RLItemAttr"].Rows.Add(attrRow);
					attrRow.SetParentRow(row, dsh.Relations["RLI_RLIA"]);
				}
			}
		}
	}
}
