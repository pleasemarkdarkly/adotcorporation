<%@ Page language="c#" Codebehind="Home.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Content.Home" %>
<% CSWContext.PageTitle = "Content Management Home"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>


<div class="PageTitle">Content Management Home</div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

This section will contain support for 
<ul>
<li>Importing new Metadata</li>
<li>Importing new Content (Media, Cover Art, etc)</li>
<li>Configuring BG Tasks</li>
<li>Scheduling BG Tasks</li>
<li>Running BG Tasks Synchronously</li>
<li>Pushing Content to Stores/Store Groups</li>
</ul>




<div style="height: 50px"></div>
<i>Feature Specification Excert Below:</i>
<h3><a name="_Toc19198717"><![if !supportLists]>1.1.1<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Retail Content Manager Application</a></h3>

<p class=MsoNormal>This Application manages the update process, roll back,
centralized logging, usage statistics, and operational health of all servers
and MMD devices on the network.<span style="mso-spacerun: yes">  </span>This
application is the heart of the system, the main central operations ‘package’
deployment and device health retrieval system. </p>

<p class=MsoNormal><![if !supportEmptyParas]>&nbsp;<![endif]><o:p></o:p></p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l68 level1 lfo15;
tab-stops:list .75in'><![if !supportLists]>1.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Application displays all installed servers that it has
established communication with.<span style="mso-spacerun: yes"> 
</span>(Central operations application will be in place before stores are
deployed.<span style="mso-spacerun: yes">  </span>Each store server will
communicate with the main operations center on installation.)</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l68 level1 lfo15;
tab-stops:list .75in'><![if !supportLists]>2.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Each MMD application will schedule a synchronization time with
the central server.<span style="mso-spacerun: yes">  </span>This is used to
calculate region, nation update procedures, as well as enable us to schedule
pushes to the MMD server for other types of information such as Advertisements.</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l68 level1 lfo15;
tab-stops:list .75in'><![if !supportLists]>3.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Application will display </p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l68 level2 lfo15;
tab-stops:list 1.25in'><![if !supportLists]>a.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Server version number, status, up time and configuration</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l68 level2 lfo15;
tab-stops:list 1.25in'><![if !supportLists]>b.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>All client nodes that the Server manages</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l68 level2 lfo15;
tab-stops:list 1.25in'><![if !supportLists]>c.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>A roll up of any anomalies concerning the system and MMD
including</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.75in;margin-bottom:.0001pt;text-indent:-1.75in;mso-text-indent-alt:
-9.0pt;mso-list:l68 level3 lfo15;tab-stops:list 1.75in'><![if !supportLists]><span
style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span>i.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Device:</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:2.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l68 level4 lfo15;
tab-stops:list 2.25in'><![if !supportLists]>1.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>devices that are used frequently/infrequently</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:2.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l68 level4 lfo15;
tab-stops:list 2.25in'><![if !supportLists]>2.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>device that are out of service</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:2.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l68 level4 lfo15;
tab-stops:list 2.25in'><![if !supportLists]>3.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>devices that skip audio/video</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:2.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l68 level4 lfo15;
tab-stops:list 2.25in'><![if !supportLists]>4.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>device content publishing latency</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.75in;margin-bottom:.0001pt;text-indent:-1.75in;mso-text-indent-alt:
-9.0pt;mso-list:l68 level3 lfo15;tab-stops:list 1.75in'><![if !supportLists]><span
style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span>ii.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Metadata:</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:2.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l68 level4 lfo15;
tab-stops:list 2.25in'><![if !supportLists]>1.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>UPC failed lookups</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:2.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l68 level4 lfo15;
tab-stops:list 2.25in'><![if !supportLists]>2.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>POS retrieval failure</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l68 level2 lfo15;
tab-stops:list 1.25in'><![if !supportLists]>d.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Mechanism to specific un update package and which stores to
deploy it to.</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:1.25in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l68 level2 lfo15;
tab-stops:list 1.25in'><![if !supportLists]>e.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Views for scheduled updates and current progress.</p>

<p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt'><![if !supportEmptyParas]>&nbsp;<![endif]><o:p></o:p></p>

<p class=MsoNormal>The database has the following blocks of information:</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l99 level1 lfo14;
tab-stops:list .75in'><![if !supportLists]>1.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Metadata and content tables – To be delivered to customers to
synch with existing providers.</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l99 level1 lfo14;
tab-stops:list .75in'><![if !supportLists]>2.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Device logging tables – FullPlay internal</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l99 level1 lfo14;
tab-stops:list .75in'><![if !supportLists]>3.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Profiles tables – Import and export rules for Retailers
existing system.</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l99 level1 lfo14;
tab-stops:list .75in'><![if !supportLists]>4.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Recommendation tables – Export to potential recommendation
companies such as AIA, Mood logic, and Savage Beast.</p>

<p class=MsoNormal style='margin:0in;margin-bottom:.0001pt'><![if !supportEmptyParas]>&nbsp;<![endif]><o:p></o:p></p>



<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
