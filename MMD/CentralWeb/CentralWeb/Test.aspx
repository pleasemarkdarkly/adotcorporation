<%@ Page language="c#" Codebehind="Test.aspx.cs" AutoEventWireup="false" Inherits="MMDCentral.WFTest" %>
<% //CSWContext.PageTitle = "Test"; %>
<% Server.Execute("/MMDCentral/Common/Header.aspx"); %>

	<% Server.Execute("TestInner.aspx"); %>
	<br>
	
	<i><%=CSWContext.Params["test"]%></i>

<% Server.Execute("/MMDCentral/Common/Footer.aspx"); %>