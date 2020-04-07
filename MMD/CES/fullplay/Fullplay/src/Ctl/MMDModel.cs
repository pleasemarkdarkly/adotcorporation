using System;
using System.Xml;



using MMD.Store.StoreLib;

using Maverick.Ctl;
using Maverick.Flow;



namespace Fullplay.Ctl
{
	/// <summary>
	/// Summary description for MMDModel.
	/// </summary>
	public class MMDModel : Command 
	{

		WebContext ctx;
		IControllerContext cctx;

		XmlDocument genres;
		XmlDocument artists;
		XmlDocument disco;
		XmlDocument games;
		XmlDocument item;
		XmlDocument reclist;
		XmlDocument movies;
		int mediaid;
		int reclistid;
		int productid;



		public MMDModel() {}

		public MMDModel(WebContext ctx, Maverick.Flow.IControllerContext cctx)
		{
			this.ctx = ctx;
			this.cctx = cctx;
			
			Search = null;


		}
		public int ReclistID {get {return reclistid; } set {reclistid = value;}}
		public int ProductID {get {return productid; } set {productid = value;}}
		public int MediaID {get {return mediaid; } set {mediaid = value;}}
		public XmlDocument Search;
		public XmlDocument Games {get {return games; } set {games = value;}}
		public XmlDocument Movies {get {return movies; } set {movies = value;}}

		public XmlDocument Artists {get {return artists; } set {artists = value;}}
		public XmlDocument Genres {get {return genres; } set {genres = value;}}
		public XmlDocument Disco {get {return disco; } set {disco = value;}}
		public XmlDocument Item {get {return item; } set {item = value;}}
		public XmlDocument Reclist {get {return reclist;} set {reclist = value;}}
	}
}
