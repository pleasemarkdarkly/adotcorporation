<%@ Page language="c#" Codebehind="Form.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Common.Dialog.Form" %>
<html>
<head>
<title>Form</title>
<link rel="STYLESHEET" type="text/css" href="/CentralWeb/Common/CentralWeb.css"></link>
<script SRC="/CentralWeb/Common/CentralWeb.js"></script>
<script LANGUAGE="Javascript">

document.title = window.dialogArguments[0]; // does not remember if set in onload
window.returnValue = 0;

var sTitle = "Form";
function ProcessArgs(arArgs)
{
   document.getElementById("msg").innerHTML = arArgs[1];
   document.getElementById("divFrm").innerHTML = arArgs[2];
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
<tr><td ALIGN="CENTER"><span class="AlertMsg" ID="msg"></span></td></tr>
<tr><td><div style="width: 5px; height: 10px"></div></td></tr>

<tr><td ALIGN="center">
<DIV ID="divFrm">Form should be here.
</DIV>
</td></tr>

</table>
<script LANGUAGE="Javascript">
	 ProcessArgs(window.dialogArguments);
</script>
</body>
</html>
