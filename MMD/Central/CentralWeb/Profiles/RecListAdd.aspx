<%@ Page language="c#" Codebehind="RecListAdd.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Profiles.RecListAdd" %>
<%@ Import NameSpace="MMD.Central.CentralLib.XmlObjects" %>
<% CSWContext.PageTitle = "Manage Recommend Lists > Create New Recommend List"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<% RecListType[] rlts = GetRecListTypes(); %>

<% foreach(RecListType rlt in rlts) { %>

<span class="SmallText"><b><a href="/CentralWeb/Profiles/RecListEdit.aspx?cmd=RecListAdd&idRecListType=<%=rlt.idRecListType%>">Create New [<%=rlt.sName%>] List</a></b></span>
<br><br>

<% } %>

<div class="BVT" onclick="ToggleBVT()">
<div class="BVTHeader"><%=CSWContext.PageTitle%>&nbsp;BVT</div>
<div class="BVTText">
Functional links:<br>
<ul>
<li>TODO:  Fill in this description.</li>
</ul>
</div>
</div>
      
<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>

