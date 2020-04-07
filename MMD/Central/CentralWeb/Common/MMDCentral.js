var fNs = navigator.appName == "Netscape";
var fIE = navigator.appName == "Microsoft Internet Explorer";
var fWin = navigator.platform == "Win32";
var ver = navigator.appVersion;
var f4x = ver.charAt(0) == "4" || ver.charAt(0) == "5" || ver.charAt(0) == "6"
var fMSN = fIE && ver.indexOf("MSN") >= 0;
var fAOL = fIE && ver.indexOf("AOL") >= 0;

function ltrim ( s )
{
	return s.replace( /^\s*/, "" )
}

function rtrim ( s )
{
	return s.replace( /\s*$/, "" );
}

function trim ( s )
{
	return rtrim(ltrim(s));
}


function IsSelectable(ele)
{
	return ele.type == "text" || ele.type == "password" || ele.type == "textarea";
}

function IsHidden(ele)
{
	return ele.type == "hidden";
}


function ValidateFormInput(frm)
{
	return ValidateMiscInput(frm, "set");
}

function ValidateSearchInput(frm)
{
	return ValidateMiscInput(frm, "cond");
}

function GetEleValue(ele)
{
	if (!ele) return "";
	if (ele.sampleon == "1") return "";
	return ele.value;
}

function ValidateMiscInput(frm, sPrefix)
{
	var rgele = frm.elements;
	var ele, eleValid, eleLabel;
	var valMain;
	var iValue;
	var uLongValue;
	var sValue;
	var dtValue;
	var tmValue;
	var dttmValue;
	for (var i = 0; i < frm.length; i++) 
		{
		eleValid = rgele[i];
		if (eleValid.name.substring(0,8) == "required" && eleValid.value != "0")  // 0 is not req
			{
			if (sPrefix == "") ele = frm.elements[sPrefix + eleValid.name.substring(9)];
			else ele = frm.elements[sPrefix + eleValid.name.substring(8)];
			eleLabel = frm.elements["label" + eleValid.name.substring(8)];
			//NiceAlert("req atrib on field " + eleLabel.value);

			if (ele == null)
				{
				NiceAlert("could not find " + sPrefix + eleValid.name.substring(8));
				continue;
				}
			sValue = new String(GetEleValue(ele));
			sValue = sValue.replace(/(^\s*)|(\s*$)/g, "");
			if (sValue == "")
				{
				return WarnValue(ele, eleLabel.value, "filled in.", true);
				}
			}
		else if (eleValid.name.substring(0,6) == "sample")
			{
			if (sPrefix == "") ele = frm.elements[sPrefix + eleValid.name.substring(7)];
			else ele = frm.elements[sPrefix + eleValid.name.substring(6)];

			if (ele == null)
				{
				NiceAlert("Could not find " + sPrefix + eleValid.name.substring(6));
				continue;
				}
			if (parseInt(ele.sampleon) == 1) ele.value = "";
			}
		
		else if (eleValid.name.substring(0,15) == "dategreaterthan")
			{
			if (sPrefix == "") ele = frm.elements[sPrefix + eleValid.name.substring(16)];
			else ele = frm.elements[sPrefix + eleValid.name.substring(15)];
			eleLabel = frm.elements["label" + eleValid.name.substring(15)];
			eleSame = frm.elements[eleValid.value];

			if (ele == null)
				{
				NiceAlert("Could not find " + sPrefix + eleValid.name.substring(6));
				continue;
				}
			if (eleSame == null)
				{
				NiceAlert("Could not find " + eleValid.value);
				continue;
				}
			if (ele.value == "" || eleSame.value == "") continue; // no test if either empty
			var dtGreater = GetNormalizedDate(ele.value)
			var dtLess = GetNormalizedDate(eleSame.value)
			if (dtGreater < dtLess)
				{
				var eleSameLabel = frm.elements["label_" + eleSame.name];
				return WarnValue(ele, eleLabel.value,  " greater than '" + eleSameLabel.value + "'.", true);
				}			
			}
		else if (eleValid.name.substring(0,6) == "sameas")
			{
			if (sPrefix == "") ele = frm.elements[sPrefix + eleValid.name.substring(7)];
			else ele = frm.elements[sPrefix + eleValid.name.substring(6)];
			eleLabel = frm.elements["label" + eleValid.name.substring(6)];
			eleSame = frm.elements[eleValid.value];

			if (ele == null)
				{
				NiceAlert("Could not find " + sPrefix + eleValid.name.substring(6));
				continue;
				}
			if (eleSame == null)
				{
				NiceAlert("could not find " + eleValid.value);
				continue;
				}
			if (GetEleValue(ele) != eleSame.value)
				{
				var eleSameLabel = frm.elements["label_" + eleSame.name];
				return WarnValue(ele, eleSameLabel.value + " and " + eleLabel.value, "the same.", true);
				}
			}
		else if (eleValid.name.substring(0,7) == "isemail")
			{
			if (sPrefix == "") ele = frm.elements[sPrefix + eleValid.name.substring(8)];
			else ele = frm.elements[sPrefix + eleValid.name.substring(7)];
			eleLabel = frm.elements["label" + eleValid.name.substring(7)];

			var sEmail = ele.value;
			if (sEmail == "")
				continue; //ok
			sEmail = StripChars(sEmail, " "); // strip spaces, whcih we get
			ele.value = sEmail;
			if (sEmail.indexOf("@") < 0 || sEmail.indexOf(".") < 0 || sEmail.indexOf(" ") >= 0)
				return WarnMsg(ele, "The " + eleLabel.value + " must contain @ and . and not contain any spaces", true);
			}
		else if (eleValid.name.substring(0,9) == "maxselect")
			{
			if (sPrefix == "") ele = frm.elements[sPrefix + eleValid.name.substring(10)];
			else ele = frm.elements[sPrefix + eleValid.name.substring(9)];
			eleLabel = frm.elements["label" + eleValid.name.substring(9)];

			// ele should be a msel list, and have no more than elevalid.value items selected
			var cMax = parseInt(eleValid.value);
			var cSelected = 0;
			var j = 0;
			for (j = 0; j < ele.length; j++)
				{
				if (ele.options[j].selected) cSelected ++;
				}
			if (cSelected > cMax)
				return WarnMsg(ele, "The question '" + eleLabel.value + "' can have no more than " + cMax + " items selected.", true);
			}
		else if (eleValid.name.substring(0,9) == "minselect")
			{
			if (sPrefix == "") ele = frm.elements[sPrefix + eleValid.name.substring(10)];
			else ele = frm.elements[sPrefix + eleValid.name.substring(9)];
			eleLabel = frm.elements["label" + eleValid.name.substring(9)];

			// ele should be a msel list, and have no more than elevalid.value items selected
			var cMin = parseInt(eleValid.value);
			var cSelected = 0;
			var j = 0;
			for (j = 0; j < ele.length; j++)
				{
				if (ele.options[j].selected) cSelected ++;
				}
			if (cSelected < cMin)
				return WarnMsg(ele, "The question '" + eleLabel.value + "' must have at least " + cMin + " items selected.", true);
			}
		else if (eleValid.name.substring(0,9) == "maxselstr")
			{
			if (sPrefix == "") ele = frm.elements[sPrefix + eleValid.name.substring(10)];
			else ele = frm.elements[sPrefix + eleValid.name.substring(9)];
			eleLabel = frm.elements["label" + eleValid.name.substring(9)];

			// ele should be a msel list, and have no more than elevalid.value items selected
			var cMax = parseInt(eleValid.value);
			var cSelected = 0;
			var j = 0;
			var arr = ele.value.split(",");
			for (j = 0; j < arr.length; j++)
				{
				if (trim(arr[j]) != "") cSelected ++;
				}
			if (cSelected > cMax)
				return WarnMsg(document.getElementById("list" + ele.name), "The question '" + eleLabel.value + "' can have no more than " + cMax + " items selected.", true);
			}
		else if (eleValid.name.substring(0,9) == "minselstr")
			{
			if (sPrefix == "") ele = frm.elements[sPrefix + eleValid.name.substring(10)];
			else ele = frm.elements[sPrefix + eleValid.name.substring(9)];
			eleLabel = frm.elements["label" + eleValid.name.substring(9)];

			// ele should be a msel list, and have no more than elevalid.value items selected
			var cMin = parseInt(eleValid.value);
			var cSelected = 0;
			var j = 0;
			var arr = ele.value.split(",");
			for (j = 0; j < arr.length; j++)
				{
				if (trim(arr[j]) != "") cSelected ++;
				}
			if (cSelected < cMin)
				return WarnMsg(document.getElementById("list" + ele.name), "The question '" + eleLabel.value + "' must have at least " + cMin + " items selected.", true);
			}
		else if (eleValid.name.substring(0,6) == "ishref")
			{
			if (sPrefix == "") ele = frm.elements[sPrefix + eleValid.name.substring(7)];
			else ele = frm.elements[sPrefix + eleValid.name.substring(6)];
			eleLabel = frm.elements["label" + eleValid.name.substring(6)];

			var sHref = ele.value;
			if (sHref == "")
				continue; //ok
			if (sHref.indexOf("http://") != 0 && sHref.indexOf("/Mentor") != 0)
				return WarnMsg(ele, "The " + eleLabel.value + " must be a valid HREF, starting with 'http://' or '/Mentor'", true);
			if (sHref.indexOf("http://") == 0 && sHref.length == 7)
				return WarnMsg(ele, "The " + eleLabel.value + " must be a valid HREF, starting with 'http://' and including a valid server name", true);
			}
		else if (eleValid.name.substring(0,6) == "length")
			{
			if (sPrefix == "") ele = frm.elements[sPrefix + eleValid.name.substring(7)];
			else ele = frm.elements[sPrefix + eleValid.name.substring(6)];
			eleLabel = frm.elements["label" + eleValid.name.substring(6)];

			var sValue = ele.value;
			if (sValue.length > parseInt(eleValid.value))
				return WarnMsg(ele, "The answer to '" + eleLabel.value + "' is too long. It must be fewer than " + eleValid.value + " characters.", true);
			}
		else if (eleValid.name.substring(0,5) == "valid" && eleValid.value != "string")  // 3 is string: we dont care
			{
			if (sPrefix == "") ele = frm.elements[sPrefix + eleValid.name.substring(6)];
			else ele = frm.elements[sPrefix + eleValid.name.substring(5)];
			eleLabel = frm.elements["label" + eleValid.name.substring(5)];
			if (ele == null || ele.value == "") 
				{
				if (ele == null)
					{
					NiceAlert("could not find " + sPrefix + eleValid.name.substring(5));
					}
				continue;
				}
			switch (eleValid.value) 
				{
			case "long": 	
				if (!IsInteger(ele.value))
					{
					return WarnValue(ele, eleLabel.value, "a positive or negative integer.", true);
					}
				break;
			case "nonnegLong":
				if (!IsInteger(ele.value) || parseInt(ele.value) < 0)
					{
					return WarnValue(ele, eleLabel.value, "a positive integer, or zero.", true);
					}
				break;
			case "bool":
				if (!IsInteger(ele.value) || parseInt(ele.value) > 1 || parseInt(ele.value) < 0)
				{
					return WarnValue(ele, eleLabel.value, "either one or zero.", true);
				}
				break;
			case "float":   // for now, we mean nonneg float
				if (!IsFloat(ele.value) || parseFloat(ele.value) < 0)
					{
					return WarnValue(ele, eleLabel.value, "a positive number, or zero.", true);
					}
				ele.value = parseFloat(ele.value);
				break;
			case "anyfloat":   // lame
				if (!IsFloat(ele.value))
					{
					return WarnValue(ele, eleLabel.value, "a number.", true);
					}
				ele.value = parseFloat(ele.value);
				break;
			case "string":			 // no validation
				break;
			case "dateonly":
				if (!IsDate(ele.value))
					{
					return WarnValue(ele, eleLabel.value, "a valid date (with no time).", true);
					}
				break;
			case "timeonly":
				if (!IsTime(ele.value))
					{
					return WarnValue(ele, eleLabel.value, "a valid time.", true);
					}
				break;
			case "date":
				if (!IsDateTime(ele.value))
					{
					return WarnValue(ele, eleLabel.value, "a valid date and time.", true);
					}
				break;
			case "futuredate":
				if (!IsDateTime(ele.value))
					{
					return WarnValue(ele, eleLabel.value, "a valid date and time.", true);
					}
				if (!IsFutureDateTime(ele.value))
					{
					return WarnValue(ele, eleLabel.value, "a date in the future.", true);
					}
				break;
				}
			}
		}
	return true;
}

function WarnValue(ele, sLabel, sMsg, bDoFocus)
{
	return WarnMsg(ele, "The value of '" + sLabel + "' must be " + sMsg, bDoFocus);
}

function WarnMsg(ele, sMsg, bDoFocus)
{
	NiceAlert(sMsg);
	if (bDoFocus) 
		{
		if(ele.type != "hidden") ele.focus();
		if (IsSelectable(ele))
			ele.select();
		}
	return false; // so you can just return this value
}

function IsInteger(sValue)
{
	if (!IsFloat(sValue)) return false;
	var iValue = parseInt(sValue);
	if (iValue * 1.0 != parseFloat(sValue)) return false;
	return true;
}

function IsFloat(sValue)
{
	var fltValue = parseFloat(sValue);
	if (isNaN(fltValue)) return false;
	return true;
}

function IsDateTime(sValue)
{
	var dt = new Date(sValue)
	if (isNaN(dt)) return false;
	return true;
}

function GetNormalizedDate(sValue)
{
	var dt = new Date(sValue)
	if (isNaN(dt)) return null;
	if (dt.getFullYear() < 1930) dt.setFullYear(dt.getFullYear() + 100);	
	return dt;
}

function IsFutureDateTime(sValue)
{
	var dt = new Date(sValue)
	if (isNaN(dt)) return false;
	if (dt.getFullYear() < 1930) dt.setFullYear(dt.getFullYear() + 100);
	if (dt < new Date())
		return false;
	return true;
}

function IsDate(sValue)
{
	if (!IsDateTime(sValue)) return false;
	var dt;
	dt = new Date(sValue);
	if (dt.getHours() != 0 || dt.getMinutes() != 0 || dt.getSeconds() != 0) return false;
	return true;
}

function IsTime(sValue)
{
	if (!IsDateTime(sValue))
		{
		if (IsDateTime("1/1/11 " + sValue)) return true;
		return false;
		}
	return false;
}



function SubmitForm(sFormName, sFormCode)
{
	if (f4x)
		{
		var frm = document.forms[sFormName];
		if (frm == null) return;

		if (ValidateMiscInput(frm, ""))
			{
			frm.submit();
			}
		else
			return;
		}
	else
		{
		var frm = document.forms[sFormName];	
		frm.submit();
		return;
		}
	
}

function ConfirmLink(sMsg, sLink)
{
	if (!NiceConfirmYN(sMsg))
		return;
	document.location = sLink;
}



function CacheConfirm(sKey, sTitle, sMsg)
{
	var res = GetCookie(sKey);
	if(res)
		return parseInt(res) != 0;
	
	if(sTitle == "") sTitle = "Please confirm your action";
	res = NiceAlertEx(3, sTitle, sMsg, "Yes", "No");
	
	if(res == 2)
		SetCookie(sKey, new String(res));
		
	return res != 0;
}

function CacheConfirmLink(sKey, sTitle, sMsg, sLink)
{
	if(!CacheConfirm(sKey, sTitle, sMsg))
		return;
	document.location = sLink;
}

function ConfirmSubmitForm(sMsg, sFormName, sFormCode)
{
	if(NiceConfirmYN(sMsg))
		SubmitForm(sFormName,sFormCode);
}



function NiceAlert(sMsg)
{
	NiceAlertOK("Alert", sMsg);
}

function NiceConfirm(sMsg)
{
	return NiceConfirmEx("", sMsg, "OK", "Cancel");
}

function NiceConfirmYN(sMsg)
{
	return NiceConfirmEx("", sMsg, "Yes", "No");
}

function NiceConfirmYNBig(sTitle, sMsg)
{
	return NiceConfirmExBig(sTitle, sMsg, "Yes", "No");
}

function NiceConfirmEx(sTitle, sMsg, sOk, sCancel)
{
	if (sTitle == "") sTitle = "Please confirm your action";
	return NiceAlertEx(1, sTitle, sMsg, sOk, sCancel);
}

function NiceConfirmExBig(sTitle, sMsg, sOk, sCancel)
{
	if (sTitle == "") sTitle = "Please confirm your action";
	return NiceAlertEx(2, sTitle, sMsg, sOk, sCancel);
}

function CacheNiceConfirm(sSection, sTitle, sMsg)
{
	var res = GetCookie(sSection);
	if(res)
		return parseInt(res) != 0;
	
	if(sTitle == "") sTitle = "Please confirm your action";
	res = NiceAlertEx(3, sTitle, sMsg, "Yes", "No");
	
	if(res == 2)
		SetCookie(sSection, new String(res));
		
	return res != 0;
}

function NiceAlertOK(sTitle, sMsg)
{
	NiceAlertEx(0, sTitle, sMsg);
}

function SubmitAlertForm(sFormId)
{
	var ele = document.getElementById(sFormId);
	if(ValidateMiscInput(ele,''))
		DoClose(ele.outerHTML);
}

function FormAlert(sFrm, sTitle, sMsg, sDiv, dx, dy)
{
	var sUrl;
	var sFeatures = "dialogWidth: " + dx + "; dialogHeight: " + dy + "; status: 0; help:0; resizable: 0; center: 1; scroll: 0;";
	var arArgs = new Array()
	arArgs[0] = sTitle;
	arArgs[1] = sMsg;
	arArgs[2] = document.getElementById(sDiv).innerHTML;
		
	sUrl = "/MMDCentral/Common/Dialog/Form.aspx?" + Math.random();
	var res = window.showModalDialog(sUrl, arArgs, sFeatures); // if returns, it should have not submitted
	if (res != 0) 
		{
		document.getElementById(sDiv).innerHTML = res; // put new form in old div
		document.all[sFrm].submit();
		}
}

function NiceAlertEx(iType, sTitle, sMsg, sOk, sCancel)
{
	var sUrl;
	var sFeatures = "dialogWidth: 26; dialogHeight: 9; status: 0; help:0; resizable: 0; center: 1; scroll: 0;";
	var arArgs = new Array()
	arArgs[0] = sTitle;
	arArgs[1] = sMsg;

	switch (parseInt(iType))
		{
	case 0:
		sUrl = "/MMDCentral/Common/Dialog/OK.aspx?" + Math.random();
   	break;
	case 1:
		sUrl = "/MMDCentral/Common/Dialog/YesNo.aspx?" + Math.random();
		arArgs[2] = sOk;
		arArgs[3] = sCancel;
		break;
	case 2:
		sUrl = "/MMDCentral/Common/Dialog/YesNo.aspx?" + Math.random();
		sFeatures = "dialogWidth: 30; dialogHeight: 12; status: 0; help:0; resizable: 0; center: 1; scroll: 0;";
		arArgs[2] = sOk;
		arArgs[3] = sCancel;
		break;
   	case 3:
		sUrl = "/MMDCentral/Common/Dialog/YesNoCookie.aspx?" + Math.random();
		sFeatures = "dialogWidth: 26; dialogHeight: 10; status: 0; help:0; resizable: 0; center: 1; scroll: 0;";
		arArgs[2] = sOk;
		arArgs[3] = sCancel;
		return parseInt( window.showModalDialog(sUrl, arArgs, sFeatures) );		
		break;
	case 5:
		sUrl = "/MMDCentral/Common/Dialog/OK.aspx?" + Math.random();
		sFeatures = "dialogWidth: 40; dialogHeight: 20; status: 0; help: 0; resizable: 1; center: 1; scroll: 1;";
		break;
	otherwise:
		alert(sMsg);
		}
	var res = window.showModalDialog(sUrl, arArgs, sFeatures);		
	if (parseInt(iType) != 0) 
		return parseInt(res) == 1;
}


//  ********************* General Cookie handling *********************
//  Cookie Functions - Second Helping  (21-Jan-96)
//  Written by:  Bill Dortch, hIdaho Design <bdortch@netw.com>
//  The following functions are released to the public domain.

function getCookieVal (offset) {  
        var endstr = document.cookie.indexOf (";", offset);  
        if (endstr == -1)    
                endstr = document.cookie.length;  
                return unescape(document.cookie.substring(offset, endstr));
}

function GetCookie (name) {  
        var arg = name + "=";  
        var alen = arg.length;  
        var clen = document.cookie.length;  
        var i = 0;  
        while (i < clen) {    
        var j = i + alen;    
        if (document.cookie.substring(i, j) == arg)      
                return getCookieVal (j);    
                i = document.cookie.indexOf(" ", i) + 1;    
                if (i == 0) break;   
        }  
        return null;
}

function SetCookie (name, value) {  
        var argv = SetCookie.arguments;  
        var argc = SetCookie.arguments.length;  
        var expires = (argc > 2) ? argv[2] : null;  
        var path = (argc > 3) ? argv[3] : null;  
        var domain = (argc > 4) ? argv[4] : null;  
        var secure = (argc > 5) ? argv[5] : false;  
        document.cookie = name + "=" + escape (value) + 
        ((expires == null) ? "" : ("; expires=" + expires.toGMTString())) + 
        ((path == null) ? "" : ("; path=" + path)) +  
        ((domain == null) ? "" : ("; domain=" + domain)) +    
        ((secure == true) ? "; secure" : "");
}

function DeleteCookie (name) {  
        var exp = new Date();  
        exp.setTime (exp.getTime() - 1);  
        // This cookie is history  
        var cval = GetCookie (name);  
        document.cookie = name + "=" + cval + "; expires=" + exp.toGMTString();

}


function LVToggleCheckAll() 
{
	for(i=1;i<=parseInt(document.getElementById("lv_RecCount").value);i++)
		document.getElementById("lv_Check_" + i).checked = event.srcElement.checked;
}

function LVSetSort(sLV, sTargetPage, sSortKey, sOrderLink, sAnchorTarget)
{
	event.srcElement.target = sAnchorTarget;
	if(event.ctrlKey)
		event.srcElement.href = sTargetPage + '?pgCmd=AddSort&pgLV=' + sLV + '&pgParamCount=1&pgSortKey_0=' + sSortKey + '&pgSortOrder_0=' + sOrderLink;
	else
		event.srcElement.href = sTargetPage + '?pgCmd=SetSort&pgLV=' + sLV + '&pgParamCount=1&pgSortKey_0=' + sSortKey + '&pgSortOrder_0=' + sOrderLink;
	return true;
}