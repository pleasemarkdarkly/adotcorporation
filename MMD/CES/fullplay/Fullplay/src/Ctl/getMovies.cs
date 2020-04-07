using System;


using Fullplay.Lib;


using Maverick.Ctl;
using Maverick.Flow;

namespace Fullplay.Ctl
{
	/// <summary>
	/// Summary description for getGames.
	/// </summary>
	public class getMovies : Command
	{
		public getMovies() {}
		public override string Perform()
		{
			//get games by genre
			Browse FPBrowse = new Browse(this.Context);
			//model.Games = FPBrowse.getGames(int.Parse(Context.Request.QueryString["idGenre"]));
			return SUCCESS;

		}
	}
}
