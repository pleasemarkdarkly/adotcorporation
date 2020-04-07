using System;


using Fullplay.Lib;


using Maverick.Ctl;
using Maverick.Flow;


namespace Fullplay.Ctl
{
	/// <summary>
	/// Summary description for getArtists.
	/// </summary>
	public class getArtists :Command
	{

		int idGenre;
		int idMediaType;
		int index;
		public getArtists() 
		{
		
		}

		public override string Perform()
		{
			Browse FPBrowse = new Browse(this.Context);
			
			
			
			try 
			{
				idGenre = int.Parse(Context.Request.QueryString["idgenre"]);
				idMediaType = int.Parse(Context.Request.QueryString["idmediatype"]);
				index = int.Parse(Context.Request.QueryString["index"]);


			}
			catch (Exception)
			{
				return ERROR;

			}
	
			model.Artists = FPBrowse.getArtists(idGenre,idMediaType, index);
			ControllerContext.Model = model;

			return SUCCESS;
		}
	}
}
