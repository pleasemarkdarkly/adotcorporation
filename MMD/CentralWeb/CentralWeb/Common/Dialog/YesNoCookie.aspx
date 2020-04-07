<%@ Page language="c#" Codebehind="YesNoCookie.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Common.Dialog.YesNoCookie" %>
<html>
<head>
<title>Confirm</title>
<link rel="STYLESHEET" type="text/css" href="/CentralWeb/Common/CentralWeb.css"></link>
<script SRC="/CentralWeb/Common/CentralWeb.js"></script>
<script LANGUAGE="Javascript">

document.title = window.dialogArguments[0]; // does not remember if set in onload
window.returnValue = 0;

var sTitle = "Alert";
function ProcessArgs(arArgs)
{
   document.getElementById("msg").innerHTML = arArgs[1];
   document.getElementById("Yes").innerHTML = arArgs[2];
   document.getElementById("No").innerHTML = arArgs[3];
}

function DoClose(iRes)
{
   if(iRes == 1 && document.getElementById("Checked").checked)
		iRes = 2;
   
   window.returnValue = iRes;
   event.returnValue = false;
   window.close();
}

</script>
</head>
<body  margintop="0" marginleft="0">
<table cellpadding="0" CELLSPACING="0" width="100%">
<tr><td><div style="width: 5px; height: 5px"></div></td></tr>
<tr><td ALIGN="CENTER">
<div class="AlertMsg" ID="msg"></div></td></tr>
<tr><td><div style="width: 5px; height: 10px"></div></td></tr>


<tr><td ALIGN="center">
<a id="Yes" class="AlertLink" href="#" onclick="DoClose(1); return false;">Yes</a>
&nbsp;&nbsp;&nbsp;&nbsp; 
<a id="No" class="AlertLink" href="#" onclick="DoClose(0); return false;">No</a>
</td></tr>
<tr><td><div style="width: 5px; height: 10px"></div></td></tr>
<tr><td align="left" style="padding-left: 5mm; ">
	<input type="checkbox" id="Checked"><label for="Checked">Disable this dialog in the future.</label>
</td></tr>
</table>
<script LANGUAGE="Javascript">
	 ProcessArgs(window.dialogArguments);
</script>
</body>
</html>