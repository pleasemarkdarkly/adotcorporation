using System;
using System.Data;
using System.IO;
using System.Web;
using MMD.Central.CentralLib.Data;
using MMD.Central.CentralLib.Utilities;
using MMD.Central.CentralLib.XmlObjects;

namespace MMD.Central.CentralLib.Commands
{

	public class FeatureRequestUpdate : CommandBase
	{
		public FeatureRequestUpdate(CSWContext ctx) : base(ctx) {}

		int idFeatureRequest;
		int idItem;
		//string sXref;
		string sAuthor;
		string txFeature;

		public override void InitParameters()
		{
			idFeatureRequest = Context.Params.Int("idFeatureRequest",0);
			idItem = Context.Params.Int("idItem",0);
			sAuthor = Context.Params["sAuthor"];
			txFeature = Context.Params["txFeature"];
		}

		public override void Execute()
		{
			ProductValidator pv = new ProductValidator(Context);
			if(!pv.IsValidItemId(idItem))
				throw new CSWCommandException((int)ErrMsg.InvalidItemId);

			Parameters cParams = new Parameters();
			cParams.Add("@idFeatureRequest",DbType.Int32, ParameterDirection.InputOutput, idFeatureRequest);
			cParams.AddInput("@idItem", DbType.Int32, idItem);
			cParams.AddInput("@sAuthor", DbType.String, sAuthor, 100);
			cParams.AddInput("@txFeature", SqlDbType.Text, txFeature);
			cParams.AddInput("@sChangedBy", DbType.String, Context.UserName, 100);
			cParams = Context.Config.MainDatabase.execSP_OutputParams("sprCSW_FeatureRequestUpdate",cParams);
			if(idFeatureRequest == 0)
			{
				Context.SetStatusMessage(StatusMsg.FeatureRequestAdded);
				Context.Params["idFeatureRequest"] = cParams["@idFeatureRequest"].Value.ToString();
			}
			else
				Context.SetStatusMessage(StatusMsg.FeatureRequestUpdated);
		}
	}

	public class FeatureRequestRemove : CommandBase
	{
		public FeatureRequestRemove(CSWContext ctx) : base(ctx) { }

		int idFeatureRequest;

		public override void InitParameters()
		{
			idFeatureRequest = Context.Params.Int("idFeatureRequest",0);
		}

		public override void Execute()
		{
			Parameters cParams = new Parameters();
			cParams.AddInput("@idFeatureRequest", DbType.Int32, idFeatureRequest);
			Context.Config.MainDatabase.execSP("sprCSW_FeatureRequestRemove", cParams);
			Context.SetStatusMessage(StatusMsg.FeatureRequestRemoved);
		}
	}

}
