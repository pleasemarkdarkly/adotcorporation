using System;
using System.Xml.Xsl;
using Maverick.Ctl;
using Maverick.Flow;
using MMD.Store.StoreLib;
using Fullplay.Lib;



namespace Fullplay.Ctl


{
	/// <summary>
	/// Summary description for Command.
	/// </summary>
	public class Command : Throwaway
	{
		protected WebContext Context;
		protected MMDModel model;


		public const string DEFAULT = "default";


		public Command() {}

		public virtual string Perform()
		{
			return SUCCESS;

		}

		public override string  Go()
		{
				
			Context = new WebContext(ControllerContext.HttpContext);
			model = new MMDModel(Context, ControllerContext);
			XsltArgumentList xslArg = new XsltArgumentList();

			XslExtension xsl = new XslExtension(this.Context);
			xslArg.AddExtensionObject("xslext", xsl);
			Context.Context.Items.Add("xslArgumentList", xslArg);


			return Perform();
		}

	}
}
