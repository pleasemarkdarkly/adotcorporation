var HTTP_DIR = "http://www.expedia.com"

var zz, zv, d, fTSR;
var gBF=false;
var g_MINY = 1601;
var g_MAXY = 4500;
var g_month = 0;
var g_day = 0;
var g_year = 0;
var g_yLow = 1990;
var g_eC=null;
var g_eCV="";
var g_dFmt=GetDateFmt();
var g_fnCB=null;

var rgMC = new Array(12);
rgMC[0] = 31;rgMC[1] = 28;rgMC[2] = 31;rgMC[3] = 30;rgMC[4] = 31;rgMC[5] = 30;rgMC[6] = 31;rgMC[7] = 31;rgMC[8] = 30;rgMC[9] = 31;rgMC[10] = 30;rgMC[11] = 31;

d = new Date();
fTSR=0;
zv = d.getTime();
zz = "&zz="+zv;

function GetDowStart() {return 0;}
function GetDateFmt() {return "mmddyy";}
function GetDateSep() {return "/";}

function GetInputDate(t,f){
  var l = t.length;
  if(0 == l) return false;
  var cSp = '\0';
  var sSp1 = "";
  var sSp2 = "";
  for(var i = 0; i < t.length; i++){
    var c = t.charAt(i);
    if(c == ' ' || isdigit(c)) continue;
    else if(cSp == '\0' && (c == '/' || c == '-' || c == '.')){
      cSp = c;
      sSp1 = t.substring(i+1,l);
    }
    else if(c == cSp) sSp2 = t.substring(i+1,l);
    else if(c != cSp) return false;
  }
  if(0 == sSp1.length) return false;
  var m;
  var d;
  var y; 
  if(f=="mmddyy"){
    m = atoi(t);
    d = atoi(sSp1);
    if(0 != sSp2.length) y = atoi(sSp2);
    else y = DefYr(m,d);
  }
  else if(f=="ddmmyy"){
    m = atoi(sSp1);
    d = atoi(t);
    if(0 != sSp2.length) y = atoi(sSp2);
    else y = DefYr(m,d);
  }
  else{
    if(0 == sSp2.length) return false;
    m = atoi(sSp1);
    d = atoi(sSp2);
    y = atoi(t);
  }
  if(y < 100){
    y = 1900+y;
    while(y < g_yLow) y = y+100;
  }
  if(y < g_MINY || y > g_MAXY || m < 1 || m > 12) return false;
  if(d < 1 || d > GetMonthCount(m,y)) return false;
  g_month = m;
  g_day = d;
  g_year = y;
  return true;
}

function DefYr(m,d){
  var dt = new Date();
  var yCur = (dt.getYear() < 1000) ? 1900+dt.getYear() : dt.getYear();
  if(m-1 < dt.getMonth() || (m-1 == dt.getMonth() && d < dt.getDate())) return 1+yCur;
  else return yCur;
}

function atoi(s){
  var t = 0;
  for(var i = 0; i < s.length; i++){
    var c = s.charAt(i);
    if(!isdigit(c)) return t;
    else t = t*10 + (c-'0');
  }
  return t;
}

function isdigit(c) {return(c >= '0' && c <= '9');}
function GetMonthCount(m,y){
  var c = rgMC[m-1];
  if((2 == m) && IsLeapYear(y)) c++;
  return c;
}
function IsLeapYear(y){
  if(0 == y % 4 && ((y % 100 != 0) || (y % 400 == 0))) return true;
  else return false;
}

function ShowCalendar(eP,eD,eDP,dmin,dmax,fnCB){
  var dF=document.all.CalFrame;
  var wF=window.frames.CalFrame;

  if(null==wF.g_fCalLoaded || false==wF.g_fCalLoaded){
    alert("Unable to load popup calendar.\r\nPlease reload the page.");
    return;
  }

  if(dmin) dmin = new Date(dmin);
  if(dmax) dmax = new Date(dmax);
  wF.SetMinMax(dmin,dmax);
  g_fnCB=fnCB;


  if(eD==g_eC && "block"==dF.style.display){
    if(g_eCV != eD.value && GetInputDate(eD.value,g_dFmt)){
      wF.SetInputDate(g_day,g_monthp-1,g_year);
      wF.SetDate(g_day,g_month-1,g_year);
      g_eCV=eD.value;
    }
    else
      dF.style.display="none";
  }
  else{
    if(GetInputDate(eD.value,g_dFmt)){
      wF.SetInputDate(g_day,g_month-1,g_year);
      wF.SetDate(g_day,g_month-1,g_year);
    }
    else if(null != eDP && GetInputDate(eDP.value,g_dFmt)){
      wF.SetInputDate(g_day,g_month-1,g_year);
      wF.SetDate(g_day,g_month-1,g_year);
    }
    else{
	  var dt = (dmin) ? new Date(dmin) : new Date();
      //wF.SetInputDate(-1,-1,-1);
      wF.SetDate(dt.getDate(),dt.getMonth(),dt.getFullYear());
    }
    var eL=0;var eT=0;
    for(var p=eP; p&&p.tagName!='BODY'; p=p.offsetParent){
      eL+=p.offsetLeft;
      eT+=p.offsetTop;
    }
    var eH=eP.offsetHeight;
    var dH=dF.style.pixelHeight;
    var sT=document.body.scrollTop;
    dF.style.left=eL;
    if(eT-dH >= sT && eT+eH+dH > document.body.clientHeight+sT)
      dF.style.top=eT-dH;
    else
      dF.style.top=eT+eH;
    if("none"==dF.style.display)
    {
      dF.style.display="block";
      //dF.style.overflow="visible";
    }
    g_eC=eD;
    g_eCV=eD.value;

	wF.BuildCurrentCalendar();
  }
}

function SetDate(d,m,y){
  var ds=GetDateSep();
  g_eC.focus();
  if("mmddyy"==g_dFmt) g_eC.value=m+ds+d+ds+y;
  else if("ddmmyy"==g_dFmt) g_eC.value=d+ds+m+ds+y;
  else g_eC.value=y+ds+m+ds+d;
  g_eCV=g_eC.value;
  if(null != g_fnCB && "" != g_fnCB)
    eval(g_fnCB);
}

function SetCalFrameSize(width,height)
{
	document.all.CalFrame.style.pixelWidth = width;
	document.all.CalFrame.style.pixelHeight = height;
}

