<%@ Page language="c#" Codebehind="OK.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Common.Dialog.OK" %>
<html>
<head>
<title>Alert</title>
<link rel="STYLESHEET" type="text/css" href="/CentralWeb/Common/CentralWeb.css"></link>
<script SRC="/CentralWeb/Common/CentralWeb.js"></script>
<script LANGUAGE="Javascript">

document.title = window.dialogArguments[0]; // does not remember if set in onload

var sTitle = "Alert";

function ProcessArgs(arArgs)
{
   document.getElementById("msg").innerHTML = arArgs[1];
   sTitle = arArgs[0];
}

function DoClose(iRes)
{
   window.returnValue = iRes;
   event.returnValue = false;
   window.close();
}


</script>
</head>
<body  margintop="0" marginleft="0">
<table cellpadding="0" CELLSPACING="0" width="100%">
<tr><td><div style="width: 5px; height: 5px"></div></td></tr>
<tr><td ALIGN="CENTER"><div class="AlertMsg" ID="msg"></div></td></tr>
<tr><td><div style="width: 5px; height: 10px"></div></td></tr>
<tr><td ALIGN="center"><a class="AlertLink" href="#" onclick="DoClose(1); return false;">Okay</a></td></tr>
</table>

<script LANGUAGE="Javascript">
	 ProcessArgs(window.dialogArguments);
</script>
</body>
</html>
