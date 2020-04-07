<%@ Page language="c#" Codebehind="Home.aspx.cs" AutoEventWireup="false" Inherits="MMD.Central.CentralWeb.Profiles.WebForm1" %>
<% CSWContext.PageTitle = "Profile Management Home"; %>
<% Server.Execute("/CentralWeb/Common/Header.aspx"); %>


<div class="PageTitle">Profile Management Home</div>
<% Server.Execute("/CentralWeb/Common/ErrorMessage.aspx"); %>

<div class="SectionTitle">Recommendation Lists</div>
<div style="padding-left: 5mm;">
<li><a href="/CentralWeb/Profiles/RecListView.aspx">Manage</a></li>
</div>

<div style="height: 50px"></div>
<i>Feature Specification Excert Below:</i>
<h3><a name="_Toc19198719"><![if !supportLists]>1.1.1<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Retail Profiles Manager Application</a></h3>

<p class=MsoNormal>This Application contains all the relevant information
relating to management and actions of profiles that exist in the system.<span
style="mso-spacerun: yes">  </span>There are two types of data represented
here, profiles, which are users that explicitly log onto the system or sessions
that are started and ended and recommendations that are inputted into the
system.<span style="mso-spacerun: yes">  </span>Recommendations are driven by:</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l39 level1 lfo17;
tab-stops:list .75in'><![if !supportLists]>1.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>User Recommendations – In store application that emulates the
rack where employees pick which content they prefer.</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l39 level1 lfo17;
tab-stops:list .75in'><![if !supportLists]>2.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Data Recommendations – Information is processed by the POS
which albums sorted by genres are popular purely based on purchasing
profile.<span style="mso-spacerun: yes">  </span>Profiles that are created
store ‘Wish Lists’, past purchases, and past views.<span style="mso-spacerun:
yes">  </span>This information is used for MMD collaborative recommendations.</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l39 level1 lfo17;
tab-stops:list .75in'><![if !supportLists]>3.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Machine Recommendations – Information that is inferred by a
third party to show relationships between content.<span style="mso-spacerun:
yes">  </span>This is not immediately interesting since the ‘proximity’
recommendation is important for an in store experience, but through the profile
program, lists of content can be stored and potentially accessed or purchase
through another route.</p>

<p class=MsoNormal><![if !supportEmptyParas]>&nbsp;<![endif]><o:p></o:p></p>

<p class=MsoNormal>A profile or new customer is created on a PC in store or
online.<span style="mso-spacerun: yes">  </span>This information is located MMD
until synchronization occurs with the main central operations.<span
style="mso-spacerun: yes">  </span>The resultant pass-key is a PIN and barcode
that is accepted at the MMD in profile log in mode.</p>

<p class=MsoNormal>The profile provides access to a customized area for the
following experiences:</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l30 level1 lfo18;
tab-stops:list .75in'><![if !supportLists]>1.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Customize content view, such as play and extended metadata.</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l30 level1 lfo18;
tab-stops:list .75in'><![if !supportLists]>2.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Past scans and wish list picks.</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l30 level1 lfo18;
tab-stops:list .75in'><![if !supportLists]>3.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Past purchases or other ‘owned’ content from the web or home
applications.</p>

<p class=MsoNormal style='margin-top:0in;margin-right:0in;margin-bottom:0in;
margin-left:.75in;margin-bottom:.0001pt;text-indent:-.25in;mso-list:l30 level1 lfo18;
tab-stops:list .75in'><![if !supportLists]>4.<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span><![endif]>Promotions that are linked by content of interest.</p>

<p class=MsoNormal><![if !supportEmptyParas]>&nbsp;<![endif]><o:p></o:p></p>

<p class=MsoNormal>Do we care about knowing about specific store employees that
input recommendations to the system?<span style="mso-spacerun: yes">  </span>At
first blush, I don’t think so.<span style="mso-spacerun: yes">  </span>Since
that information is collated for in store staff picks.<span
style="mso-spacerun: yes">  </span>We don’t want to invalidate staff picks or
recommendations if the employee is terminated.<span style="mso-spacerun: yes"> 
</span>It also reduces complexity for keeping a separate log in interface.</p>

<p class=MsoNormal>We have customer profiles, in store managers, and employees.</p>


<% Server.Execute("/CentralWeb/Common/Footer.aspx"); %>
