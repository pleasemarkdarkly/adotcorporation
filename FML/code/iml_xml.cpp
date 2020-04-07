#include "iml_xml.h"

char deviceDescriptionUID[80];

char deviceDescriptionBase[] = 

"<?xml version=\"1.0\"?>"
"<root xmlns=\"urn:schemas-upnp-org:device-1-0\">"
"  <specVersion>"
"    <major>1</major>"
"    <minor>0</minor>"
"  </specVersion>"
"  <device>"
"    <UDN>uuid:%s</UDN><friendlyName>%s</friendlyName>"
"    <deviceType>urn:www-fullplaymedia-com:device:medialibrary:1</deviceType>"
//"    <presentationURL>/presentation/</presentationURL>"
"    <manufacturer>Fullplay Media Systems</manufacturer>"
"    <manufacturerURL>http://www.fullplaymedia.com/</manufacturerURL>"
"    <modelName>FPML</modelName>"
"    <modelNumber>1</modelNumber>"
"    <modelDescription>Fullplay Media Library</modelDescription>"
"    <modelURL>http://www.fullplaymedia.com.com/</modelURL>"
"    <UPC>000000000001</UPC>"
"    <serialNumber>0000001</serialNumber>"
"    <serviceList>"
"      <service>"
"        <serviceType>urn:www-fullplaymedia-com:service:mls:1</serviceType>"
"        <serviceId>urn:upnp-org:serviceId:mls</serviceId>"
"        <controlURL>/control/</controlURL>"
"        <eventSubURL>/events/</eventSubURL>"
"        <SCPDURL>/iMLSCPD.xml</SCPDURL>"
"      </service>"
"    </serviceList>"
"    <URLBase>%s</URLBase>"
"   </device>"
"</root>";

