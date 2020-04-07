using System;


using Fullplay.Lib;


using Maverick.Ctl;
using Maverick.Flow;

namespace Fullplay.Ctl
{
	/// <summary>
	/// Summary description for getGames.
	/// </summary>
	public class getGames : Command
	{
		public getGames() {}
		public override string Perform()
		{
			//get games by genre
			Browse FPBrowse = new Browse(this.Context);
			model.Games = FPBrowse.getGames(int.Parse(Context.Request.QueryString["idGenre"]), int.Parse(Context.Request.QueryString["index"]));
			return SUCCESS;

		}
	}
}
