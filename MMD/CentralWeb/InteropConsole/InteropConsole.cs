using System;
using System.Configuration;
using System.Xml;
using System.IO;
using System.Collections;
using System.Reflection;
using Interop;

//[assembly: log4net.Config.DOMConfigurator(ConfigFileExtension="config")]

namespace Interop
	                                                              
{
	class InteropConsole
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		/// 

		//log4net logging class.
		private static readonly log4net.ILog log = log4net.LogManager.GetLogger(System.Reflection.MethodBase.GetCurrentMethod().DeclaringType);

		[STAThread]
		static void Main(string[] args)
		{
			XmlNode xmld = (XmlNode)ConfigurationSettings.GetConfig("Interop");


			XmlNodeList xmlnTasks = xmld.SelectNodes("./taskList/task");
			
			foreach(XmlNode xmlnTask in xmlnTasks)
			{
				XmlNode  xmlnAction = xmlnTask.SelectSingleNode("./trigger");
				string ActionType = xmlnAction.Attributes.GetNamedItem("type").InnerText;
				object [] Params = new Object[1];
				Params[0] = xmlnTask;
				TriggerHandler trigger = (TriggerHandler)Activator.CreateInstance(Type.GetType(ActionType),Params);
				trigger.SetTrigger();
			}
			while(true) {
				System.Threading.Thread.Sleep(1000);
			}
		}
	}
}
