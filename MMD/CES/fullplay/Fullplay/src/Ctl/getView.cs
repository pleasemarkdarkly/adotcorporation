using System;
using Maverick.Ctl;
using Maverick.Flow;



namespace Fullplay.Ctl
{
	/// <summary>
	/// Summary description for getView.
	/// </summary>
	public class getView : Command
	{
		public getView() {}

		public override string Perform()
		{
			
			ControllerContext.Model = this.model;
			string view = ControllerContext.HttpContext.Request.QueryString["view"];
			return (view!=null)?view:DEFAULT;

		}
	}
}
