using System;


using Fullplay.Lib;


using Maverick.Ctl;
using Maverick.Flow;

namespace Fullplay.Ctl
{
	/// <summary>
	/// Summary description for getDiscography.
	/// </summary>
	public class getDiscography : Command
	{
		public getDiscography() {}

		public override string Perform()
		{
		
			Browse FPBrowse = new Browse(this.Context);
			string sArtistName = Context.Request.QueryString["artistname"];
			model.Disco = FPBrowse.getDiscography(sArtistName);
			ControllerContext.Model = model; 
			return SUCCESS;
		
		}
	}
}
