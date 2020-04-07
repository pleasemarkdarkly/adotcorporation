using System;
using System.Data;
using System.IO;
using System.Web;
using MMD.Central.CentralLib.Data;
using MMD.Central.CentralLib.Utilities;
using MMD.Central.CentralLib.XmlObjects;

namespace MMD.Central.CentralLib.Commands
{
	/// <summary>
	/// Summary description for RecList.
	/// </summary>
	public class RecList
	{
		public RecList()
		{
			//
			// TODO: Add constructor logic here
			//
		}
	}

	public class RecListAdd : CommandBase
	{
		public RecListAdd(CSWContext ctx) : base(ctx) {}

		int idRecListType;
		int idProductType;
		int idStoreConcept;

		public override void InitParameters()
		{
			// The first items in each of the corresponding tables is the default
			idRecListType = Context.Params.Int("idRecListType",0);
			idProductType = Context.Params.Int("idProductType",0);
			idStoreConcept = Context.Params.Int("idStoreConcept",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecListType", DbType.Int32, idRecListType);
			cParams.AddInput("@idProductType", DbType.Int32, idProductType);
			cParams.AddInput("@idStoreConcept", DbType.Int32, idStoreConcept);
			cParams.AddInput("@sChangedBy", DbType.String, Context.UserName, 100);
			cParams.Add("@idRecList",DbType.Int32,ParameterDirection.Output);
			cParams.Add("@iReturn", DbType.Int32, ParameterDirection.ReturnValue);
			cParams = Context.Config.MainDatabase.execSP_OutputParams("sprCSW_RecListAdd",cParams);

			int iReturn = Convert.ToInt32(cParams["@iReturn"].Value);
			Context.Params["idRecList"] = cParams["@idRecList"].Value.ToString();
			if(iReturn == 0)
			{
				Context.LoadListViewFromSession = false;
				Context.SetStatusMessage(StatusMsg.RecListAdded);
			}
			else
				Context.SetMessage(iReturn);
		}
	}


	public class RecListEdit : CommandBase
	{
		public RecListEdit(CSWContext ctx) : base(ctx) {}

		int idRecList;
		int idProductType;
		int idStoreConcept;
		//int idRecListOut;
		int iAttrCount;
		int[] idRecListAttribute;
		string[] sValue;
		XmlObjects.RecListAttribute[] _rlas;

		public override void InitParameters()
		{
			// The first items in each of the corresponding tables is the default
			idRecList = Context.Params.Int("idRecList",0);
			idProductType = Context.Params.Int("idProductType",0);
			idStoreConcept = Context.Params.Int("idStoreConcept",0);

			// get all the info for the extented attributes
			iAttrCount = Context.Params.Int("ixAttrCount",0);
			if(iAttrCount > 0)
			{
				XmlObjects.RecListPersister rlp = new RecListPersister(Context,false,Location.Staging);
				XmlObjects.RecList rl = rlp.GetRecListById(idRecList);
				XmlObjects.RecListAttribute[] rlas = rl.RecListAttributes;

				idRecListAttribute = new int[iAttrCount];
				sValue = new string[iAttrCount];
				_rlas = new RecListAttribute[iAttrCount];

				// check to see that we've got the right number of attributes?
				if(rlas.Length != iAttrCount)
					throw new CSWException("The number of Attributes given does not match the number expected");

				// grab all of the info from the context
				for(int i=0;i<iAttrCount;i++)
				{
					idRecListAttribute[i] = Context.Params.Int("ixAttr_" + i,0);
					sValue[i] = Context.Params.String("ixAttrValue_" + i,"");
					// make our own array of the attributes in the order we want
					_rlas[i] = null;
					foreach(XmlObjects.RecListAttribute rla in rlas)
					{
						if(rla.idRecListAttribute == idRecListAttribute[i])
							_rlas[i] = rla;
					}
				}
			}
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecList", DbType.Int32, idRecList);
			cParams.AddInput("@idProductType", DbType.Int32, idProductType);
			cParams.AddInput("@idStoreConcept", DbType.Int32, idStoreConcept);
			cParams.AddInput("@sChangedBy", DbType.String, Context.UserName, 100);
			cParams.Add("@idRecListOut",DbType.Int32,ParameterDirection.Output);
			cParams.Add("@iReturn", DbType.Int32, ParameterDirection.ReturnValue);
			cParams = Context.Config.MainDatabase.execSP_OutputParams("sprCSW_RecListEdit",cParams);

			int iReturn = Convert.ToInt32(cParams["@iReturn"].Value);
			Context.Params["idRecList"] = cParams["@idRecListOut"].Value.ToString();
			if(iReturn == 0)
			{
				//Context.TargetPage = "/CentralWeb/Profiles/RecListItemView.aspx";
				Context.LoadListViewFromSession = false;
				Context.SetStatusMessage(StatusMsg.RecListEdited); // need new status message.  Recommend List Attributes Updated Successfully.

				// update all of the list attributes as well
				if(iAttrCount > 0)
				{					
					for(int i=0;i<iAttrCount;i++)
					{
						if(_rlas[i] != null)
						{
							Parameters spParams = new Parameters();
							spParams.AddInput("@idRecListAttribute", DbType.Int32, idRecListAttribute[i]);

							spParams.AddInput("@sValue", DbType.String, null, 255);
							spParams.AddInput("@txValue", SqlDbType.Text, null);
							spParams.AddInput("@iValue", DbType.Int32, null);

							string sField = _rlas[i].TypeAttribute.AttributeType.sField;

							spParams["@" + sField].Value = (sField == "iValue") ? (object)Convert.ToInt32(sValue[i]) : (object)sValue[i];

							spParams.AddInput("@sChangedBy", DbType.String, Context.UserName, 100);
							Context.Config.MainDatabase.execSP("sprCSW_RecListAttributeEdit", spParams);
						}
						else
							Context.SetMessage(iReturn); // need an error status message id here?
					}
				}
			}
			else
				Context.SetMessage(iReturn);
		}
	}

	public class RecListDelete : CommandBase
	{
		public RecListDelete(CSWContext ctx) : base(ctx) {}

		int idRecList;

		public override void InitParameters()
		{
			idRecList = Context.Params.Int("idRecList",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecList",DbType.Int32, idRecList);
			Context.Config.MainDatabase.execSP("sprCSW_RecListDelete",cParams);
			Context.SetStatusMessage(StatusMsg.RecListDeleted);
			Context.LoadListViewFromSession = false;
		}
	}

	public class RecListUndo : CommandBase
	{
		public RecListUndo(CSWContext ctx) : base(ctx) {}

		int idRecList;

		public override void InitParameters()
		{
			idRecList = Context.Params.Int("idRecList",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecList", DbType.Int32, idRecList);
			Context.Config.MainDatabase.execSP("sprCSW_RecListUndo", cParams);
			Context.SetStatusMessage(StatusMsg.RecListReverted);
			Context.LoadListViewFromSession = false;
		}
	}

	public class RecListUndoAll : CommandBase
	{
		public RecListUndoAll(CSWContext ctx) : base(ctx) {}

		int idRecListBaseType;

		public override void InitParameters()
		{
			idRecListBaseType = Context.Params.Int("idRecListBaseType",1);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecListBaseType", DbType.Int32, idRecListBaseType);
			Context.Config.MainDatabase.execSP("sprCSW_RecListUndoAll", cParams);
			Context.SetStatusMessage(StatusMsg.RecListReverted);
			Context.LoadListViewFromSession = false;
		}
	}

	public class RecListSubmit : CommandBase
	{
		public RecListSubmit(CSWContext ctx) : base(ctx) {}

		int idRecList;

		public override void InitParameters()
		{
			idRecList = Context.Params.Int("idRecList",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecList", DbType.Int32, idRecList);
			cParams.Add("@idRecListItem",DbType.Int32,ParameterDirection.Output);
			cParams.Add("@iReturn",DbType.Int32, ParameterDirection.ReturnValue);
			Context.Config.MainDatabase.execSP("sprCSW_RecListSubmit", cParams);
		
			int iReturn = int.Parse(cParams["@iReturn"].Value.ToString());
			if(iReturn == 0)
			{
				Context.SetStatusMessage(StatusMsg.RecListSubmit);
				Context.LoadListViewFromSession = false;
			}
			else if(iReturn == 2010) // A required attribute field was empty
			{
				Context.SetMessage(iReturn,idRecList.ToString());
			}
			else if(iReturn == 2011) // A required item attribute field was empty
			{
				Context.SetMessage(iReturn,idRecList,cParams["@idRecListItem"].Value.ToString());
			}
			else
			{
				Context.SetMessage(iReturn);
			}
		}
	}

	public class RecListSubmitAll : CommandBase
	{
		public RecListSubmitAll(CSWContext ctx) : base(ctx) {}

		int idRecListBaseType;

		public override void InitParameters()
		{
			idRecListBaseType = Context.Params.Int("idRecListBaseType",1);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecListBaseType", DbType.Int32, idRecListBaseType);
			cParams.Add("@idRecList",DbType.Int32,ParameterDirection.Output);
			cParams.Add("@idRecListItem",DbType.Int32,ParameterDirection.Output);
			cParams.Add("@iReturn",DbType.Int32, ParameterDirection.ReturnValue);
			Context.Config.MainDatabase.execSP("sprCSW_RecListSubmitAll", cParams);

			int iReturn = int.Parse(cParams["@iReturn"].Value.ToString());
			if(iReturn == 0)
			{
				Context.SetStatusMessage(StatusMsg.RecListSubmitAll);
				Context.LoadListViewFromSession = false;
			}
			else if(iReturn == 2010) // A required attribute field was empty
			{
				Context.SetMessage(iReturn,cParams["@idRecList"].Value.ToString());
			}
			else if(iReturn == 2011) // A required item attribute field was empty
			{
				Context.SetMessage(iReturn,cParams["@idRecList"].Value.ToString(),cParams["@idRecListItem"].Value.ToString());
			}
			else
			{
				Context.SetMessage(iReturn);
			}
		}
	}

	public class RecListItemInsert : CommandBase
	{
		public RecListItemInsert(CSWContext ctx): base(ctx) {}

		int idRecListItem;
		int idRLIInsertBefore;

		public override void InitParameters()
		{
			idRecListItem = int.Parse(Context.Params["idRecListItem"]);

			string sValue = ListView.ListView.FindFirstCheckboxValue(Context);
			if(sValue == null)
				throw new CSWCommandException((int)ErrMsg.RecListItemInsertItemNotChecked);

			idRLIInsertBefore = Convert.ToInt32(sValue);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecListItem", DbType.Int32, idRecListItem);
			cParams.AddInput("@idRLIInsertBefore", DbType.Int32, idRLIInsertBefore);
			Context.Config.MainDatabase.execSP("sprCSW_RecListItemInsert",cParams);
			Context.SetStatusMessage(StatusMsg.RecListItemInserted);
		}
	}

	public class RecListItemAdd : CommandBase
	{
		public RecListItemAdd(CSWContext ctx) : base(ctx) {}
		
		int idItem;
		int idRecList;
		int idRLIInsertBefore;

		public override void InitParameters()
		{
			idItem = Context.Params.Int("idItem",0);
			idRecList = Context.Params.Int("idRecList",0);
			idRLIInsertBefore = -1;

			string sValue;
			//find the first checkbox in the listview
			int cRecCount = Context.Params.Int("lv_RecCount",0);
			for(int i=1;i<=cRecCount;i++)
			{
				sValue = Context.Params["lv_Check_" + i];
				if(sValue != null && sValue != "")
				{
					idRLIInsertBefore = int.Parse(sValue);
					return;
				}
			}
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecList",DbType.Int32, idRecList);
			cParams.AddInput("@idItem", DbType.Int32, idItem);
			object o = null;
			if(idRLIInsertBefore != -1)	o = idRLIInsertBefore;
			cParams.AddInput("@idRLIInsertBefore", DbType.Int32, o);
			cParams.AddInput("@sChangedBy", DbType.String, Context.UserName,100);
			cParams.Add("@idRecListItem",DbType.Int32,ParameterDirection.Output);
			cParams.Add("@iReturn",DbType.Int32, ParameterDirection.ReturnValue);
			Context.Config.MainDatabase.execSP("sprCSW_RecListItemAdd",cParams);

			Context.Params["idRecListItem"] = cParams["@idRecListItem"].Value.ToString();
			int iReturn = int.Parse(cParams["@iReturn"].Value.ToString());
			if(iReturn == 0)
				Context.SetStatusMessage(StatusMsg.RecListItemAdded);
			else
				Context.SetMessage(iReturn);
		}
	}

	public class RecListItemDelete : CommandBase
	{
		public RecListItemDelete(CSWContext ctx) : base(ctx) {}
		int idRecListItem;

		public override void InitParameters()
		{
			idRecListItem = Context.Params.Int("idRecListItem",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecListItem",DbType.Int32, idRecListItem);
			Context.Config.MainDatabase.execSP("sprCSW_RecListItemDelete",cParams);
			Context.SetStatusMessage(StatusMsg.RecListItemDeleted);
		}
	}

	public class RecListItemEdit : CommandBase
	{
		public RecListItemEdit(CSWContext ctx) : base(ctx) {}

		int idRecListItem;
		int idRecList;
		int idItem;
		int iSeq;
		int iAttrCount;
		int[] idRecListItemAttribute;
		string[] sValue;
		XmlObjects.RecListItem _rli;
		XmlObjects.RecListItemAttribute[] _rlias;

		public override void InitParameters()
		{
			// The first items in each of the corresponding tables is the default
			idRecListItem = Context.Params.Int("idRecListItem",0);
			idRecList = Context.Params.Int("idRecList",0);
			idItem = Context.Params.Int("idItem",0);
			iSeq = Context.Params.Int("iSeq",0);

			// get all the info for the extented attributes
			iAttrCount = Context.Params.Int("ixAttrCount",0);
			if(iAttrCount > 0)
			{
				XmlObjects.RecListPersister rlp = new RecListPersister(Context,false,Location.Staging);
				XmlObjects.RecList rl = rlp.GetRecListById(idRecList);
				XmlObjects.RecListItem[] rlis = rl.RecListItems;

				// get the rec list item that we want
				foreach(RecListItem rli in rlis)
				{
					if(rli.idRecListItem == idRecListItem)
						_rli = rli;
				}

				idRecListItemAttribute = new int[iAttrCount];
				sValue = new string[iAttrCount];
				_rlias = new RecListItemAttribute[iAttrCount];

				// get the extended attributes of this rec list item
				XmlObjects.RecListItemAttribute[] rlias = _rli.RecListItemAttributes;

				// check to see that we've got the right number of item attributes?
				if(rlias.Length != iAttrCount)
					throw new CSWException("The number of Item Attributes given does not match the number expected");

				// grab all of the info from the context
				for(int i=0;i<iAttrCount;i++)
				{
					idRecListItemAttribute[i] = Context.Params.Int("ixAttr_" + i,0);
					sValue[i] = Context.Params.String("ixAttrValue_" + i,"");
					// make our own array of the items attributes in the order we want
					_rlias[i] = null;
					foreach(XmlObjects.RecListItemAttribute rlia in rlias)
					{
						if(rlia.idRecListItemAttribute == idRecListItemAttribute[i])
							_rlias[i] = rlia;
					}
				}
			}
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecListItem", DbType.Int32, idRecListItem);
			cParams.AddInput("@idItem", DbType.Int32, idItem);
			cParams.AddInput("@iSeq", DbType.Int32, iSeq);
			cParams.AddInput("@sChangedBy", DbType.String, Context.UserName, 100);
			cParams.Add("@iReturn", DbType.Int32, ParameterDirection.ReturnValue);
			cParams = Context.Config.MainDatabase.execSP_OutputParams("sprCSW_RecListItemEdit",cParams);

			int iReturn = Convert.ToInt32(cParams["@iReturn"].Value);
			if(iReturn == 0)
			{
				Context.Params["idRecListItem"] = idRecListItem.ToString();
				Context.Params["idRecList"] = idRecList.ToString();
				Context.LoadListViewFromSession = false;
				Context.SetStatusMessage(StatusMsg.RecListItemEdited); // need new status message.  Recommend List Item Attributes Updated Successfully.

				// update all of the list attributes as well
				if(iAttrCount > 0)
				{					
					for(int i=0;i<iAttrCount;i++)
					{
						if(_rlias[i] != null)
						{
							Parameters spParams = new Parameters();
							spParams.AddInput("@idRecListItemAttribute", DbType.Int32, idRecListItemAttribute[i]);

							spParams.AddInput("@sValue", DbType.String, null, 255);
							spParams.AddInput("@txValue", SqlDbType.Text, null);
							spParams.AddInput("@iValue", DbType.Int32, null);

							string sField = _rlias[i].TypeItemAttribute.AttributeType.sField;

							spParams["@" + sField].Value = (sField == "iValue") ? (object)Convert.ToInt32(sValue[i]) : (object)sValue[i];

							spParams.AddInput("@sChangedBy", DbType.String, Context.UserName, 100);
							Context.Config.MainDatabase.execSP("sprCSW_RecListItemAttributeEdit", spParams);
						}
						else
							Context.SetMessage(iReturn); // need an error status message id here?
					}
				}
			}
			else
				Context.SetMessage(iReturn);
		}
	}

	public class RecListTypeAdd : CommandBase
	{
		public RecListTypeAdd(CSWContext ctx) : base(ctx) {}

		//int idRecListType;
		int idRecListBaseType;
		string sName;

		public override void InitParameters()
		{
			idRecListBaseType = Context.Params.Int("idRecListBaseType",0);
			sName = Context.Params.String("sName", "untitled");
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecListBaseType", DbType.Int32, idRecListBaseType);
			cParams.AddInput("@sName", DbType.String, sName, 25);
			cParams.AddInput("@sChangedBy", DbType.String, Context.UserName,100);
			cParams.Add("@idRecListType",DbType.Int32,ParameterDirection.Output);
			cParams.Add("@iReturn", DbType.Int32, ParameterDirection.ReturnValue);
			cParams = Context.Config.MainDatabase.execSP_OutputParams("sprCSW_RecListTypeAdd",cParams);

			int iReturn = Convert.ToInt32(cParams["@iReturn"].Value);
			if(iReturn == 0)
			{
				Context.LoadListViewFromSession = false;
				Context.SetStatusMessage(StatusMsg.RecListTypeAdded);
				Context.Params["idRecListType"] = cParams["@idRecListType"].Value.ToString();
				//Context.Params.SessionInt("idRecListType"); // example of how to sessionize a variable
			}
			else
				Context.SetMessage(iReturn);
		}
	}

	public class RecListTypeDelete : CommandBase
	{
		public RecListTypeDelete(CSWContext ctx) : base(ctx) {}

		int idRecListType;

		public override void InitParameters()
		{
			idRecListType = Context.Params.Int("idRecListType",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecListType",DbType.Int32, idRecListType);
			Context.Config.MainDatabase.execSP("sprCSW_RecListTypeDelete",cParams);
			Context.SetStatusMessage(StatusMsg.RecListTypeDeleted);
			Context.LoadListViewFromSession = false;
		}
	}

	public class RecListTypeUndo : CommandBase
	{
		public RecListTypeUndo(CSWContext ctx) : base(ctx) {}

		int idRecListType;

		public override void InitParameters()
		{
			idRecListType = Context.Params.Int("idRecListType",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecListType",DbType.Int32, idRecListType);
			Context.Config.MainDatabase.execSP("sprCSW_RecListTypeUndo",cParams);
			Context.SetStatusMessage(StatusMsg.RecListTypeReverted);
			Context.LoadListViewFromSession = false;
		}
	}

	public class RecListTypeSubmit : CommandBase
	{
		public RecListTypeSubmit(CSWContext ctx) : base(ctx) {}

		int idRecListType;

		public override void InitParameters()
		{
			idRecListType = Context.Params.Int("idRecListType",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecListType", DbType.Int32, idRecListType);
			Context.Config.MainDatabase.execSP("sprCSW_RecListTypeSubmit", cParams);
			Context.SetStatusMessage(StatusMsg.RecListTypeSubmit);
			Context.LoadListViewFromSession = false;
		}
	}

	public class RecListTypeSubmitAll : CommandBase
	{
		public RecListTypeSubmitAll(CSWContext ctx) : base(ctx) {}

		public override void Execute()
		{
			Context.Config.MainDatabase.execSP("sprCSW_RecListTypeSubmitAll",null);
			Context.SetStatusMessage(StatusMsg.RecListTypeSubmitAll);
			Context.LoadListViewFromSession = false;
		}
	}

	public class RecListTypeAttributeAdd : CommandBase
	{
		public RecListTypeAttributeAdd(CSWContext ctx) : base(ctx) {}

		//int idRecListTypeAttribute;
		int idRecListType;
		int idRecListAttributeType;
		string sName;
		bool bRequired;


		public override void InitParameters()
		{
			idRecListType = Context.Params.Int("idRecListType",0);
			idRecListAttributeType = Context.Params.Int("idRecListAttributeType",0);
			sName = Context.Params.String("sName", "untitled");
			bRequired = 1 == Context.Params.Int("bRequired",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecListType", DbType.Int32, idRecListType);
			cParams.AddInput("@idRecListAttributeType", DbType.Int32, idRecListAttributeType);
			cParams.AddInput("@sName", DbType.String, sName, 25);
			cParams.AddInput("@bRequired", DbType.Boolean, bRequired);
			cParams.Add("@idRecListTypeAttribute",DbType.Int32,ParameterDirection.Output);
			cParams.Add("@iReturn", DbType.Int32, ParameterDirection.ReturnValue);
			cParams = Context.Config.MainDatabase.execSP_OutputParams("sprCSW_RecListTypeAttributeAdd",cParams);

			int iReturn = Convert.ToInt32(cParams["@iReturn"].Value);
			if(iReturn == 0)
			{
				Context.LoadListViewFromSession = false;
				Context.SetStatusMessage(StatusMsg.RecListTypeAttributeAdded);
			}
			else
				Context.SetMessage(iReturn);
		}
	}

	public class RecListTypeAttributeDelete : CommandBase
	{
		public RecListTypeAttributeDelete(CSWContext ctx) : base(ctx) {}

		int idRecListTypeAttribute;

		public override void InitParameters()
		{
			idRecListTypeAttribute = Context.Params.Int("idRecListTypeAttribute",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecListTypeAttribute",DbType.Int32, idRecListTypeAttribute);
			Context.Config.MainDatabase.execSP("sprCSW_RecListTypeAttributeDelete",cParams);
			Context.SetStatusMessage(StatusMsg.RecListTypeAttributeDeleted);
			Context.LoadListViewFromSession = false;
		}
	}

	public class RecListTypeItemAttributeAdd : CommandBase
	{
		public RecListTypeItemAttributeAdd(CSWContext ctx) : base(ctx) {}

		//int idRecListTypeItemAttribute;
		int idRecListType;
		int idRecListAttributeType;
		string sName;
		bool bRequired;


		public override void InitParameters()
		{
			idRecListType = Context.Params.Int("idRecListType",0);
			idRecListAttributeType = Context.Params.Int("idRecListAttributeType",0);
			sName = Context.Params.String("sName", "untitled");
			bRequired = 1 == Context.Params.Int("bRequired",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecListType", DbType.Int32, idRecListType);
			cParams.AddInput("@idRecListAttributeType", DbType.Int32, idRecListAttributeType);
			cParams.AddInput("@sName", DbType.String, sName, 25);
			cParams.AddInput("@bRequired", DbType.Boolean, bRequired);
			cParams.Add("@idRecListTypeItemAttribute",DbType.Int32,ParameterDirection.Output);
			cParams.Add("@iReturn", DbType.Int32, ParameterDirection.ReturnValue);
			cParams = Context.Config.MainDatabase.execSP_OutputParams("sprCSW_RecListTypeItemAttributeAdd",cParams);

			int iReturn = Convert.ToInt32(cParams["@iReturn"].Value);
			if(iReturn == 0)
			{
				Context.LoadListViewFromSession = false;
				Context.SetStatusMessage(StatusMsg.RecListTypeItemAttributeAdded);
			}
			else
				Context.SetMessage(iReturn);
		}
	}

	public class RecListTypeItemAttributeDelete : CommandBase
	{
		public RecListTypeItemAttributeDelete(CSWContext ctx) : base(ctx) {}

		int idRecListTypeItemAttribute;

		public override void InitParameters()
		{
			idRecListTypeItemAttribute = Context.Params.Int("idRecListTypeItemAttribute",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecListTypeItemAttribute",DbType.Int32, idRecListTypeItemAttribute);
			Context.Config.MainDatabase.execSP("sprCSW_RecListTypeItemAttributeDelete",cParams);
			Context.SetStatusMessage(StatusMsg.RecListTypeItemAttributeDeleted);
			Context.LoadListViewFromSession = false;
		}
	}


	public class RecListItemMove : CommandBase
	{
		public RecListItemMove(CSWContext ctx) : base(ctx) {}
		
		int idRecListItem;
		int up;

		public override void InitParameters()
		{
			idRecListItem = Context.Params.Int("idRecListItem",0);
			up = Context.Params.Int("up",0);
		}

		public override void Execute() 
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idRecListItem",DbType.Int32, idRecListItem);
			cParams.AddInput("@up", DbType.Int32, up);
			cParams.AddInput("@sChangedBy", DbType.String, Context.UserName, 100);
			Context.Config.MainDatabase.execSP("sprCSW_RecListItemMove",cParams);
			if(up == 1)
				Context.SetStatusMessage(StatusMsg.RecListItemMovedUp);
			else
				Context.SetStatusMessage(StatusMsg.RecListItemMovedDown);
		}
	}
}
