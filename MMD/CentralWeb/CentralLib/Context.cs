using System;
using System.Diagnostics;
using System.Data;
using System.Data.SqlClient;
using System.Reflection;
using System.Web;
using System.Web.UI;
using System.Web.SessionState;
using System.Web.Services;
using System.Xml;
using MMD.Central.CentralLib.XmlObjects;
using MMD.Central.CentralLib.ListView;

namespace MMD.Central.CentralLib
{
	public class AuthenticateException : ApplicationException
	{
		public AuthenticateException() : base("Authentication Exception")	{}
	}


	public class CSWException : ApplicationException
	{
		private int _lError;
		public CSWException(string sMessage) : this(0,sMessage,null) {}
		
		public CSWException(int lError, string sMessage) : this(lError,sMessage,null)
		{
		}

		public CSWException(int lError, string sMessage, string sSource) : base(sMessage)
		{
			_lError = lError;
			if(sSource != null)
				this.Source = sSource;
		}

		public int Error 
		{ 
			get { return _lError; }
			set { _lError = value; }
		}
	}

	public abstract class CSWObjectBase
	{
		CSWContext _context;

		public CSWObjectBase(CSWContext context)
		{
			System.Diagnostics.Debug.Assert( context != null );
			_context = context;
		}

		public CSWContext Context { get { return _context; } }
	}


	public class CSWParameters : CSWObjectBase
	{
		System.Collections.Specialized.NameValueCollection _coll;

		public CSWParameters(CSWContext ctx) : base(ctx) 
		{
			_coll = new System.Collections.Specialized.NameValueCollection();
		}
		
		public string this[string sName]
		{
			get
			{
				string s = _coll[sName];
				if(s != null)
					return s;
				else
					return this.Context.Request.Params[sName];
			}
			set
			{
				_coll[sName] = value;		
			}
		}

		public int Int(string sName, int iDefault)
		{
			try
			{
				return int.Parse(this[sName]);
			}
			catch
			{
				return iDefault;
			}
		}

		public string String(string sName, string sDefault)
		{
			string sValue = this[sName];
			return (sValue != null) ? sValue : sDefault;
		}

		public int SessionInt(string sName)
		{
			return SessionInt(sName,0);
		}

		public int SessionInt(string sName, int iDefault)
		{
			int iValue = Int(sName,-1);
			if(iValue == -1)
			{
				if(Context.Session[sName] == null)
				{
					iValue = iDefault;
					Context.Session[sName] = iValue;
				}
				else
					iValue = (int)Context.Session[sName];
				this[sName] = iValue.ToString();
			}
			else
				Context.Session[sName] = iValue;
			return iValue;
		}

		public string SessionString(string sName)
		{	
			string sValue = this[sName];
			if(sValue == null)
			{
				sValue = Context.Session[sName].ToString();
				this[sName] = sValue;
			}
			else
				Context.Session[sName] = sValue;
			return sValue;
		}
	}

	public enum AppSection
	{
		Common = 0,
		Admin,
		Advertisement,
		Content,
		Marketing,
		Profiles
	}

/*	Moved into Lookups.cs
	public class CSWLookups : CSWObjectBase
	{
		public CSWLookups(CSWContext ctx) : base(ctx) {}

		LookupTable _ltStoreConcept;
		public LookupTable StoreConcept
		{
			get 
			{
				if(_ltStoreConcept == null)
					_ltStoreConcept = new LookupTable(Context.Config.MainDatabase,"select idStoreConcept, sName from tblMMD_StoreConcept order by sName");
				return _ltStoreConcept;
			}
		}

		LookupTable _ltFileMediaType;
		public LookupTable FileMediaType
		{
			get 
			{
				if(_ltFileMediaType == null)
					_ltFileMediaType = new LookupTable(MMD.Central.CentralLib.XmlObjects.FileMediaType.Image.GetType());
				return _ltFileMediaType;
			}
		}

		LookupTable _ltMediaType;
		public LookupTable MediaType
		{
			get
			{
				if(_ltMediaType == null)
					_ltMediaType = new LookupTable(Context.Config.MainDatabase,"select idMediaType, sName from tblMMD_MetaDataMediaType where bUseInDevice = 1 order by sName");
				return _ltMediaType;
			}
		}

		LookupTable _ltProductType;
		public LookupTable ProductType
		{
			get
			{
				if(_ltProductType == null)
					_ltProductType = new LookupTable(Context.Config.MainDatabase,"select idProductType, sName from tblMMD_MetaDataProductType order by sName");
				return _ltProductType;
			}
		}

		LookupTable _ltYesNo;
		public LookupTable YesNo
		{
			get 
			{
				if(_ltYesNo == null)
					_ltYesNo = new LookupTable("1,Yes,0,No", ",");
				return _ltYesNo;
			}
		}

		LookupTable _ltEventType;
		public LookupTable EventType
		{
			get 
			{
				if(_ltEventType == null)
					_ltEventType = new LookupTable("0,Periodic,1,Daily,2,Run Once", ",");
				return _ltEventType;
			}
		}

		LookupTable _ltEventStatus;
		public LookupTable EventStatus
		{
			get 
			{
				if(_ltEventStatus == null)
					_ltEventStatus = new LookupTable("0,Disabled,1,Ready,2,Running", ",");
				return _ltEventStatus;
			}
		}

		public LookupTable RecListFiles(int idStoreConcept)
		{
			return new LookupTable(Context.Config.MainDatabase,"select idFile, sName + ' (' + sFileName + ')' from tblMMD_File where idStoreConcept in (0, " + idStoreConcept + " ) and iType = 2 order by sName");
		}
	}
*/

	public class CSWContext
	{
		const string HTTPCONTEXT_KEY_CONTEXT = "_context";

		System.Web.UI.Page _page = null;
		WebService _service = null;
		HttpContext _context = null;
		CSWParameters _params = null;
		string _sTargetPage = null;
		
		CSWConfig _config;
		CSWLog _log;
		CSWLookups _lookups;

		AppSection _eAppSection;
		string _sPageTitle = "";
		string _sNavTreeTag = "";
		bool _bLoadListViewFromSession = true;
		string _sErrMsg = "";
		ErrMsg _eErrMsg = 0;
		string _sStatusMsg = "";
		StatusMsg _eStatusMsg = 0;
		bool _bShowNavPane = true;

		public bool LoadListViewFromSession 
		{
			get { return _bLoadListViewFromSession; }
			set { _bLoadListViewFromSession = value; }
		}

		public bool ShowNavPane { get { return _bShowNavPane; } set { _bShowNavPane = value; } }


		public string ErrorMessage { get { return _sErrMsg; } }
		public ErrMsg ErrMsg { get { return _eErrMsg; } }
		public string StatusMessage { get { return _sStatusMsg; } }
		public StatusMsg StatusMsg { get { return _eStatusMsg; } }

		public void SetMessage( int iError, params object[] args)
		{
			if(iError > (int)ErrMsg.Min && iError < (int)ErrMsg.Max)
				SetErrorMessage((ErrMsg)iError,args);
			else if(iError > (int)StatusMsg.Min && iError < (int)StatusMsg.Max)
				SetStatusMessage((StatusMsg)iError,args);
			else
			{
				string sMessage = "Unknown Error [" + iError + "] ";
				foreach(object o in args)
					sMessage += "(" + o.ToString() + ") ";
				SetErrorMessage(sMessage);
			}
		}

		public void SetErrorMessage( string sErrorMessage )
		{
			_eErrMsg = ErrMsg.Unknown;
			_sErrMsg = sErrorMessage;
		}

		public void SetStatusMessage( string sMessage )
		{
			_eStatusMsg = StatusMsg.Unknown;
			_sStatusMsg = sMessage;
		}

		public void SetErrorMessage( ErrMsg eErrorMessage, params object[] args )
		{
			_eErrMsg = eErrorMessage;
			_sErrMsg = BuildMessage((int)eErrorMessage, "ErrMsg." + eErrorMessage.ToString(), args);
		}

		public void SetStatusMessage( StatusMsg eStatusMessage, params object[] args )
		{
			_eStatusMsg = eStatusMessage;
			_sStatusMsg = BuildMessage((int) eStatusMessage, "StatusMsg." + eStatusMessage.ToString(), args);
		}


		private string BuildMessage( int idMessage, string sMessageId, params object[] args )
		{
			string sMessage;
			
			Data.NamedFieldReader nfr = Config.MainDatabase.execSQL_NFR("SELECT * FROM tblCSW_Message WHERE idMessage = " + idMessage);
			try
			{
				if(nfr.Read())
				{
					string sDBMsg = nfr.GetString("sMessage");
					try
					{
						sMessage = String.Format(sDBMsg,args);
					}
					catch
					{
						sMessage = sDBMsg;
					}
				}
				else //message not found
				{
					sMessage = "Unknown Error: " + sMessageId + " [" + idMessage + "]" ;
					if(args.Length > 0)
					{
						sMessage += " ( ";
						for(int i=0;i<args.Length;i++)
						{
							sMessage += args[i].ToString();
							if(i < (args.Length - 1) )
								sMessage += " , ";
						}
						sMessage += " ) ";
					}
				}
			}
			finally
			{
				nfr.Close();
			}
			return sMessage;
		}

		public string PageTitle
		{
			get { return _sPageTitle; }
			set { _sPageTitle = value; }
		}

		public string NavTreeTag
		{
			get { return _sNavTreeTag; }
			set { _sNavTreeTag = value; }
		}
		
		/// <summary>
		/// CSWContext for Central Web
		/// </summary>
		/// <param name="page"></param>
		/// <param name="context"></param>
		/// <returns></returns>
		public static CSWContext Get(System.Web.UI.Page page, HttpContext context)
		{
			//page.ErrorPage = "/CentralWeb/Common/Error.aspx";

			if(page == context.Handler)
			{
				CSWContext ctx = new CSWContext(page, context);
				ctx.HttpContext.Items[HTTPCONTEXT_KEY_CONTEXT] = ctx;
				ctx.ExecuteContext();
				return ctx;
			}
			else
			{
				CSWContext ctx = (CSWContext)(context.Items[HTTPCONTEXT_KEY_CONTEXT]);
				System.Diagnostics.Debug.Assert(ctx != null);
				return ctx;
			}
		}

		/// <summary>
		/// CSWContext instantiator for MMDWebService
		/// </summary>
		/// <param name="service"></param>
		/// <returns></returns>
		public static CSWContext Get(WebService service)
		{
			CSWContext ctx = new CSWContext(service);
			return ctx;
		}

		/// <summary>
		/// CSWContext instantiator for MMD Store only
		/// </summary>
		/// <param name="context"></param>
		/// <returns></returns>
		public static CSWContext Get(HttpContext context)
		{
			CSWContext ctx = new CSWContext(context);
			return ctx;
		}

		private CSWContext(System.Web.UI.Page page, HttpContext context)
		{
			_page = page;
			_context = context;

			_eAppSection = GetAppSection();

			_params = new CSWParameters( this );
			_config = new CSWConfig( this );
			_log = new CSWLog( this );
			_lookups = new CSWLookups( this );	
		}

		private CSWContext(WebService service)
		{
			_service = service;
			_context = service.Context;

			_config = new CSWConfig( this );
			_log = new CSWLog( this );
			_lookups = new CSWLookups( this );	

		}

		private CSWContext(HttpContext context)
		{
			_context = context;

			_config = new CSWConfig( this );
			_log = new CSWLog( this );
			_lookups = new CSWLookups( this );
		}


		//[Conditional("Trace")]
		public void Trace(string sFormat, params object[] args)
		{
			string s = String.Format(sFormat,args);
			System.Diagnostics.Trace.WriteLine(s);
			Response.Write("<!-- TRACE: ");
			Response.Write(s);
			Response.Write(" -->");

			//Response.Write("<div style=\"color: green\">");
			//Response.Write(s);
			//Response.Write("</div>");
		}



		public System.Web.UI.Page Page { get { return _page; } }
		public HttpApplicationState Application { get { return _context.Application; } }
		public HttpSessionState Session { get { return _context.Session; } }
		public HttpRequest Request { get { return _context.Request; } }
		public HttpResponse Response { get { return _page.Response; } }
		public HttpServerUtility Server { get { return _context.Server; } }
		public HttpContext HttpContext { get { return _context; } }
		public CSWParameters Params { get { return _params; } }
		public CSWConfig Config { get { return _config; } }
		public CSWLog Log { get { return _log; } }
		public CSWLookups Lookups { get { return _lookups; } }
		public AppSection AppSection { get { return _eAppSection; } }

		public string TargetPage
		{
			get { return _sTargetPage; }
			set { _sTargetPage = value; }
		}

		private AppSection GetAppSection()
		{
			string sAppSection = _page.TemplateSourceDirectory;
			sAppSection = sAppSection.Substring(sAppSection.IndexOf('/',1) + 1);

			switch(sAppSection)
			{
				case "Admin": return AppSection.Admin;
				case "Advertisement": return AppSection.Advertisement;
				case "Content" : return AppSection.Content;
				case "Marketing" : return AppSection.Marketing;
				case "Profiles" : return AppSection.Profiles;
				default: return 0;
			}
		}

		private string _sUserName;

		public string UserName 
		{ 
			get 
			{ 
				if(_sUserName == null)
				{
					_sUserName = Request.ServerVariables["AUTH_USER"]; 
					if(_sUserName == "" || _sUserName == null)
						_sUserName = "Anonymous";
				}
				return _sUserName;
			} 
			set { _sUserName = value; }
		}
		
		[Conditional("NTLM")]
		private void AuthenticateUser()
		{
			if(!Request.IsAuthenticated)
			{
				SetErrorMessage(ErrMsg.UserNotAuthenticated,UserName);
				Server.Transfer("/CentralWeb/Common/Error.aspx");
			}
		}
		
		private void ExecuteContext()
		{
			if(_page == _context.Handler)
			{
				AuthenticateUser();
		
				TargetPage = Request.FilePath;

				DispatchCommand();
				
				if(TargetPage != Request.FilePath)
					Server.Transfer(TargetPage);
			}
		}

		private void DispatchCommand()
		{
			if(Request["cmd"] != null)
			{
				//TODO - support other assemblies at some point.  
				//Maybe have a configuration file, or scan directory.
				string sCommand = Request["cmd"];
				string sTypeName = CommandType(sCommand);

				try
				{
					ExecuteCommand(sCommand);
				}
				catch(CSWCommandException e)
				{
					SetErrorMessage((ErrMsg)e.Error, e.Message);
				}
				catch(CSWException e)
				{
					if(e.Error == (int)ErrMsg.CommandNotFound)
						SetErrorMessage(ErrMsg.CommandNotFound, sTypeName);
					else
						SetErrorMessage(String.Format("CSW Exception ({0}):[{1}] {2} at {3}",sTypeName, e.Error, e.Message, e.Source));
				}
				catch(Exception e)
				{
					SetErrorMessage( String.Format("Exception ({0}): {1} at {2}",sTypeName,e.Message,e.Source) );
				}

			}
		}

		private string CommandType(string sCommand)
		{
			return "MMD.Central.CentralLib.Commands." + sCommand;
		}

		public void ExecuteCommand(string sCommand)
		{
			//TODO - support other assemblies at some point.  
			//Maybe have a configuration file, or scan directory.
			Assembly assembly = Assembly.GetCallingAssembly();
			ICommand cmd = null;
			string sTypeName = CommandType(sCommand);

			try
			{
				object oCmd = assembly.CreateInstance(sTypeName, 
					true, 
					BindingFlags.CreateInstance | BindingFlags.Instance | BindingFlags.Public, 
					null, 
					new object[] { this }, 
					null, 
					null);
				if(oCmd == null)
					throw new CSWException( (int)ErrMsg.CommandNotFound , sTypeName );

				cmd = (ICommand) oCmd;
				cmd.InitParameters();
				cmd.Authenticate();
				cmd.Execute();

				// log error message/status message
				if(ErrMsg != 0)	Log.Error((int)ErrMsg,null,ErrorMessage,sTypeName,null);
				if(StatusMsg != 0) Log.Error((int)StatusMsg,null,StatusMessage,sTypeName,null);
			}
			catch(CSWCommandException e)
			{
				Log.Error(e.Error,e.Source,this.ErrorMessage,sTypeName,e.StackTrace);
				throw e;
			}
			catch(CSWException e)
			{
				Log.Error(e.Error,e.Source,e.Message,sTypeName,e.StackTrace);
				throw e;
			}
			catch(Exception e)
			{
				Log.Error((int)ErrMsg.Unknown,e.Source,e.Message,sTypeName,e.StackTrace);
				throw e;
			}
		}
	}

	public class CSWConfigurationSectionHandler : System.Configuration.IConfigurationSectionHandler 
	{
		public CSWConfigurationSectionHandler() {}

		public object Create(object parent, object configContext, XmlNode section)
		{
			return section;
		}
	}

	public class CSWConfig : CSWObjectBase
	{
		XmlNode m_xmld;
		//XmlDocument m_xmld;
		public CSWConfig(CSWContext ctx) : base(ctx) 
		{
			m_xmld = (XmlNode)System.Configuration.ConfigurationSettings.GetConfig("CSWConfig");			
		}

		Data.Connection _connDeploy;
		Data.Connection _connStaging;
		Data.Connection _connLog;

		public Data.Connection MainDatabase { get { return this.StagingDatabase; } }

		public Data.Connection DeployDatabase
		{
			get
			{
				if(_connDeploy == null)
					_connDeploy = new Data.Connection(m_xmld.SelectSingleNode("//database[@type='deploy']").InnerText);
				return _connDeploy;
			}
		}

		public Data.Connection StagingDatabase
		{
			get
			{
				if(_connStaging == null)
					_connStaging = new Data.Connection(m_xmld.SelectSingleNode("//database[@type='staging']").InnerText);
				return _connStaging;
			}
		}

		public Data.Connection LogDatabase
		{
			get
			{
				if(_connLog == null)
					_connLog = new Data.Connection(m_xmld.SelectSingleNode("//database[@type='log']").InnerText );
				return _connLog;
			}
		}


		public string GetWebPath(int idStoreConcept, MMDPostedFileType fileType)
		{
			XmlElement xmle = (XmlElement)m_xmld.SelectSingleNode("//web/concept[@id='" + idStoreConcept + "']/" + fileType + "/virtualdir");
			return xmle.InnerText.Trim();
		}

		public string GetWebPath(int idStoreConcept,MMDPostedFileType fileType, string sFileName)
		{
			return GetWebPath(idStoreConcept,fileType) + "/" + sFileName;
		}

		public string GetLocalPath(int idStoreConcept, MMDPostedFileType fileType)
		{
			return Context.Server.MapPath(GetWebPath(idStoreConcept, fileType));
		}

		public string GetLocalPath(int idStoreConcept, MMDPostedFileType fileType, string sFileName)
		{
			return Context.Server.MapPath(GetWebPath(idStoreConcept,fileType,sFileName));
		}


		

	}

	public class CSWLog : CSWObjectBase
	{
		public CSWLog(CSWContext ctx) : base(ctx) { }

		
		/*
		 *	iError int
		 *  sSource varchar 500 - Class.Method call generating error (NULL allowed)
		 * sMessage varchar 2000 - description of error
		 * sCommand varchar 100 - Command that generated error (NULL allowed)
		 * sContext varchar 2000 - stack trace, if appropriate (NULL allowed)
		 */

		public void Error(int iError, string sSource, string sMessage, string sCommand, string sContext)
		{
			Data.Parameters cParams = new Data.Parameters();
			cParams.AddInput("@iError", DbType.Int32, iError);
			cParams.AddInput("@sSource", DbType.String, sSource, 500);
			cParams.AddInput("@sMessage", DbType.String, sMessage, 2000);
			cParams.AddInput("@sCommand", DbType.String, sCommand, 100);
			cParams.AddInput("@sContext", DbType.String, sContext, 2000);
			cParams.AddInput("@sUser", DbType.String, Context.UserName, 100);
			Context.Config.MainDatabase.execSP("sprCSW_LogError",cParams);
		}

	}


}
