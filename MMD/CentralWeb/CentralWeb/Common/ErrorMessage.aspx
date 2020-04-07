<%@ Page language="c#" Codebehind="ErrorMessage.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Common.ErrorMessage" %>

<% if(CSWContext.ErrMsg != 0) { %>
<span class="ErrMsg">ERROR: [<%=(int)CSWContext.ErrMsg%>]&nbsp;<%=CSWContext.ErrorMessage%></span><br>
<% } %>
<% if(CSWContext.StatusMsg != 0) { %>
<span class="StatusMsg">STATUS: [<%=(int)CSWContext.StatusMsg%>]&nbsp;<%=CSWContext.StatusMessage%></span><br>
<% } %>
<% else { %>
<span class="StatusMsg">&nbsp;</span><br>
<% } %>
