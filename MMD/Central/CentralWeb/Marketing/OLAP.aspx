<%@ Page language="c#" Codebehind="OLAP.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Marketing.OLAP" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0 Transitional//EN" > 

<html>
  <head>
    <title>OLAP</title>
    <meta name="GENERATOR" Content="Microsoft Visual Studio 7.0">
    <meta name="CODE_LANGUAGE" Content="C#">
    <meta name=vs_defaultClientScript content="JavaScript">
    <meta name=vs_targetSchema content="http://schemas.microsoft.com/intellisense/ie5">
  </head>
  <body MS_POSITIONING="GridLayout">
	
	<form action="OLAP.aspx" method="post">
		<input type="hidden" name="post" value="1">
		Catalog:&nbsp;<input type="text" name="Catalog" value="CSW"><br>
		Query:&nbsp;<textarea name="MDXQuery" rows=10 cols=60>
		select non empty
{ [Week Time].[Week].Members} on columns,
{ Hierarchize(  {[Product Type].[Media Type].Members, [Product Type].[Genre].Members } ) } on rows
from [Product Scans]
		
		</textarea>
		<br>
		<input type="submit" value="Submit">
	</form>
	
  </body>
</html>
