<%@ Page language="c#" Codebehind="Home.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Advertisement.Home" %>
<% CSWContext.PageTitle = "Advertisement Home"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>

<div class="PageTitle">Advertisement Home</div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

This section will allow for configuring advertising content for stores/store grouping, 
as well as idleloop content creation.
<br>
<p>
<a href="/CentralWeb/Advertisement/BannerTypeView.aspx">Manage Advertisement Banner Types</a><br>
<a href="/CentralWeb/Advertisement/IdleLoopsView.aspx">Manage Idle Loops</a><br>

<div style="height: 50px;"></div>
<i>Feature Specification Excerpt Below:</i>

<h3><a name="_Toc19198718"><![if !supportLists]>1.1.1<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Retail Advertising Manager Application</a></h3>

<p class=MsoNormal>This Application manages all advertisement that may be
deployed to the MMD idle loops.</p>

<p class=MsoNormal>Some of the features overlap with the Marketing Manager,
however the information included is designed to aid in decisions on where
advertisements are placed through out the nation/store.</p>

<p class=MsoNormal>Ability to import, view, schedule and deploy Idle loops for
playback on devices.<span style="mso-spacerun: yes">  </span>Focused Genre
advertisements may be scheduled to pay by section.<span style="mso-spacerun:
yes">  </span>Reporting for the idle loops contain the following information:</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l75 level1 lfo16;
tab-stops:list .75in'><![if !supportLists]>1.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Impressions – verification of playback.<span
style="mso-spacerun: yes">  </span>(Possible to integrate into traffic security
system?)</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l75 level1 lfo16;
tab-stops:list .75in'><![if !supportLists]>2.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Interruptions – add main application menu to view more
information about the specific Idle loop advertisement.</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l75 level1 lfo16;
tab-stops:list .75in'><![if !supportLists]>3.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]><span style="mso-spacerun: yes"> </span>Recommendations for
scheduling optimization.<span style="mso-spacerun: yes">  </span>Traffic,
purchase times, correlation with specific feature items and high traffic
purchases with advertisements.<span style="mso-spacerun: yes">  </span>i.e.
Volume Brittany Spears is purchased from 3-5 each week day and 11-4 on
Saturday, focus Rock new releases during that time.<span style="mso-spacerun:
yes">  </span>Top scanned item during Sunday after church 1-4 PM is Christian
rock, display advertisement for Amy Grant’s new album at that time.</p>



<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
