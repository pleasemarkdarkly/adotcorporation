using System;

using MMD.Store.StoreLib;


namespace Fullplay.Lib
{
	/// <summary>
	/// Summary description for XslExtensions.
	/// </summary>
	public class XslExtension : WebObjectBase
	{
		//public XslExtension() {}
		public XslExtension(WebContext ctx) : base(ctx)
		{

		
		}
		public string UrlEncode(string S)
		{
			return Context.Server.UrlEncode(S);
		
		}

	}
}
