# Microsoft Developer Studio Project File - Name="mavspi" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mavspi - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mavspi.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mavspi.mak" CFG="mavspi - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mavspi - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mavspi - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mavspi - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Gz /ML /W3 /Gm /ZI /Od /I "c:\progra~1\wmsdk\wmfsdk\include" /D "_DEBUG" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 msvcrtd.lib mssachlp.lib rsa32sdk2.lib randlib.lib bignumsdk.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib setupapi.lib /nologo /subsystem:windows /dll /debug /machine:I386 /nodefaultlib /pdbtype:sept /libpath:"c:\progra~1\wmsdk\wmfsdk\lib"
# SUBTRACT LINK32 /pdb:none /incremental:no
# Begin Custom Build - Performing registration
OutDir=.\Debug
TargetPath=.\Debug\mavspi.dll
InputPath=.\Debug\mavspi.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "mavspi - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "mavspi___Win32_Release"
# PROP BASE Intermediate_Dir "mavspi___Win32_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /Gz /W3 /O1 /I "c:\progra~1\wmsdk\wmfsdk\include" /D "NDEBUG" /D "_MBCS" /D "_ATL_STATIC_REGISTRY" /D "_ATL_MIN_CRT" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Gz /W3 /I "c:\progra~1\wmsdk\wmfsdk\include" /D "NDEBUG" /D "_ATL_STATIC_REGISTRY" /D "WIN32" /D "_WINDOWS" /D "_USRDLL" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 msvcrt.lib mssachlp.lib rsa32sdk2.lib randlib.lib bignumsdk.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386 /nodefaultlib /libpath:"c:\progra~1\wmsdk\wmfsdk\lib"
# ADD LINK32 msvcrt.lib mssachlp.lib rsa32sdk2.lib randlib.lib bignumsdk.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib setupapi.lib /nologo /subsystem:windows /dll /incremental:yes /machine:I386 /nodefaultlib /libpath:"c:\progra~1\wmsdk\wmfsdk\lib"
# Begin Custom Build - Performing registration
OutDir=.\Release
TargetPath=.\Release\mavspi.dll
InputPath=.\Release\mavspi.dll
SOURCE="$(InputPath)"

"$(OutDir)\regsvr32.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	regsvr32 /s /c "$(TargetPath)" 
	echo regsvr32 exec. time > "$(OutDir)\regsvr32.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "mavspi - Win32 Debug"
# Name "mavspi - Win32 Release"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\auddat.cpp
# End Source File
# Begin Source File

SOURCE=.\device.cpp
# End Source File
# Begin Source File

SOURCE=.\enumdev.cpp
# End Source File
# Begin Source File

SOURCE=.\enumstor.cpp
# End Source File
# Begin Source File

SOURCE=.\mavcmd.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\mavinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\mavspi.cpp
# End Source File
# Begin Source File

SOURCE=.\mavspi.def
# End Source File
# Begin Source File

SOURCE=.\mavspi.idl
# ADD MTL /tlb ".\mavspi.tlb" /h "mavspi.h" /iid "mavspi_i.c" /Oicf
# End Source File
# Begin Source File

SOURCE=.\mavspi.rc
# End Source File
# Begin Source File

SOURCE=.\servprov.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\storage.cpp
# End Source File
# Begin Source File

SOURCE=.\storglob.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon_hd.ico
# End Source File
# Begin Source File

SOURCE=.\maverick.ico
# End Source File
# Begin Source File

SOURCE=.\servprov.rgs
# End Source File
# End Group
# End Target
# End Project
