# Microsoft Developer Studio Project File - Name="FML" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FML - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FML.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FML.mak" CFG="FML - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FML - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FML - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/FML", HREHAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FML - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MTd /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Nafxcwd.lib Libcmtd.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "FML - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Nafxcwd.lib Libcmtd.lib /nologo /subsystem:windows /debug /machine:I386 /out:"InstallFiles/FML.exe"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "FML - Win32 Release"
# Name "FML - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\albumfolder.ico
# End Source File
# Begin Source File

SOURCE=.\res\arrowcop.cur
# End Source File
# Begin Source File

SOURCE=.\res\newGraphics\bitmap10.bmp
# End Source File
# Begin Source File

SOURCE=.\res\book03.ico
# End Source File
# Begin Source File

SOURCE=.\res\FML.ico
# End Source File
# Begin Source File

SOURCE=.\res\FML.rc2
# End Source File
# Begin Source File

SOURCE=.\res\ico00001.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon5.ico
# End Source File
# Begin Source File

SOURCE=.\res\icon7.ico
# End Source File
# Begin Source File

SOURCE=.\res\nodrop.cur
# End Source File
# Begin Source File

SOURCE=.\res\plainfolder.ico
# End Source File
# End Group
# Begin Group "FML Code"

# PROP Default_Filter ""
# Begin Group "ID3Lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\code\adler32.c
# End Source File
# Begin Source File

SOURCE=.\code\asferr.h
# End Source File
# Begin Source File

SOURCE=.\code\compress.c
# End Source File
# Begin Source File

SOURCE=.\code\crc32.c
# End Source File
# Begin Source File

SOURCE=.\code\deflate.c
# End Source File
# Begin Source File

SOURCE=.\code\deflate.h
# End Source File
# Begin Source File

SOURCE=.\code\gzio.c
# End Source File
# Begin Source File

SOURCE=.\code\httpserver.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_dll.h
# End Source File
# Begin Source File

SOURCE=.\code\id3_dll_wrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_error.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_error.h
# End Source File
# Begin Source File

SOURCE=.\code\id3_externals.h
# End Source File
# Begin Source File

SOURCE=.\code\id3_field.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_field.h
# End Source File
# Begin Source File

SOURCE=.\code\id3_field_binary.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_field_integer.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_field_string_ascii.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_field_string_unicode.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_frame.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_frame.h
# End Source File
# Begin Source File

SOURCE=.\code\id3_frame_parse.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_frame_render.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_header.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_header.h
# End Source File
# Begin Source File

SOURCE=.\code\id3_header_frame.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_header_frame.h
# End Source File
# Begin Source File

SOURCE=.\code\id3_header_tag.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_header_tag.h
# End Source File
# Begin Source File

SOURCE=.\code\id3_int28.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_int28.h
# End Source File
# Begin Source File

SOURCE=.\code\id3_misc_support.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_misc_support.h
# End Source File
# Begin Source File

SOURCE=.\code\id3_tag.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_tag.h
# End Source File
# Begin Source File

SOURCE=.\code\id3_tag_file.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_tag_find.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_tag_parse.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_tag_parse_lyrics3.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_tag_parse_v1.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_tag_render.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_tag_sync.cpp
# End Source File
# Begin Source File

SOURCE=.\code\id3_types.h
# End Source File
# Begin Source File

SOURCE=.\code\id3_version.h
# End Source File
# Begin Source File

SOURCE=.\code\infblock.c
# End Source File
# Begin Source File

SOURCE=.\code\infblock.h
# End Source File
# Begin Source File

SOURCE=.\code\infcodes.c
# End Source File
# Begin Source File

SOURCE=.\code\infcodes.h
# End Source File
# Begin Source File

SOURCE=.\code\inffast.c
# End Source File
# Begin Source File

SOURCE=.\code\inffast.h
# End Source File
# Begin Source File

SOURCE=.\code\inffixed.h
# End Source File
# Begin Source File

SOURCE=.\code\inflate.c
# End Source File
# Begin Source File

SOURCE=.\code\inftrees.c
# End Source File
# Begin Source File

SOURCE=.\code\inftrees.h
# End Source File
# Begin Source File

SOURCE=.\code\infutil.c
# End Source File
# Begin Source File

SOURCE=.\code\infutil.h
# End Source File
# Begin Source File

SOURCE=.\code\trees.c
# End Source File
# Begin Source File

SOURCE=.\code\trees.h
# End Source File
# Begin Source File

SOURCE=.\code\uncompr.c
# End Source File
# Begin Source File

SOURCE=.\code\zconf.h
# End Source File
# Begin Source File

SOURCE=.\code\zlib.h
# End Source File
# Begin Source File

SOURCE=.\code\zutil.c
# End Source File
# Begin Source File

SOURCE=.\code\zutil.h
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\code\global.cpp
# End Source File
# Begin Source File

SOURCE=.\code\global.h
# End Source File
# Begin Source File

SOURCE=.\code\httpserver.h
# End Source File
# Begin Source File

SOURCE=.\code\iml_upnp.cpp
# End Source File
# Begin Source File

SOURCE=.\code\iml_upnp.h
# End Source File
# Begin Source File

SOURCE=.\code\iml_xml.cpp
# End Source File
# Begin Source File

SOURCE=.\code\iml_xml.h
# End Source File
# Begin Source File

SOURCE=.\code\IPExport.h
# End Source File
# Begin Source File

SOURCE=.\code\IPHlpApi.h
# End Source File
# Begin Source File

SOURCE=.\code\IPTypes.h
# End Source File
# Begin Source File

SOURCE=.\code\mnetwork.cpp
# End Source File
# Begin Source File

SOURCE=.\code\network.cpp
# End Source File
# Begin Source File

SOURCE=.\code\network.h
# End Source File
# Begin Source File

SOURCE=.\code\nserror.h
# End Source File
# Begin Source File

SOURCE=.\code\parser.cpp
# End Source File
# Begin Source File

SOURCE=.\code\parser.h
# End Source File
# Begin Source File

SOURCE=.\code\publish.cpp
# End Source File
# Begin Source File

SOURCE=.\code\QueryResponses.cpp
# End Source File
# Begin Source File

SOURCE=.\code\responses.h
# End Source File
# Begin Source File

SOURCE=.\code\ssdp.cpp
# End Source File
# Begin Source File

SOURCE=.\code\ssdp.h
# End Source File
# Begin Source File

SOURCE=.\code\udevice.cpp
# End Source File
# Begin Source File

SOURCE=.\code\udevice.h
# End Source File
# End Group
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\code\ConfigDirectories.cpp
# End Source File
# Begin Source File

SOURCE=.\code\ConfigDirectories.h
# End Source File
# Begin Source File

SOURCE=.\code\IntRadio.cpp
# End Source File
# Begin Source File

SOURCE=.\code\IntRadio.h
# End Source File
# Begin Source File

SOURCE=.\code\PlayListDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\code\PlayListDlg.h
# End Source File
# Begin Source File

SOURCE=.\code\RefreshDatabaseDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\code\RefreshDatabaseDlg.h
# End Source File
# Begin Source File

SOURCE=.\code\ScanDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\ScanDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\TrayDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\TrayDialog.h
# End Source File
# Begin Source File

SOURCE=.\code\WaitingDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\code\WaitingDialog.h
# End Source File
# End Group
# Begin Group "Controls"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\code\DirTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\code\DirTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\code\MediaDragCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\code\MediaDragCtrl.h
# End Source File
# Begin Source File

SOURCE=.\code\MediaToPlayListDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\code\MediaToPlayListDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\code\MyRadioTree.cpp
# End Source File
# Begin Source File

SOURCE=.\code\MyRadioTree.h
# End Source File
# Begin Source File

SOURCE=.\code\PlaylistDragTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\code\PlaylistDragTarget.h
# End Source File
# Begin Source File

SOURCE=.\code\PlaylistTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\code\PlaylistTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\code\RadioTree.cpp
# End Source File
# Begin Source File

SOURCE=.\code\RadioTree.h
# End Source File
# Begin Source File

SOURCE=.\code\xSkinButton.cpp
# End Source File
# Begin Source File

SOURCE=.\code\xSkinButton.h
# End Source File
# End Group
# Begin Group "Utils"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\code\encode.cpp
# End Source File
# Begin Source File

SOURCE=.\code\encode.h
# End Source File
# Begin Source File

SOURCE=.\code\ShFolder.h
# End Source File
# Begin Source File

SOURCE=.\code\ShlObj.h

!IF  "$(CFG)" == "FML - Win32 Release"

!ELSEIF  "$(CFG)" == "FML - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\ShLwApi.h

!IF  "$(CFG)" == "FML - Win32 Release"

!ELSEIF  "$(CFG)" == "FML - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\code\SortStringArray.cpp
# End Source File
# Begin Source File

SOURCE=.\code\SortStringArray.h
# End Source File
# Begin Source File

SOURCE=.\code\uniqstr.cpp
# End Source File
# Begin Source File

SOURCE=.\code\uniqstrarr.h
# End Source File
# Begin Source File

SOURCE=.\code\util.cpp
# End Source File
# Begin Source File

SOURCE=.\code\util.h
# End Source File
# Begin Source File

SOURCE=.\code\wmsbuffer.h
# End Source File
# Begin Source File

SOURCE=.\code\wmsdk.h
# End Source File
# Begin Source File

SOURCE=.\code\wmsdkidl.h
# End Source File
# Begin Source File

SOURCE=.\code\xmlif.cpp
# End Source File
# Begin Source File

SOURCE=.\code\xmlif.h
# End Source File
# Begin Source File

SOURCE=.\code\xmlmain.cpp
# End Source File
# Begin Source File

SOURCE=.\code\xmlmain.h
# End Source File
# End Group
# Begin Group "Database"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\code\CMediaDatabase.cpp
# End Source File
# Begin Source File

SOURCE=.\code\CMediaFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\code\CMediaManager.cpp
# End Source File
# Begin Source File

SOURCE=.\code\CPlayListManager.cpp
# End Source File
# Begin Source File

SOURCE=.\code\CViewManager.cpp
# End Source File
# Begin Source File

SOURCE=.\code\mediahelper.h
# End Source File
# Begin Source File

SOURCE=.\code\medialibrary.h
# End Source File
# Begin Source File

SOURCE=.\code\metadata.cpp
# End Source File
# Begin Source File

SOURCE=.\code\metadata.h
# End Source File
# End Group
# End Group
# Begin Group "Libraries"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\code\WMVCORE.lib
# End Source File
# Begin Source File

SOURCE=.\code\ShFolder.Lib
# End Source File
# Begin Source File

SOURCE=.\code\ShLwApi.Lib
# End Source File
# Begin Source File

SOURCE=.\code\wmstub.lib
# End Source File
# Begin Source File

SOURCE=.\code\IPHlpApi.Lib
# End Source File
# End Group
# Begin Group "MFC framework"

# PROP Default_Filter ""
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\FML.cpp
# End Source File
# Begin Source File

SOURCE=.\FML.rc
# End Source File
# Begin Source File

SOURCE=.\FMLDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\FML.h
# End Source File
# Begin Source File

SOURCE=.\FMLDlg.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# End Group
# Begin Group "Version Info"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\code\version.h
# End Source File
# End Group
# End Target
# End Project
