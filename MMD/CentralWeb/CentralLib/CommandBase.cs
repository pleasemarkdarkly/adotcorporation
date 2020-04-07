using System;

namespace MMD.Central.CentralLib
{
	public class CSWCommandException : CSWException 
	{
		public CSWCommandException() : this("") {}
		public CSWCommandException(string sMessage) : base(sMessage) {}
		public CSWCommandException(int iError) : this(iError,"") {}
		public CSWCommandException(int iError, string sMessage) : base(iError, sMessage) {}
	}

	public class CommandBase : CSWObjectBase, ICommand
	{
		public CommandBase(CSWContext ctx) : base(ctx) 
		{
			//most commands update the system, so prep list views to load from session.
			Context.LoadListViewFromSession = false;					
		}

		public virtual void InitParameters() {}
		public virtual void Authenticate() { }
		public virtual void Execute() {}
	}

	interface ICommand
	{
		void InitParameters(); 
		void Authenticate();
		void Execute();
	}
}
