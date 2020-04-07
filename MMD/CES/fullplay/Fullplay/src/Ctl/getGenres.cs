using System;


using Fullplay.Lib;


using Maverick.Ctl;
using Maverick.Flow;


namespace Fullplay.Ctl
{
	/// <summary>
	/// Summary description for getGenres.
	/// </summary>
	public class getGenres : Command
	{
		public getGenres(){}

		public override string Perform()
		{

			Browse FPBrowse = new Browse(this.Context);
			int mediaid = int.Parse(Context.Request.QueryString["mediaid"]);
			this.model.Genres = FPBrowse.getGenres(mediaid);
			ControllerContext.Model = this.model;
			
			switch (mediaid)
			{
				case 1: return SUCCESS;
				case 10: return "game";
				case 13: return "video";
			
			
			}

			return SUCCESS;
		}
	}
}
