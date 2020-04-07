<%@ Page language="c#" Codebehind="Home.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Marketing.Home" %>
<% CSWContext.PageTitle = "Marketing Home"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>


<div class="PageTitle">Marketing Home</div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

This section will contain support for 
<ul>
	<li>Monitor customer/shopper activity per customer/store/store groups</li>
	<li>Tracking effectiveness of advertising schemes currently loaded into store/store groups</li>
</ul>




<div style="height: 50px"></div>
<i>Feature Specification Excert Below:</i>
<h3><a name="_Toc19198716"><![if !supportLists]>1.1.1<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Retail Marketing Manager Application</a></h3>

<p class=MsoNormal>This Application requires real-time sampling and POS
information.<span style="mso-spacerun: yes">  </span>This application
facilitates the views and creations of queries that allow the marketing manager
to generation reports and gather key strategic decisions.<span
style="mso-spacerun: yes">  </span>Information presented is content, usage
models and POS specific.</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l92 level1 lfo13;
tab-stops:list .75in'><![if !supportLists]>1.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Device usage patterns</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l92 level2 lfo13;
tab-stops:list 1.25in'><![if !supportLists]>a.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Detect dead spots or non-used devices</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l92 level2 lfo13;
tab-stops:list 1.25in'><![if !supportLists]>b.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Present high traffic device details</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l92 level2 lfo13;
tab-stops:list 1.25in'><![if !supportLists]>c.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Types of devices that are being used, Genre, Multi-Genre,
Features</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l92 level1 lfo13;
tab-stops:list .75in'><![if !supportLists]>2.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Content usage patterns</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l92 level2 lfo13;
tab-stops:list 1.25in'><![if !supportLists]>a.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Top scanned content</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l92 level2 lfo13;
tab-stops:list 1.25in'><![if !supportLists]>b.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Top Idle Loop Interruption</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l92 level2 lfo13;
tab-stops:list 1.25in'><![if !supportLists]>c.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Usage type</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.75in;margin-bottom:.0001pt;text-indent:-1.75in;mso-text-indent-alt:
-9.0pt;mso-list:l92 level3 lfo13;tab-stops:list 1.75in'><![if !supportLists]><span
style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span>i.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Sampling content</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.75in;margin-bottom:.0001pt;text-indent:-1.75in;mso-text-indent-alt:
-9.0pt;mso-list:l92 level3 lfo13;tab-stops:list 1.75in'><![if !supportLists]><span
style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span>ii.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Traversing content promotion</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:2.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l92 level4 lfo13;
tab-stops:list 2.25in'><![if !supportLists]>1.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Top Sellers</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:2.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l92 level4 lfo13;
tab-stops:list 2.25in'><![if !supportLists]>2.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Recommendations</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l92 level1 lfo13;
tab-stops:list .75in'><![if !supportLists]>3.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Store placement information</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l92 level2 lfo13;
tab-stops:list 1.25in'><![if !supportLists]>a.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Popular Genres listings</p>


<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>